package edu.cs300;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.Writer;
import java.io.FileWriter;
import java.time.LocalDateTime;
import java.util.Scanner;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * The Employee class should be used to create a worker thread for each employee
 * that has a calendar. After being initialized and started, the thread will
 * run until it receives a request_id of 0, which is passed in from the 
 * input queue.
 * 
 * @note Worker.java was used as a template for this class.
 */
public class Employee extends Thread {
    
    String employeeID;
    String calendarFilename;
    String employeeName;
    ArrayBlockingQueue<MeetingRequest> incomingRequests;
    ArrayBlockingQueue<MeetingResponse> outgoingResponse;
    CalendarList calendar;

    /**
     * The constructor for the Employee class.
     * 
     * @param employeeID       The ID of an employee who has a calendar which
     *                         will potentially need to be updated.
     * @param calendarFilename The name of the file that has the calendar's 
     *                         starting information. The information from this
     *                         file will be read in and added to the calendar
     *                         data structure upon thread initialization.
     * @param employeeName     The name of the employee.
     * @param incomingRequests A queue that will be used to recieve messages
     *                         via a blocking wait. Once a message is received,
     *                         the thread will attempt to add it to the calendar.
     *                         This queue should only be accessed by one employee
     *                         thread at a time.
     * @param outgoingResponse A queue that wil be used to send response messages
     *                         regarding the addition of the meeting. This queue is
     *                         shared between all employee threads.
     */
    public Employee(String employeeID, 
                    String calendarFilename, 
                    String employeeName,
                    ArrayBlockingQueue<MeetingRequest> incomingRequests,
                    ArrayBlockingQueue<MeetingResponse> outgoingResponse) {

        this.employeeID       = employeeID;
        this.calendarFilename = calendarFilename;
        this.employeeName     = employeeName;
        this.incomingRequests = incomingRequests;
        this.outgoingResponse = outgoingResponse;
        this.calendar         = new CalendarList();
    }

    /**
     * Worker thread function for the Employee class. An Employee thread should
     * be created for each employee in the employee.csv file. Once running, the
     * thread will loop on the input queue awaiting meetin requests to be 
     * added to the calendar. Once received, a response will be generated and
     * added to the output queue. This thread will exit once it receives a signal
     * via the input queue. To ensure data integrity, the current calendar will
     * be backed up to a <employeeID>.dat.bak file which will list the meetings
     * in chronological order.
     * 
     * Expected format of <employeeID>.dat and <employeeID>.dat.bak
     * 
     * "<description>","<location>",<localdatetime>,<duration>
     */
    public void run() {
        // Read each line of the employee's data file and add it to the calendar.
        try {
            File calendarFile   = new File(employeeID + ".dat");
            Scanner fileScanner = new Scanner(calendarFile);

            // Each line of the data file should correspond to a meeting
            while (fileScanner.hasNextLine()) {
                String   meetingInformationString = fileScanner.nextLine();
                String[] meetingInformationArray  = meetingInformationString.split(",");

                // Method for removing leading a trailing quotation marks gotten from:
                // https://www.baeldung.com/java-remove-start-end-double-quote
                if (meetingInformationArray[0] != null && meetingInformationArray[0].length() >= 2 
                    && meetingInformationArray[0].charAt(0) == '\"' && meetingInformationArray[0].charAt(meetingInformationArray[0].length() - 1) == '\"') {
                    meetingInformationArray[0] = meetingInformationArray[0].substring(1, meetingInformationArray[0].length() - 1);
                }

                if (meetingInformationArray[1] != null && meetingInformationArray[1].length() >= 2 
                    && meetingInformationArray[1].charAt(0) == '\"' && meetingInformationArray[1].charAt(meetingInformationArray[1].length() - 1) == '\"') {
                    meetingInformationArray[1] = meetingInformationArray[1].substring(1, meetingInformationArray[1].length() - 1);
                }

                this.calendar.AddMeeting(meetingInformationArray[0],
                                         meetingInformationArray[1],
                                         LocalDateTime.parse(meetingInformationArray[2]),
                                         Integer.valueOf(meetingInformationArray[3]));
            }
        } catch (FileNotFoundException e) {
            System.out.println("An error occured when openining " + this.calendarFilename);
        }

        // Blocking on the incoming requests queue will prevent the infinite loop
        // from wasting CPU cycles
        while (true) {
            try {
                MeetingRequest mtgReq = (MeetingRequest)this.incomingRequests.take();

                // A request_id serves as a signal that all data has been received,
                // and that it is now safe to backup the data and exit.
                if (mtgReq.request_id > 0) {
                    boolean added = calendar.AddMeeting(mtgReq.description, 
                                                        mtgReq.location, 
                                                        LocalDateTime.parse(mtgReq.datetime), 
                                                        mtgReq.duration);

                    // Send a reponse message signaling whether the request was accepted or not
                    // ArrayBlockingQueues are thread safe, so no need to worry about concurrently
                    // puting requests on it.
                    // https://stackoverflow.com/questions/26543807/is-blockingqueue-completely-thread-safe-in-java
                    if (added) {
                        this.outgoingResponse.put(new MeetingResponse(mtgReq.request_id, 1));
                    }
                    else {
                        this.outgoingResponse.put(new MeetingResponse(mtgReq.request_id, 0));
                    }
                } else {
                    // Gets a sorted array of the employee's calendar
                    Meeting[] meetingArray = calendar.GetCalendarArray();

                    try {
                        FileWriter fileWriter = new FileWriter(this.calendarFilename + ".bak");

                        for (Meeting meeting : meetingArray) {
                            fileWriter.write("\"" + meeting.description + "\"" + "," +
                                             "\"" + meeting.location    + "\"" + "," + 
                                                    meeting.time               + "," + 
                                                    meeting.duration           + "\n");
                        }

                        fileWriter.close();
                        break;
                    } catch (IOException e) {
                        System.out.println("A file writing error occured");
                    }
                }
            } catch(InterruptedException e) {
                System.err.println(e.getMessage());
            }
        }
    }
}
