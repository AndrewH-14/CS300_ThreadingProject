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

#define MAX_INPUT_NUMBER 200

//#define DEBUG

// Variables used to determine the system 5 queue that will be used
int msqid;
int msgflg = IPC_CREAT | 0666;
key_t key;

// Phread variables that are used to ensure concurrency works correctly
pthread_mutex_t send_mutex;
pthread_mutex_t rbuf_copy_mutex;
pthread_mutex_t handle_response_mutex;
pthread_mutex_t send_last_msg_mutex;

pthread_cond_t  copy_cond              = PTHREAD_COND_INITIALIZER;
pthread_cond_t  ok_to_send_end_message = PTHREAD_COND_INITIALIZER;
volatile bool   copy_complete          = false;

// Global variables that will store information regarding responses
volatile int current_response        = 0;
volatile int requests_to_be_sent     = 0;

// Global arrays regarding request responses that the threads will use to 
// support concurrency
meeting_response_buf responses[200] = {{ 0 }};
pthread_cond_t response_conds[200];

// Thread functions that will be called by pthread_create()
void * send_request(void * p_rbuf);
void * receive_response();

// Helper functions
meeting_request_buf parse_request(char * p_request_string);
bool init_queue(void);

/**
 * The main function for request_mtgs.c
 */
int main(int argc, char *argv[])
{
    // An array to a line from stdin when reading in requests
    char input_line[MAX_LINE_LENGTH] = { 0 };

    // Used to keep track of how many threads we need to wait on
    int num_created_threads = 0;
    meeting_request_buf rbuf;
    rbuf.request_id = -1;
    pthread_t threads[MAX_INPUT_NUMBER + 1];
    current_response = 0;

    // Initialize pthread mutex variables that will be used to support concurrency
    pthread_mutex_init(&send_mutex, NULL);
    pthread_mutex_init(&handle_response_mutex, NULL);
    pthread_mutex_init(&send_last_msg_mutex, NULL);
    pthread_mutex_init(&rbuf_copy_mutex, NULL);

    // Initialize the pthread conditional variables that the threads will wait on.
    for (int idx = 0; idx < 200; idx++)
    {
        pthread_cond_init(&response_conds[idx], NULL);
    }

    // Initialize queue and exit if it fails
    if (!init_queue()) { return 1; }

    // Create a single thread to read the responses from the message queue
    pthread_t receiver_thread;
    pthread_create(&receiver_thread, NULL, receive_response, NULL);

    // Loop until we have received the last request to be sent.
    while (rbuf.request_id != 0)
    {
        // Get the next line from stdin
        fgets(input_line, MAX_LINE_LENGTH, stdin);
        #ifdef DEBUG
            fprintf(stderr, "line read: %s", input_line);
        #endif

        // Convert the read string into a request buf to be sent
        rbuf = parse_request(input_line);

        pthread_mutex_lock(&send_last_msg_mutex);
        requests_to_be_sent++;
        pthread_mutex_unlock(&send_last_msg_mutex);

        #ifdef DEBUG 
            fprintf(stderr, "Creating thread for request %d\n", rbuf.request_id);
        #endif

        // Must wait for the rbuf value to be copied over to the thread before creating the next one
        pthread_mutex_lock(&rbuf_copy_mutex);
        pthread_create(&threads[rbuf.request_id - 1], NULL, send_request, (void *) &rbuf);
        while (!copy_complete)
        {
            pthread_cond_wait(&copy_cond, &rbuf_copy_mutex);
        }
        copy_complete = false;
        pthread_mutex_unlock(&rbuf_copy_mutex);

        #ifdef DEBUG
            fprintf(stderr, "Copy for request %d successfully completed\n", rbuf.request_id);
        #endif

        num_created_threads++;
    }

    // Ensure that all threads have ended before closing
    for (int thread_idx = 0; thread_idx < num_created_threads; thread_idx++)
    {
        pthread_join(threads[thread_idx], NULL);
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
    // Copy the rbuf to this threads stack, and then signal to the main
    // thread that it can continue.
    pthread_mutex_lock(&rbuf_copy_mutex);
    meeting_request_buf rbuf = *(meeting_request_buf *) p_rbuf;
    copy_complete = true;
    pthread_cond_signal(&copy_cond);
    pthread_mutex_unlock(&rbuf_copy_mutex);

    // If attempting to send the final message, first check to make sure there are
    // no other pending messages to be sent in other threads.
    pthread_mutex_lock(&send_last_msg_mutex);
    if ((rbuf.request_id == 0) && (requests_to_be_sent > 1))
    {
        pthread_cond_wait(&ok_to_send_end_message, &send_last_msg_mutex);
    }
    pthread_mutex_unlock(&send_last_msg_mutex);

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

    // Update the number of requests that still need to be sent and send a signal
    // in case the last message was blocked. If no thread is waiting for this signal
    // nothing should happen.
    pthread_mutex_lock(&send_last_msg_mutex);
    requests_to_be_sent--;
    pthread_cond_signal(&ok_to_send_end_message);
    pthread_mutex_unlock(&send_last_msg_mutex);

    // If the request expects a response, block until it is received
    if (rbuf.request_id != 0)
    {
        // Lock the received mutex to ensure that all of the shared variables are accurate
        // when accessed. This also needs to include the print statements to ensure that
        // they are printed before allowing another thread to execute, creating a race
        // condition as to which print statement executes first.
        pthread_mutex_lock(&handle_response_mutex);
        while(responses[rbuf.request_id - 1].request_id == 0) //< Wait until the response has been received
        {
            // Put this thread to sleep until the receiver signals that a new response has been received,
            // and this request is the next one to be printed.
            pthread_cond_wait(&response_conds[rbuf.request_id - 1], &handle_response_mutex);
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
        printf("%d\n", current_response);

        // If the next response has already been received, then signal the thread to wake up
        if ((current_response < MAX_INPUT_NUMBER) && (responses[current_response].request_id != 0))
        {
            pthread_cond_signal(&response_conds[current_response]);
        }
        pthread_mutex_unlock(&handle_response_mutex);
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

    while (true)
    {
        do 
        {
            // Block until a message type 1 message is received
            ret = msgrcv(msqid, &rbuf, sizeof(rbuf)-sizeof(long), 1, 0);

            // Store the reponse in the global array so that it can be accessed by the threads.
            // Then signal to the thread that it's response has been received. This must be in
            // a locked section to avoid a race condition where current_response is about to be
            // updated by another thread.
            pthread_mutex_lock(&handle_response_mutex);
            responses[rbuf.request_id - 1] = rbuf;
            if ((rbuf.request_id - 1) == current_response)
            {
                pthread_cond_signal(&response_conds[current_response]);
            }
            pthread_mutex_unlock(&handle_response_mutex);

            int errnum = errno;
            if (ret < 0 && errno !=EINTR)
            {
                fprintf(stderr, "Value of errno: %d\n", errno);
                perror("Error printed by perror");
                fprintf(stderr, "Error receiving msg: %s\n", strerror( errnum ));
            }
        } 
        while ((ret < 0 ) && (errno == 4));
#ifdef DEBUG
            fprintf(stderr,"msgrcv-mtgReqResponse: request id %d  avail %d: \n",rbuf.request_id,rbuf.avail);
#endif
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
        DESCRIPTION_FIELD_LENGTH
    );
    memmove(rbuf.description_string, &rbuf.description_string[1], DESCRIPTION_FIELD_LENGTH - 1);
    for (int char_idx = 0; char_idx < DESCRIPTION_FIELD_LENGTH - 1; char_idx++)
    {
        if (rbuf.description_string[char_idx + 1] == '\0')
        {
            rbuf.description_string[char_idx] = '\0';
            break;
        }
    }
    strncpy(
        rbuf.location_string,
        strtok(NULL, deliminter),
        LOCATION_FIELD_LENGTH
    );
    memmove(rbuf.location_string, &rbuf.location_string[1], LOCATION_FIELD_LENGTH - 1);
    for (int char_idx = 0; char_idx < LOCATION_FIELD_LENGTH - 1; char_idx++)
    {
        if (rbuf.location_string[char_idx + 1] == '\0')
        {
            rbuf.location_string[char_idx] = '\0';
            break;
        }
    }
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

/**
 * Function that initializes the System V message queue that will be used
 * to send and receive data. 
 * 
 * @retval true  Success
 * @retval false Failure
 */
bool init_queue(void)
{
    // Set up the system 5 queue that will be used to send and receive messages
    // Note: Code copied from msgsnd_mtg_request.c
    key = ftok(FILE_IN_HOME_DIR,QUEUE_NUMBER);
    if (key == 0xffffffff) {
        fprintf(stderr,"Key cannot be 0xffffffff..fix queue_ids.h to link to existing file\n");
        return false;
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
    return true;
}