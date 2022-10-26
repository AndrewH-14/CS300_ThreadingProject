# Fall22\_cs300\_project

You will complete a multithreaded two process system that communicates via System V message queues. The system goal is that meeting requests received by the **request\_mtgs** program are sent to the java program **CalendarManager** via the System 5 queue. The **CalendarManager** loads each employee's calendars at the start of the program, updates calendars with meetings that can be accommodated, sends back conflict/added responses to **request\_mtgs** program and backs up each employee's calendar when the program exits.

Git Repository for project files

## All commands and code have been tested on cs-operatingsystems01.ua.edu

`make` should make all files.  mvn package will build the java files

- Set up JAVA_HOME environment variable

`export JAVA_HOME=/usr/java/latest`

- Compile java files in edu/cs300 packages

`mvn package`

- Create header file for System V C message functions

`javac -cp src/ -h . src/edu/cs300/MessageJNI.java`

- Compile C native functions into a library for use with MessageJNI

`gcc -c -fPIC -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux system5_msg.c -o edu_cs300_MessageJNI.o`

`gcc -shared -o libsystem5msg.so edu_cs300_MessageJNI.o -lc`


- Compile test send and receive functions

`gcc -std=c99 -D_GNU_SOURCE msgsnd_meeting_request.c -o msgsnd`

`gcc -std=c99 -D_GNU_SOURCE msgrcv_meeting_response.c -o msgrcv`


## Commands to run sample programs

- Create a test atbat message and puts it on the queue

`./msgsnd 1 "6789" "command line mtg" "online" "2022-12-17T14:30" 60`

```
msgget: msgget succeeded: msgqid = 524288
msgsnd--mtg_req: reqid 1 empid 6789 descr command line mtg loc online date 2022-12-17T14:30 duration 60
```

- Java program reads queue contents using native C function and creates and sends a response message via the System V msg queue

`java -cp ./target/classes  -Djava.library.path=. edu/cs300/MessageJNI`

```
Running MessageJNI test routine
msgget: msgget succeeded: msgqid = 524288
 successfully sent
JNI msgsnd-mtgReqResponse: request id 1  avail 0:
msgget: msgget succeeded: msgqid = 524288
size 104
JNI msgrcv-mtg_req: msgid 2 reqid 0 empid 6789 descr command line mtg loc online date 2022-12-17T14:30 duration 60 ret bytes rcv'd 104
MeetingRequest [request_id=0, empId=6789, description=command line mtg, location=online, datetime=2022-12-17T14:30, duration=60]
```

- Retrieves message from System V queue and prints it

`./msgrcv`

```
msgget: msgget succeeded: msgqid = 524288
msgrcv-mtgReqResponse: request id 1  avail 0:
```

- Program to illustrate use of Java threading and BlockingArrayQueue 
Run in two windows

Window 1 - send and receive messages from C program

`./msgsnd 0 "1234" "command line mtg" "online" "2022-12-17T14:30" 60`
```
msgget: msgget succeeded: msgqid = 524288
msgsnd--mtg_req: reqid 0 empid 1234 descr command line mtg loc online date 2022-12-17T14:30 duration 60
```

`./msgrcv`
```
msgget: msgget succeeded: msgqid = 524288
msgrcv-mtgReqResponse: request id 0  avail 1:
```

Window 2 Use threaded java program

`java -cp ./target/classes  -Djava.library.path=. edu/cs300/CalendarManager`

```
msgget: msgget succeeded: msgqid = 524288
size 104
Worker.run():19  Thread (4567) thread started ...
CalendarManager$OutputQueueProcessor.run():46 Thread-2 processing responses
Worker.run():19  Thread (1234) thread started ...
JNI msgrcv-mtg_req: msgid 2 reqid 0 empid 1234 descr command line mtg loc online date 2022-12-17T14:30 duration 60 ret bytes rcv'd 104
CalendarManager$InputQueueProcessor.run():75 Thread-3recvd msg from queue for 1234
CalendarManager$InputQueueProcessor.run():78 Thread-3 pushing req MeetingRequest [request_id=0, empId=1234, description=command line mtg, location=online, datetime=2022-12-17T14:30, duration=60] to 1234
msgget: msgget succeeded: msgqid = 524288
size 104
Worker.run():22 Worker-1234 MeetingRequest [request_id=0, empId=1234, description=command line mtg, location=online, datetime=2022-12-17T14:30, duration=60] pushing response 0
msgget: msgget succeeded: msgqid = 524288
 successfully sent
JNI msgsnd-mtgReqResponse: request id 0  avail 1:
```

## Verify the message queue setup

```
> cat ./queue_ids.h 
#define FILE_IN_HOME_DIR "/home/anderson/anderson"
#define QUEUE_NUMBER 20 //day of birth
> ls -lt  /home/anderson/anderson
-rw-r--r-- 1 anderson users 9 Feb 26  2020 /home/anderson/anderson
```


## Commands to check/delete message queues

- Determine the status of your message queue via `ipcs -a`

```
------ Message Queues --------
key        msqid      owner       perms      used-bytes   messages    
0x0d03bc96 0          crimson2      666        0            0           
0xffffffff 98305      crimson3      666        0            0           
0x0303fabb 65538      crimson1      666        0            0           
**0x0c030904 131075     anderson    666        13            1**           
------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status      
------ Semaphore Arrays --------
key        semid      owner      perms      nsems     
```

*0x0C030904 is the queue created by `./msgsnd`.  It has 1 message of 13 bytes.*


- Removes the queue along with any messages `ipcrm -Q 0x0c030904`

```
------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages    
0x0d03bc96 0          crimson2    666        0            0           
0xffffffff 98305      crimson3    666        0            0           
0x0303fabb 65538      crimson1    666        0            0           

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status      
------ Semaphore Arrays --------
key        semid      owner      perms      nsems
```

>Note:  If your queue key is 0x0xffffffff, you must follow the directions to create the queue file in your home directory and update queue_ids.h

