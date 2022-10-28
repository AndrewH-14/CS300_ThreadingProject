#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#include "meeting_request_formats.h"
#include "queue_ids.h"

int   msqid;
int   msgflg;
key_t key;

void * msg_send();

void * msg_rcv();



int main(int argc, char *argv[])
{
    printf("%s\n", argv[0]);
    printf("%s\n", argv[1]);
    printf("%s\n", argv[2]);
    printf("%s\n", argv[3]);

    return 0;
}

/**
 * Thread function that will add the message to the system 5 message queue
 * 
 * @param rbuf A meeting_request_buf struct containing the information that 
 *             needs to be sent.
 * 
 * @return 
 */
void * msg_send(void * p_rbuf)
{

}

/**
 * Thread function that will wait for responses for the sent messages
 */
void * msg_rcv(void)
{
    
}