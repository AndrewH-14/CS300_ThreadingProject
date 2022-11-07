#include <assert.h>
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

// Macro to convert max length to a string for use in scanf
// https://stackoverflow.com/a/3301451
#define STRINGIFY(x) STRINGIFY2(x)
#define STRINGIFY2(x) #x

#define MAX_INPUT_NUMBER 200

// System V message queue variables
// Global so that the multiple different thread functions can easily
// access the values when sending/receiving System V messages.
int msqid;
int msgflg = IPC_CREAT | 0666;
key_t key;

// Mutexes that will be used to ensure the application is thread safe.
// Global since multiple threads and functions need access to this address
pthread_mutex_t rbuf_copy_mutex;
pthread_mutex_t handle_response_mutex;
pthread_mutex_t send_last_msg_mutex;

// Conditional variables that will assist in making the application thread safe
// Global since muliple threads and functions need access to this address
pthread_cond_t  copy_cond              = PTHREAD_COND_INITIALIZER;
pthread_cond_t  ok_to_send_end_message = PTHREAD_COND_INITIALIZER;

// Global flags and variables that are needed by multiple different threads
volatile int  requests_to_be_sent = 0;
volatile bool copy_complete       = false;

// Global arrays regarding request responses that the threads will use to 
// support concurrency
meeting_response_buf responses[200] = {{ 0 }};
pthread_cond_t response_conds[200];

// Thread functions that will be called by pthread_create()
void * request_thread(void * p_rbuf);
void * receiver_thread();

// Helper functions
void remove_quotes(char * p_request_string, size_t size);
bool init_queue(void);

/**
 * The main function for request_mtgs.c
 */
int main(int argc, char *argv[])
{
    // Array to store all of the possible threads that will be created.
    // MAX_INPUT_ARRAY + 1 due to the request_id == 0 message
    pthread_t threads[MAX_INPUT_NUMBER + 1];

    // Meeting request struct that will store the parsed information from stdin
    meeting_request_buf rbuf;

    // Used to keep track of how many threads we need to wait on
    int num_created_threads = 0;
    // Set to -1 to get past the first while loop check
    rbuf.request_id         = -1;

    // Initialize pthread mutex variables that will be used to support concurrency
    assert(0 == pthread_mutex_init(&handle_response_mutex, NULL));
    assert(0 == pthread_mutex_init(&send_last_msg_mutex, NULL));
    assert(0 == pthread_mutex_init(&rbuf_copy_mutex, NULL));

    // Initialize the pthread conditional variables that the request threads will wait on.
    for (int idx = 0; idx < 200; idx++)
    {
        assert(0 == pthread_cond_init(&response_conds[idx], NULL));
    }

    // Initialize queue and exit if it fails
    if (!init_queue()) { return 1; }

    // Create a single thread to read the responses from the message queue
    pthread_t p;
    assert(0 == pthread_create(&p, NULL, receiver_thread, NULL));

    // Loop until we have received the last request to be sent.
    while (rbuf.request_id != 0)
    {
        // Read in the request from stdin and directly add it to
        // the meeting request struct
        // https://stackoverflow.com/a/15091406
        rbuf.mtype = 2;
        scanf("%d,%"
              STRINGIFY(EMP_ID_MAX_LENGTH)"[^,],%"
              STRINGIFY(DESCRIPTION_MAX_LENGTH)"[^,],%"
              STRINGIFY(LOCATION_MAX_LENGTH)"[^,],%"
              STRINGIFY(DATETIME_LENGTH)"[^,],%d",
              &rbuf.request_id, 
              rbuf.empId, 
              rbuf.description_string, 
              rbuf.location_string, 
              rbuf.datetime, 
              &rbuf.duration);
        remove_quotes(rbuf.location_string, LOCATION_FIELD_LENGTH);
        remove_quotes(rbuf.description_string, DESCRIPTION_FIELD_LENGTH);

        assert(0 == pthread_mutex_lock(&send_last_msg_mutex));
        requests_to_be_sent++;
        assert(0 == pthread_mutex_unlock(&send_last_msg_mutex));

        #ifdef DEBUG 
            fprintf(stderr, "Creating thread for request %d\n", rbuf.request_id);
        #endif

        // Must wait for the rbuf value to be copied over to the thread before creating the next one
        // https://stackoverflow.com/questions/19372973/passing-multiple-variables-to-pthread-in-a-loop-arguments-get-overwritten-next
        assert(0 == pthread_mutex_lock(&rbuf_copy_mutex));
        assert(0 == pthread_create(&threads[rbuf.request_id - 1], NULL, request_thread, (void *) &rbuf));
        while (!copy_complete)
        {
            assert(0 == pthread_cond_wait(&copy_cond, &rbuf_copy_mutex));
        }
        copy_complete = false;
        assert(0 == pthread_mutex_unlock(&rbuf_copy_mutex));

        #ifdef DEBUG
            fprintf(stderr, "Copy for request %d successfully completed\n", rbuf.request_id);
        #endif

        // Increment the number of request threads that have been started.
        // This will be used to know how many threads to join after all 
        // of the requests have been received.
        num_created_threads++;
    }

    // Once all of the request threads have ended, it is safe to exit the program
    for (int thread_idx = 0; thread_idx < num_created_threads - 1; thread_idx++)
    {
        assert(0 == pthread_join(threads[thread_idx], NULL));
    }

    return 0;
}

/**
 * Thread function that will add the message to the system 5 message queue
 * 
 * @param p_rbuf A pointer to the message request struct to be sent.
 * 
 * @return NULL
 */
void * request_thread(void * p_rbuf)
{
    // Copy the rbuf to this threads stack, and then signal to the main
    // thread that it can continue.
    assert(0 == pthread_mutex_lock(&rbuf_copy_mutex));
    meeting_request_buf rbuf = *(meeting_request_buf *) p_rbuf;
    copy_complete = true;
    assert(0 == pthread_cond_signal(&copy_cond));
    assert(0 == pthread_mutex_unlock(&rbuf_copy_mutex));

    // If attempting to send the final message, first check to make sure there are
    // no other pending messages to be sent in other threads.
    assert(0 == pthread_mutex_lock(&send_last_msg_mutex));
    if ((rbuf.request_id == 0) && (requests_to_be_sent > 1))
    {
        assert(0 == pthread_cond_wait(&ok_to_send_end_message, &send_last_msg_mutex));
    }
    assert(0 == pthread_mutex_unlock(&send_last_msg_mutex));

    // Send a message.
    if((msgsnd(msqid, &rbuf, SEND_BUFFER_LENGTH, IPC_NOWAIT)) < 0) {
        int errnum = errno;
        fprintf(stderr,
                "%d, %ld, %d, %ld\n", 
                msqid, rbuf.mtype, rbuf.request_id, SEND_BUFFER_LENGTH);
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
    assert(0 == pthread_mutex_lock(&send_last_msg_mutex));
    requests_to_be_sent--;
    assert(0 == pthread_cond_signal(&ok_to_send_end_message));
    assert(0 == pthread_mutex_unlock(&send_last_msg_mutex));

    // If the request expects a response, block until it is received
    if (rbuf.request_id != 0)
    {
        // Lock the received mutex to ensure that all of the shared variables are accurate
        // when accessed.
        assert(0 == pthread_mutex_lock(&handle_response_mutex));
        while(responses[rbuf.request_id - 1].request_id == 0) //< Wait until the response has been received
        {
            // Put this thread to sleep until the receiver signals that a new response has been received,
            // and this request is the next one to be printed.
            assert(0 == pthread_cond_wait(&response_conds[rbuf.request_id - 1], &handle_response_mutex));
        }
        assert(0 == pthread_mutex_unlock(&handle_response_mutex));

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
void * receiver_thread()
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
            // Then signal to the thread that it's response has been received.
            assert(0 == pthread_mutex_lock(&handle_response_mutex));
            responses[rbuf.request_id - 1] = rbuf;
            assert(0 == pthread_cond_signal(&response_conds[rbuf.request_id - 1]));
            assert(0 == pthread_mutex_unlock(&handle_response_mutex));

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
            fprintf(stderr,"msgrcv-mtgReqResponse: request id %d  avail %d: \n",
                    rbuf.request_id,rbuf.avail);
        #endif
    }

    return NULL;
}

/**
 * Removes the leading and trailing quotes from the given string.
 * 
 * @param[out] p_string A string that with leading and trailing quotation
 *                      marks to be removed.
 * @param      size     The size of the buffer that the string is stored
 *                      in.
 */
void remove_quotes(char * p_string, size_t size)
{
    memmove(p_string, &p_string[1], size - 1);
    for (int char_idx = 0; char_idx < size - 1; char_idx++)
    {
        if (p_string[char_idx + 1] == '\0')
        {
            p_string[char_idx] = '\0';
            break;
        }
    }
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