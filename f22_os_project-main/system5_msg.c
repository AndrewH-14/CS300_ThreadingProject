#include "edu_cs300_MessageJNI.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <jni.h>
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


/*
 * Class:     edu_cs300_MessageJNI
 * Method:    writeReportRequest
 * Signature: (IILjava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_cs300_MessageJNI_writeMtgReqResponse
  (JNIEnv *env, jclass obj, jint id, jint avail) {

    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    meeting_response_buf sbuf;

    key = ftok(FILE_IN_HOME_DIR ,QUEUE_NUMBER);
    if (key == 0xffffffff) {
        fprintf(stderr,"Key cannot be 0xffffffff..fix queue_ids.h to link to existing file\n");
        fprintf(stderr,"Ctl-C and fix the problem\n");
        return;
    }

    if ((msqid = msgget(key, msgflg)) < 0) {
        int errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("(msgget)");
        fprintf(stderr, "Error sending msg: %s\n", strerror( errnum ));
    }
#ifdef DEBUG
    else
        fprintf(stderr, "msgget: msgget succeeded: msgqid = %d\n", msqid);
#endif

    int buffer_length=sizeof(meeting_response_buf)-sizeof(long); //int

    // // We'll send message type 1
    sbuf.mtype = 1;
    sbuf.request_id=id; //index of response
    sbuf.avail=avail; //total excerpts available

    // Send a message.
    if((msgsnd(msqid, &sbuf, buffer_length, IPC_NOWAIT)) < 0) {
        int errnum = errno;
        perror("(msgsnd)");
        fprintf(stderr, " resulted in error sending msg: %s\n", strerror( errnum ));
        exit(1);
    }
#ifdef DEBUG
    else
        fprintf(stderr," successfully sent\n");
#endif
    fprintf(stderr,"JNI msgsnd-mtgReqResponse: request id %d  avail %d: \n",sbuf.request_id,sbuf.avail);

}

/*
 * Class:     edu_cs300_MessageJNI
 * Method:    readMeetingRequest
 * Signature: ()Ledu/cs300/MeetingRequest;
 */

JNIEXPORT jobject JNICALL Java_edu_cs300_MessageJNI_readMeetingRequest
  (JNIEnv *env, jclass obj){  
    

    key_t key;
    int msqid;
    int msgflg = IPC_CREAT | 0666;
    meeting_request_buf mtg_req;


    key = ftok(FILE_IN_HOME_DIR ,QUEUE_NUMBER);
    if (key == 0xffffffff) {
        fprintf(stderr,"Key cannot be 0xffffffff..fix queue_ids.h to link to existing file\n");
        fprintf(stderr,"Ctl-C and fix the problem\n");
        return 0;
    }

    // msgget creates a message queue
    // and returns identifier
    if ((msqid = msgget(key, msgflg)) < 0) {
        int errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("(msgget)");
        fprintf(stderr, "Error in mssget: %s\n", strerror( errnum ));
    }
#ifdef DEBUG
    else
        fprintf(stderr, "msgget: msgget succeeded: msgqid = %d\n", msqid);
#endif

#ifdef DEBUG
    printf("size %ld\n",SEND_BUFFER_LENGTH);
#endif

    // msgrcv to receive message type 2
    int ret = msgrcv(msqid, &mtg_req, SEND_BUFFER_LENGTH , 2, 0);//TODO what is the correct length here
    if (ret < 0) {
        int errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error (msgrcv)");
        fprintf(stderr, "Error receiving msg: %s\n", strerror( errnum ));
        
    }

    fprintf(stderr,"JNI msgrcv-mtg_req: msgid %ld reqid %d empid %s descr %s loc %s date %s duration %d ret bytes rcv'd %d\n",
    mtg_req.mtype,mtg_req.request_id,mtg_req.empId,mtg_req.description_string,mtg_req.location_string,mtg_req.datetime,mtg_req.duration,ret);

    // Create the object of the class UserData
    jclass MeetingRequestClass = (*env)->FindClass(env,"edu/cs300/MeetingRequest");
    jstring description = (*env)->NewStringUTF(env,mtg_req.description_string);
    jstring location = (*env)->NewStringUTF(env,mtg_req.location_string);
    jstring datetime = (*env)->NewStringUTF(env,mtg_req.datetime);
    jstring empid = (*env)->NewStringUTF(env,mtg_req.empId);

    jmethodID constructor = (*env)->GetMethodID(env, MeetingRequestClass, "<init>", "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
    jobject meetingRequest = (*env)->NewObject(env, MeetingRequestClass, constructor,mtg_req.request_id, empid,description,location,datetime,mtg_req.duration);


    // // Try running the toString() on this newly create object
#ifdef DEBUG
    jmethodID mrToString = (*env)->GetMethodID(env, MeetingRequestClass, "toString", "()Ljava/lang/String;");
    if (NULL == mrToString) return NULL;
    jstring resultStr = (*env)->CallObjectMethod(env, meetingRequest, mrToString);
    const char *resultCStr = (*env)->GetStringUTFChars(env, resultStr, NULL);
    printf("In C: the value is %s\n", resultCStr);
#endif

    return meetingRequest;
  }
  

JNIEXPORT jstring JNICALL Java_edu_cs300_MessageJNI_readStringMsg
(JNIEnv *env, jobject obj) {

    key_t key;
    int msqid;
    char rbuf[100];
    int msgflg = IPC_CREAT | 0666;

    // ftok to generate unique key
    key = ftok(FILE_IN_HOME_DIR, QUEUE_NUMBER);

    // msgget creates a message queue
    // and returns identifier
    if ((msqid = msgget(key, msgflg)) < 0) {
        int errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("(msgget)");
        fprintf(stderr, "Error msgget: %s\n", strerror( errnum ));
    }
    else
        fprintf(stderr, "msgget: msgget succeeded: msgqid = %d\n", msqid);

    // msgrcv to receive message
    int ret = msgrcv(msqid, &rbuf, 100, 1, 0);//TODO what is the correct length here
    if (ret < 0) {
        int errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error printed by perror");
        fprintf(stderr, "Error receiving msg: %s\n", strerror( errnum ));
        strcpy(rbuf,"error");//return error to java program
    }

    jstring result;

    puts(rbuf);
    result = (*env)->NewStringUTF(env,rbuf);
    return result;
}


