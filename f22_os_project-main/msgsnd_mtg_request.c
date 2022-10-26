#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "meeting_request_formats.h"
#include "queue_ids.h"

#ifndef darwin
size_t                  /* O - Length of string */
strlcpy(char       *dst,        /* O - Destination string */
        const char *src,      /* I - Source string */
        size_t      size)     /* I - Size of destination string buffer */
{
    size_t    srclen;         /* Length of source string */


    /*
     * Figure out how much room is needed...
     */

    size --;

    srclen = strlen(src);

    /*
     * Copy the appropriate amount...
     */

    if (srclen > size)
        srclen = size;

    memcpy(dst, src, srclen);
    dst[srclen] = '\0';

    return (srclen);
}
#endif

int main(int argc, char**argv)
{
    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    meeting_request_buf rbuf;

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

    // We'll send message type 2
    rbuf.mtype = 2;
    rbuf.request_id=atoi(argv[1]);
    strncpy(rbuf.empId,argv[2],EMP_ID_MAX_LENGTH);
    strncpy(rbuf.description_string,argv[3],DESCRIPTION_MAX_LENGTH);
    strncpy(rbuf.location_string,argv[4],LOCATION_MAX_LENGTH);
    strncpy(rbuf.datetime,argv[5],DATETIME_LENGTH);
    rbuf.duration=atoi(argv[6]);
    // strncpy(rbuf.empId,"1234",EMP_ID_MAX_LENGTH);
    // strncpy(rbuf.description_string,"Test description",DESCRIPTION_MAX_LENGTH);
    // strncpy(rbuf.location_string,"zoom link",LOCATION_MAX_LENGTH);
    // strncpy(rbuf.datetime,"2022-12-19T15:30",16);
    // rbuf.duration=60;

    // Send a message.
    if((msgsnd(msqid, &rbuf, SEND_BUFFER_LENGTH, IPC_NOWAIT)) < 0) {
        int errnum = errno;
        fprintf(stderr,"%d, %ld, %d, %ld\n", msqid, rbuf.mtype, rbuf.request_id, SEND_BUFFER_LENGTH);
        perror("(msgsnd)");
        fprintf(stderr, "Error sending msg: %s\n", strerror( errnum ));
        exit(1);
    }

    else
    fprintf(stderr,"msgsnd--mtg_req: reqid %d empid %s descr %s loc %s date %s duration %d \n",
    rbuf.request_id,rbuf.empId,rbuf.description_string,rbuf.location_string,rbuf.datetime,rbuf.duration);


    exit(0);
}

