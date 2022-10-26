**CS300 Fall 2022**

**Calendar Manager - Programming Project**

**Due Date: Friday Nov 4 @ 12pm**

**Project files: https://github.com/monicadelaine/f22_os_project**

**Overview**

You will complete a multithreaded two process system that communicates via System V message queues. The system goal is that meeting requests received by the **request\_mtgs** program are sent to the java program **CalendarManager** via the System 5 queue. The **CalendarManager** loads each employee's calendars at the start of the program, updates calendars with meetings that can be accommodated, sends back conflict/added responses to **request\_mtgs** program and backs up each employee's calendar when the program exits.

**request\_mtgs Logic**

**request\_mtgs** reads meeting requests from stdin for employees. Each request is checked against an employee's existing schedule for conflicts by sending the request to the **CalendarManager** program via the System V queue. The **CalendarManager** will respond with a message that indicates whether the meeting was added to the calendar by indicating availability (availability of 0 means conflict, availability of 1 means added). The **request\_mtgs** will print out the result (conflict or added) of each meeting request.

**request\_mtgs requirements**

- Must be written in C. Source file containing main function is request\_mtgs.c. Any additional source files must be compiled and linked in the makefile
- Do not use msgrcv or msgsnd in your solution.  You should borrow logic for use in **request_mtgs**.  Do not use exec, system or any other process creation API.  You must use threads
- Format for incoming meeting requests includes the following fields. The header file provides length constraints.
  - Request id: integer
  - Employee id: string
  - Meeting description: quote delimited string
  - Meeting location: quote delimited string
  - Meeting date/time: date formatted as string; Assume single time zone and 24-hour time; Ex: ***2007-12-03T10:15:30***.
  - Meeting duration: integer in minutes
- Must create a thread to send each request, receive its response and print the result. Make sure to release all dynamically allocated memory.
- Must send and receive data via System V message queues using the predefined message formats
- Must respond to SIGINT with number of outstanding requests
- The last message received from stdin will have a request id of zero. This request indicates the end of the input. Once all the previous requests have been sent, processed and replies received and printed, the request\_mtgs should exit.

**Format:** `./request_mtgs`

```
anderson@cs-operatingsystems01.ua.edu ~% cat input.msg
1,1234,"morning mtg","conf room",2022-12-17T14:30,60
2,1234,"conflict mtg","zoom",2022-12-17T15:00,60
0,9999,"any","any",any,60
```

```
anderson@cs-operatingsystems01.ua.edu ~% ./request_mtgs < input.msg
Meeting request 1 for employee 1234 was accepted (morning mtg @ conf room starting 2022-12-17T14:30 for 60 minutes
Meeting request 2 for employee 1234 was rejected due to conflict (conflict mtg @ zoom starting 2022-12-17T15:00 for 60 minutes
```

**CalendarManager logic**

The **CalendarManager** be responsible for maintaining the current calendar. Each employee's initial calendar will be loaded from a file. The **CalendarManager** will check each meeting request against an employees' existing calendar. A response message will be sent back via the System V queue with the request ID and the availability (1 if added, 0 if a conflict). When the **CalendarManager** receives a request with a **request\_id of 0**, the **CalendarManager** will backup each employee's calendar file, and complete all of the threads.

The **CalendarManager** will create the following threads:

1. A Worker thread will be created for each employee that is listed in the employees.csv
  1. Reads the existing calendar from a file (denoted in the employees.csv) concurrently. Each line represents an accepted meeting,
  2. Reads a queue to receive meeting requests from the incoming meeting request thread
  3. Processes meeting requests concurrently via a queue,
  4. Pushes meeting responses to an outgoing meeting response thread (a meeting conflicts if it overlaps another meeting),
  5. Backs up the current calendar file and exits when the terminate message (`request_id==0`) is received
2. An **incoming meeting request (see InputQueueProcessor class in `CalendarManager.java`)** thread that reads the meeting requests from the System V queue using a Java Native Call Interface in MessageJNI.readMeetingRequest() and sends the requests to the appropriate worker for each employee. This thread exits when all requests have been processed (receives a message with request ID of 0)
3. An **outgoing meeting response thread see OutputQueueProcessor class in `CalendarManager.java`** that retrieves every response message (from all Workers) and sends it to the **request_mtgs** via the System V queue using MessageJNI.writeMtgReqResponse(). This thread exits once all responses have been processed (triggered by upline message received with request_id of 0)

The incoming meeting request thread and the outgoing meeting response thread are the only threads to access the System V queue, avoiding any synchronization issues with the Java Native Interface.

**Format:** `java -cp ./target/classes  -Djava.library.path=. edu/cs300/CalendarManager`

```
anderson@cs-operatingsystems01.ua.edu: java -cp ./target/classes  -Djava.library.path=. edu/cs300/CalendarManager
```

**CalendarManager** requirements

- Written in Java with the main function in `edu.cs300.CalendarManager.java`
- Read employee information from `employees.csv` in java root directory (hardcode the name-`employees.csv` with no path)
- Read contents of each employee's calendar file in the root directory (see sample files)
  - Comma delimited record format: `employee_id,calendar_filename,employee name`
  - When **CalendarManager** exits, backup the contents of each employee's calendar file to the calendar filename+".bak". Contents should be sorted in event start order. Overwrite any previous backup file.
- All employees that receive meeting requests messages will have a record in employees.csv. The calendar file (via the name in the `employees.csv`) will exist but may be empty
- Requests for one employee should process concurrently with the requests from other employees and should write the backup files concurrently
- A single thread should be used to call `MessageJNI.writeMtgReqResponse` methods to send messages back to the **request_mtgs** (see OutputQueueProcessor class in `CalendarManager.java`)
- A single thread should be used to retrieve messages using the `MessageJNI.readMeetingRequest` (see InputQueueProcessor class in `CalendarManager.java`)

**Sample data files:**

event.dat

```
1,6789,"Weekly status mtg","conference room",2022-12-17T14:30,60
2,1234,"Christmas lunch","Chuck's Fish",2022-12-24T11:30,60
```

employees.csv

```
1234,1234.dat,John Doe
4567,4567.dat,Jane Doe
6789,6789.dat,Joe Doe
```

1234.dat

```
"staff meeting","conference room",2022-12-20T15:30,60
"status meeting","conference room",2022-12-20T11:30,30
"team lunch","Chipotle",2022-12-21T11:30,90
"doctor appointment","UMC",2022-12-19T15:30,45
```

1234.dat.bak

```
"morning mtg"," conf room ",2022-12-17T14:30,60
"doctor appointment","UMC",2022-12-19T15:30,45
"status meeting","conference room",2022-12-20T11:30,30
"staff meeting","conference room",2022-12-20T15:30,60
"team lunch","Chipotle",2022-12-21T11:30,90
```

Input piped to stdin to request_mtgs

```
1,1234,"morning mtg","conf room",2022-12-17T14:30,60
1,1234,"conflict mtg","zoom",2022-12-17T15:00,60
0,9999,"any","any",any,60
```

**Additional project code specifications**

- Any debug messages should print to stderr or be controlled by a DEBUG flag that is turned off by default
- You must use the header file and the predefined messages
- You must place and implement functionality as described in the description. Programs that do not follow the guidelines will receive a zero.
- The System V message queue requires an existing file and integer to create a unique queue name. You should create a file using your crimson id in your home directory. Use queue\_ids.h header file to create a constant string that holds the path to the queue and a constant integer to hold the day of your birthday. Use FILE\_IN\_HOME\_DIR and QUEUE\_NUMBER in the ftok command to generate the identifier

(see https://github.com/monicadelaine/f22_os_project/blob/master/msgsnd_mtg_request.c for an example)

```
#define FILE_IN_HOME_DIR"/home/anderson/anderson"
#define QUEUE_NUMBER 12 //day of birth
```

- Place files in the directory structure below (matches sample github). Use `make archive` to create a user specific file to turn in. Turn the created file **USERNAME-f22os.tar.gz**.  The project will be tested via a script. Not using the make archive breaks the script and will cause your project test to fail.

![](/images/project_folder.png)


**meeting\_request\_formats.h notes**

```
#define DESCRIPTION_MAX_LENGTH    30
#define LOCATION_MAX_LENGTH       30
#define EMP_ID_MAX_LENGTH         10
#define DESCRIPTION_FIELD_LENGTH  DESCRIPTION_MAX_LENGTH+1
#define LOCATION_FIELD_LENGTH     LOCATION_MAX_LENGTH+1
#define EMP_ID_FIELD_LENGTH       EMP_ID_MAX_LENGTH+1
```

**Sending meeting request from ./msgsnd to JNI**

- Message struct for sending in `msgsnd_mtg_request.c:47` and received in `system5msg.c:115` called from called from `java_edu_cs300_MessageJNI.readMeetingRequest`
- Type should be set ***2*** for sending `msgsnd_mtg_request.c:66`
- Receive message of type ***2*** using `msgrcv(msqid, &rbuf, SEND_BUFFER_LENGTH
- , ***2*** , 0)` in `system5msg.c:143`
- Record length determined by `message_request_buf` size (see calculation of `SEND_BUFFER_LENGTH`)

![](/images/mtg_req_buf.png)

**Sending meeting response from JNI to ./msgrcv**

- Message struct for sending in `system5_msg.c:59` called from java `edu/cs300/MessageJNI.writeMeetingResponse()` and receiving in `msgrcv_mtg_response.c:16`
- Type should be set ***1*** for sending at `system5_msg.c:82`
- Receive message of type ***1*** using `msgrcv(msqid, &rbuf, length, ***1***, 0)` in `msgrcv_mtg_response.c:37`
- Record length: 2 \* sizeof(int)

![](/images/mtg_rep_buf.png)

**JNI Functions**

- Defined in `system5_msg.c`
- Accessed via Java calls in `MessageJNI.java`
- Examples for using `edu/cs300/MessageJNI.java:22` and `edu/cs300/MessageJNI.java:23`
- System generated header file for `system5_msg` is autogenerated from `MessageJNI.java`

**Other criteria**

- Minimize resource usage (do not hardcode any values other than given above)
- Do not assume any ordering of the message retrieval
- Maximize parallel processing
- Appropriately protect data structures as needed
- Minimize use of global variables (don't use as a mechanism to avoid passing parameters)
- Free any allocated memory; join any threads
- Do not remove IPC queue when done
- Message queue key FILE\_IN\_HOME\_DIR should be a file in your home directory
- Programs should be coded in C language (C99 standard) and will be compiled and tested on cs-operatingsystems01.ua.edu. If you choose to program on another system, give yourself enough time verify it works on cs-operatingsystems01.ua.edu. No other system will be used to test your code. May need \_GNU\_SOURCE switch.
- You should use the pthreads library for threading. You can use mutexes or condition variables from the pthreads library and/or semaphores from the posix library.
- Appropriate data structures should be selected based on your knowledge of data structures (CS201, etc).
- Algorithms should be efficient and appropriate. This program should demonstrate not only your understanding of process synchronization but your ability to design a program appropriately
- No sleeps
- Use #ifdef DEBUG to remove/add debug print statements based on compilation (-DDEBUG=0 or -DDEBUG=1)
- Use standard error to print error messages
- Use assert to check for unexpected conditions

**Grading policy**

_Failure to follow directions will result in point deductions. There are 75 students in this class. It is unreasonable to expect that any exceptions to the procedure will be made._

**Late assignments will not be accepted** unless you have a doctor's note covering the entire period from Oct 21-Nov 4. The source code and test results should be printed and brought to class on Nov 4th. Make sure your printout is easy to read (line wrapping etc). The source code should also be turned in via Blackboard (not emailed to me or the TA). Test results (using your generated data) should also be printed and submitted via blackboard in pdf format. Test result submissions of any other type will not be graded.

This is an individual assignment.  The program must represent your own work. You can discuss high-level concepts. Do not show your code to anyone. I reserve the right to ask you about your program to discern if you indeed wrote the code. If you cannot explain your code and choices verbally, you may be turned in for academic misconduct. All submissions will be analyzed to identify possible cases of cheating.  Any cases of suspected collaboration will be referred to the College of Engineering Dean.  A zero or low grade is always better than having an academic misconduct on your academic record.



_Programs will be evaluated based on many functional and design criteria_

**Sample criteria include:**

70% - functionality

- Program contains the correct code to read/send/manage/backup calendars and meeting requests
- Code for CalendarManager contains correct functionality
- Code for request\_mtgs contains correct functionality
- Hardcoding and lengths as specified
- Signal catch implemented and working
- Process sync correct (threads in CalendarManager tracker and signals in request\_mtgs)
- Maximizes concurrency
- Other functional or correctness features

25% - design

- Program exhibits defensible design choices in algorithms and data structures (if you add any)
- Program does not contain extra loops or any code that hurts efficiency
- Other design and efficiency features

5% - style

- Program must use appropriate and consistent style for naming of elements
- Program must include reasonable whitespace and appropriate indentation
- Program must include comments, especially in areas where you need to support your choices or where the purpose of the code is unclear.

**Clarifications on the assignment will be posted to blackboard or github as appropriate**
