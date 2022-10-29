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

volatile int num_pending_messages = 0;
meeting_request_buf * requests;
int msqid;
int msgflg = IPC_CREAT | 0666;
key_t key;

int test_argc = 4;
char * test_argv[4];

void * send_request(void * p_request_id);

void * receive_response(void);

meeting_request_buf parse_request(char * p_request_string);

// ./request_mtgs | cat input.msg
int main(int argc, char *argv[])
{
    // Test argv used until stdin argv is figured out
    // TODO: remove this
    test_argv[0] = malloc(sizeof(char) * sizeof("./request_mtgs"));
    strncpy(test_argv[0], "./request_mtgs", strlen("./request_mtgs"));
    test_argv[1] = malloc(sizeof(char) * sizeof("1,1234,\"morning mtg\",\"conf room\",2022-12-17T14:30,60"));
    strncpy(test_argv[1], "1,1234,\"morning mtg\",\"conf room\",2022-12-17T14:30,60", strlen("1,1234,\"morning mtg\",\"conf room\",2022-12-17T14:30,60"));
    test_argv[2] = malloc(sizeof(char) * sizeof("1,1234,\"conflict mtg\",\"zoom\",2022-12-17T15:00,60"));
    strncpy(test_argv[2], "2,1234,\"conflict mtg\",\"zoom\",2022-12-17T15:00,60", strlen("1,1234,\"conflict mtg\",\"zoom\",2022-12-17T15:00,60"));
    test_argv[3] = malloc(sizeof(char) * sizeof("0,9999,\"any\",\"any\",any,60"));
    strncpy(test_argv[3], "0,9999,\"any\",\"any\",any,60", strlen("0,9999,\"any\",\"any\",any,60"));
    // End of test code

    // Pthread variables
    pthread_t p;

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

    // Create a dynamically sized buffer to store the request information using argc
    requests = (meeting_request_buf *) malloc(sizeof(meeting_request_buf) * (test_argc - 2));

    // Create a loop that passes the meeting request struct to the thread
    for (int current_message = 1; current_message < test_argc; current_message++)
    {
        // printf("Creating a thread for: %s\n", test_argv[current_message]);
        meeting_request_buf rbuf  = parse_request(test_argv[current_message]);
        requests[rbuf.request_id] = rbuf;
        if (rbuf.request_id > 0)
        {
            // Use the globally stored argument to ensure that no race condition is encountered
            pthread_create(&p, NULL, send_request, (void *) &requests[rbuf.request_id].request_id);
            num_pending_messages++;
        }
    }

    // Create multiple receiver threads that will wait for responses to be sent
    for (int idx = 0; idx < num_pending_messages; idx++)
    {
        printf("Create a client thread that will handle incoming messages.");
    }
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
void * send_request(void * p_request_id)
{
    int request_id = *(int *)p_request_id;

    meeting_request_buf rbuf = requests[request_id];

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

    return NULL;
}

/**
 * Thread function that will 
 */
void * receive_response(void)
{
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
        strtok(p_request_string, deliminter),
        EMP_ID_MAX_LENGTH
    );
    strncpy(
        rbuf.description_string,
        strtok(p_request_string, deliminter),
        DESCRIPTION_MAX_LENGTH
    );
    strncpy(
        rbuf.location_string,
        strtok(p_request_string, deliminter),
        LOCATION_MAX_LENGTH
    );
    strncpy(
        rbuf.datetime,
        strtok(p_request_string, deliminter),
        DATETIME_LENGTH
    );
    rbuf.duration=atoi(strtok(p_request_string, deliminter));

    return rbuf;
}
