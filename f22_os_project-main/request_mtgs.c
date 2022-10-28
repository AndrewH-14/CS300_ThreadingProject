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

void * request_thread(void * p_rbuf);

// ./request_mtgs | cat input.msg
int main(int argc, char *argv[])
{
    printf("%i\n", argc);
}

/**
 * Thread function that will add the message to the system 5 message queue
 * 
 * @param rbuf A meeting_request_buf struct containing the information that 
 *             needs to be sent.
 * 
 * @return 
 */
void * request_thread(void * p_rbuf)
{
    // Send messsage

    // Wait for response

}
