#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include "meeting_request_formats.h"
#include "queue_ids.h"

// Variables used to determine the system 5 queue that will be used
int msqid;
int msgflg = IPC_CREAT | 0666;
key_t key;

// An array to a line from stdin when reading in requests
// TODO: Compute the maximum length
char inputLine[100] = { 0 };

// Phread varibales that are used to ensure concurrency works correctly
pthread_mutex_t send_mutex;
pthread_mutex_t receive_mutex;
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;
volatile int done = 0;
volatile int number_of_responses = 0;

// A global thread of responses so that any thread can view it
meeting_response_buf responses[200]      = { 0 };
pthread_cond_t       response_conds[200] = { PTHREAD_COND_INITIALIZER };
int                  cond_variables[200] = { 0 };

// Thread functions that will be called by pthread_create()
void * send_request(void * p_rbuf);
void * receive_response();

// Helper functions
meeting_request_buf parse_request(char * p_request_string);

/**
 * The main function for request_mtgs.c
 * 
 * @return 0 
 */
int main(int argc, char *argv[])
{
    // Pthread variables
    pthread_t p;
    pthread_mutex_init(&send_mutex, NULL);
    pthread_mutex_init(&receive_mutex, NULL);

    // Initialize the system 5 queue variables
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

    while (fgets(inputLine, 100, stdin) != NULL)
    {
        // printf("%s", inputLine);
        meeting_request_buf rbuf = parse_request(inputLine);
        pthread_mutex_lock(&send_mutex);
        pthread_create(&p, NULL, send_request, (void *) &rbuf);
        while (done == 0)
            pthread_cond_wait(&cond, &send_mutex);
        pthread_mutex_unlock(&send_mutex);
        done = 0;
        number_of_responses++;
    }

    // Create a single thread to read the responses from the message queue
    pthread_t receiver_thread;
    pthread_create(&receiver_thread, NULL, receive_response, NULL);

    // Once the reciever thread exits, we have received all responses, and can
    // exit the program
    pthread_join(receiver_thread, NULL);

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
    done = 1;
    pthread_cond_signal(&cond);

    // Send a message.
    if((msgsnd(msqid, &rbuf, SEND_BUFFER_LENGTH, IPC_NOWAIT)) < 0) {
        int errnum = errno;
        fprintf(stderr,"%d, %ld, %d, %ld\n", msqid, rbuf.mtype, rbuf.request_id, SEND_BUFFER_LENGTH);
        perror("(msgsnd)");
        fprintf(stderr, "Error sending msg: %s\n", strerror( errnum ));
        exit(1);
    }
    else
    {
        fprintf(stderr,"msgsnd--mtg_req: reqid %d empid %s descr %s loc %s date %s duration %d \n",
        rbuf.request_id,rbuf.empId,rbuf.description_string,rbuf.location_string,rbuf.datetime,rbuf.duration);
    }

    // Wait for the thread to be signalled that it's thread has been received,
    // and that it is it's turn to print the response
    pthread_mutex_lock(&receive_mutex);
    while (cond_variables[rbuf.request_id - 1] == 0)
        pthread_cond_wait(&response_conds[rbuf.request_id - 1], &receive_mutex);
    pthread_mutex_unlock(&receive_mutex);


    // Print the reponse (we can now access the response since no other thread will edit it)
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
    return NULL;
}

/**
 * Thread function that will 
 */
void * receive_response()
{
    // msgrcv to receive mtg request response type 1
    int ret;
    meeting_response_buf rbuf = { 0 };

    // A response will not be sent for a message with request_id == 0
    number_of_responses = number_of_responses - 1;

    while (number_of_responses > 0)
    {
        do 
        {
            ret = msgrcv(msqid, &rbuf, sizeof(rbuf)-sizeof(long), 1, 0);//receive type 1 message

            responses[rbuf.request_id - 1] = rbuf;
            cond_variables[rbuf.request_id - 1] = 1;
            pthread_cond_signal(&response_conds[rbuf.request_id - 1]);

            int errnum = errno;
            if (ret < 0 && errno !=EINTR)
            {
                fprintf(stderr, "Value of errno: %d\n", errno);
                perror("Error printed by perror");
                fprintf(stderr, "Error receiving msg: %s\n", strerror( errnum ));
            }
        } 
        while ((ret < 0 ) && (errno == 4) && (number_of_responses > 0));

        fprintf(stderr,"msgrcv-mtgReqResponse: request id %d  avail %d: \n",rbuf.request_id,rbuf.avail);
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

    return rbuf;
}