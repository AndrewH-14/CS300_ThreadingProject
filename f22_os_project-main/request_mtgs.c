#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include "meeting_request_formats.h"
#include "queue_ids.h"

int msqid;
int msgflg = IPC_CREAT | 0666;
key_t key;
char inputLine[100] = { 0 };
sem_t lock;

void * send_request(void * p_rbuf);
void * receive_response();

meeting_request_buf parse_request(char * p_request_string);

// ./request_mtgs | cat input.msg
int main(int argc, char *argv[])
{
    // Pthread variables
    pthread_t p;
    sem_init(&lock, 0, 1);

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
        sem_wait(&lock);
        pthread_create(&p, NULL, send_request, (void *) &rbuf);
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
    sem_post(&lock);

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
void * receive_response()
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