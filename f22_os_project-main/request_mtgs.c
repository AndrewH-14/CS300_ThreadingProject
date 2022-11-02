#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include "meeting_request_formats.h"
#include "queue_ids.h"

// Macros to determine the maximum size that an input string can be
#define REQUEST_ID_LENGTH 3
#define DURATION_LENGTH   3
#define COMMA_DELIMITERS  5
#define MAX_LINE_LENGTH   (REQUEST_ID_LENGTH      + \
                           EMP_ID_MAX_LENGTH      + \
                           DESCRIPTION_MAX_LENGTH + \
                           LOCATION_MAX_LENGTH    + \
                           DURATION_LENGTH        + \
                           COMMA_DELIMITERS)

// Variables used to determine the system 5 queue that will be used
int msqid;
int msgflg = IPC_CREAT | 0666;
key_t key;

// An array to a line from stdin when reading in requests
char input_line[MAX_LINE_LENGTH] = { 0 };

// Phread variables that are used to ensure concurrency works correctly
pthread_mutex_t send_mutex;
pthread_mutex_t receive_mutex;

pthread_cond_t  copy_cond     = PTHREAD_COND_INITIALIZER;
volatile bool   copy_complete = false;

pthread_t request_threads[200];

// Global variables that will store information regarding responses
volatile int number_of_responses = 0;
volatile int current_response    = 0;

// Global arrays regarding request responses that the threads will use to 
// support concurrency
meeting_response_buf responses[200] = { 0 };
pthread_cond_t response_conds[200];

// Thread functions that will be called by pthread_create()
void * send_request(void * p_rbuf);
void * receive_response();

// Helper functions
meeting_request_buf parse_request(char * p_request_string);

/**
 * The main function for request_mtgs.c
 */
int main(int argc, char *argv[])
{
    int number_of_requests = 0;

    // Initialize pthread mutex variables that will be used to support concurrency
    pthread_mutex_init(&send_mutex, NULL);
    pthread_mutex_init(&receive_mutex, NULL);

    // Initialize the pthread conditional variables that the threads will wait on.
    for (int idx = 0; idx < 200; idx++)
    {
        pthread_cond_init(&response_conds[idx], NULL);
    }

    // Set up the system 5 queue that will be used to send and receive messages
    // Note: Code copied from msgsnd_mtg_request.c
    key = ftok(FILE_IN_HOME_DIR,QUEUE_NUMBER);
    if (key == 0xffffffff) {
        fprintf(stderr,"Key cannot be 0xffffffff..fix queue_ids.h to link to existing file\n");
        return 1;
    }
    if ((msqid = msgget(key, msgflg)) < 0) {
        int errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("(msgget)");
        fprintf(stderr, "Error msgget: %s\n", strerror( errnum ));
    }
#ifdef DEBUG
    else
        fprintf(stderr, "msgget: msgget succeeded: msgqid = %d\n", msqid);
#endif

    // Create a thread that will send a meeting request per line of stdin
    while (fgets(input_line, MAX_LINE_LENGTH, stdin) != NULL)
    {
#ifdef DEBUG
        fprintf(stderr, "line read: %s", input_line);
#endif
        meeting_request_buf rbuf = parse_request(input_line);

        // Must use a mutex lock to ensure that the passed argument is copied over
        // to the thread's stack before overwritting the value
        pthread_mutex_lock(&send_mutex);
#ifdef DEBUG 
        fprintf(stderr, "Creating thread for request %d\n", rbuf.request_id);
#endif
        pthread_create(&request_threads[number_of_responses], NULL, send_request, (void *) &rbuf);
        while (!copy_complete)
        {
            pthread_cond_wait(&copy_cond, &send_mutex);
        }
#ifdef DEBUG
        fprintf(stderr, "Copy for request %d successfully completed\n", rbuf.request_id);
#endif
        copy_complete = false;
        pthread_mutex_unlock(&send_mutex);

        // Record the number of responses that have been received. This value
        // will let us know when we have received all of the responses, and can
        // exit the program. This value will be decremented as responses are 
        // received.
        number_of_responses++;

        // Record the number of requests made. This will be used to ensure that
        // all responses are recorded. This value will NOT be decremented.
        number_of_requests++;
    }

    // Create a single thread to read the responses from the message queue
    pthread_t receiver_thread;
    pthread_create(&receiver_thread, NULL, receive_response, NULL);

    // Once all of the request threads have completed, we know that all of the 
    // reponses have been received and printed, therefore we can now exit the
    // program
    for (int request_thread_idx = 0; request_thread_idx < number_of_requests; request_thread_idx++)
    {
        pthread_join(request_threads[request_thread_idx], NULL);
    }

    return 0;
}

/**
 * Thread function that will add the message to the system 5 message queue
 * 
 * @param p_request_id A pointer to an integer that will be used to index the
 *                     requests buffer in order to get the information that
 *                     needs to be sent.
 * 
 * @return NULL
 */
void * send_request(void * p_rbuf)
{
    meeting_request_buf rbuf = *(meeting_request_buf *) p_rbuf;
    pthread_mutex_lock(&send_mutex);
    copy_complete = true;
    pthread_mutex_unlock(&send_mutex);
    pthread_cond_signal(&copy_cond);

    // Send a message.
    if((msgsnd(msqid, &rbuf, SEND_BUFFER_LENGTH, IPC_NOWAIT)) < 0) {
        int errnum = errno;
        fprintf(stderr,"%d, %ld, %d, %ld\n", msqid, rbuf.mtype, rbuf.request_id, SEND_BUFFER_LENGTH);
        perror("(msgsnd)");
        fprintf(stderr, "Error sending msg: %s\n", strerror( errnum ));
        exit(1);
    }
#ifdef DEBUG
    else
    {
        fprintf(stderr,"msgsnd--mtg_req: reqid %d empid %s descr %s loc %s date %s duration %d \n",
                rbuf.request_id,rbuf.empId,rbuf.description_string,rbuf.location_string,rbuf.datetime,rbuf.duration);
    }
#endif
    // If the request expects a response, block until it is received
    if (rbuf.request_id != 0)
    {
        // Lock the received mutex to ensure that all of the shared variables are accurate
        // when accessed. This also needs to include the print statements to ensure that
        // they are printed before allowing another thread to execute, creating a race
        // condition as to which print statement executes first.
        pthread_mutex_lock(&receive_mutex);
        while(responses[rbuf.request_id - 1].request_id == 0) //< Wait until the response has been received
        {
            // Put this thread to sleep until the receiver signals that a new response has been received,
            // and this request is the next one to be printed.
            pthread_cond_wait(&response_conds[rbuf.request_id - 1], &receive_mutex);
        }

        if (responses[rbuf.request_id - 1].avail == 1)
        {
            printf("Meeting request %d for employee %s was accepted (%s @ %s starting %s for %d minutes\n", 
                rbuf.request_id, rbuf.empId, rbuf.description_string, rbuf.location_string, rbuf.datetime, rbuf.duration);
        }
        else
        {
            printf("Meeting request %d for employee %s was rejected due to conflict (%s @ %s starting %s for %d minutes\n", 
                rbuf.request_id, rbuf.empId, rbuf.description_string, rbuf.location_string, rbuf.datetime, rbuf.duration);
        }

        // Update the response that needs to be printed next
        current_response++;

        // If the next response has already been received, then signal the thread to wake up
        if (responses[current_response].request_id != 0)
        {
            pthread_cond_signal(&response_conds[current_response]);
        }
        pthread_mutex_unlock(&receive_mutex);
    }
    return NULL;
}

/**
 * Thread function that will loop until reponses to all the requests have been
 * received.
 * 
 * @note This thread will signal to the request threads via conditional variables.
 * 
 * @return NULL
 */
void * receive_response()
{
    // msgrcv to receive mtg request response type 1
    int ret;
    meeting_response_buf rbuf = { 0 };

    // A response will not be sent for a message with request_id == 0
    number_of_responses = number_of_responses - 1;

    // Loop until we have received a response for each request sent
    while (number_of_responses > 0)
    {
        do 
        {
            // Block until a message type 1 message is received
            ret = msgrcv(msqid, &rbuf, sizeof(rbuf)-sizeof(long), 1, 0);

            // Store the reponse in the global array so that it can be accessed by the threads.
            // Then signal to the thread that it's response has been received. This must be in
            // a locked section in case a thread is attempting to update current_response.
            pthread_mutex_lock(&receive_mutex);
            responses[rbuf.request_id - 1] = rbuf;
            pthread_cond_signal(&response_conds[current_response]);
            pthread_mutex_unlock(&receive_mutex);

            int errnum = errno;
            if (ret < 0 && errno !=EINTR)
            {
                fprintf(stderr, "Value of errno: %d\n", errno);
                perror("Error printed by perror");
                fprintf(stderr, "Error receiving msg: %s\n", strerror( errnum ));
            }
        } 
        while ((ret < 0 ) && (errno == 4) && (number_of_responses > 0));

#ifdef DEBUG
        fprintf(stderr,"msgrcv-mtgReqResponse: request id %d  avail %d: \n",rbuf.request_id,rbuf.avail);
#endif
        // Now waiting for one less reponse
        number_of_responses--;
    }

    return NULL;
}

/**
 * Parses out the request information from the request string.
 * 
 * @param p_request_string A pointer to a string that contains a requests
 *                         information. For example: 
 *                         1,1234,"morning mtg","conf room",2022-12-17T14:30,60
 * 
 * @return A meeting_request_buffer object containing the parsed information.
 */
meeting_request_buf parse_request(char * p_request_string)
{
    meeting_request_buf rbuf = { 0 };
    const char * deliminter = ",";

    rbuf.mtype = 2;
    rbuf.request_id=atoi(strtok(p_request_string, deliminter));
    strncpy(
        rbuf.empId,
        strtok(NULL, deliminter),
        EMP_ID_MAX_LENGTH
    );
    strncpy(
        rbuf.description_string,
        strtok(NULL, deliminter),
        DESCRIPTION_MAX_LENGTH
    );
    strncpy(
        rbuf.location_string,
        strtok(NULL, deliminter),
        LOCATION_MAX_LENGTH
    );
    strncpy(
        rbuf.datetime,
        strtok(NULL, deliminter),
        DATETIME_LENGTH
    );
    rbuf.duration=atoi(strtok(NULL, deliminter));

#ifdef DEBUG
    fprintf(stderr, "Parsed information: request_id %d, empId %s, desc %s, loc %s, datetime %s, duration %d\n",
            rbuf.request_id, rbuf.empId, rbuf.description_string, rbuf.location_string, rbuf.datetime, rbuf.duration);
#endif

    return rbuf;
}