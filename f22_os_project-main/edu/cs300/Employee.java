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

public class Employee extends Thread {
    
    String employee_id;
    String calendar_filename;
    String employee_name;
    ArrayBlockingQueue<MeetingRequest> incomingRequests;
    ArrayBlockingQueue<MeetingResponse> outgoingResponse;
    CalendarList calendar;

    /**
     * Constructor for the Employee class. Stores employee information, and 
     * initializes the data structure that will hold the employee's calendar.
     * 
     * @param employee_id       The employees id in string form.
     * @param calendar_filename The name of the file that contains the employee 
     *                          calendar information.
     * @param employee_name     The name of the employee.
     */
    public Employee(String employee_id, 
                    String calendar_filename, 
                    String employee_name,
                    ArrayBlockingQueue<MeetingRequest> incomingRequests,
                    ArrayBlockingQueue<MeetingResponse> outgoingResponse) {

        this.employee_id        = employee_id;
        this.calendar_filename  = calendar_filename;
        this.employee_name      = employee_id;
        this.calendar           = new CalendarList();
        this.incomingRequests   = incomingRequests;
        this.outgoingResponse   = outgoingResponse;
    }

    /*
     * Function that creates a thread for the employee that must
     * 
     *  1. Read the existing calendar from the employees file.
     *  2. Reads a queue to receive meeting requests from the incoming message
     *     request thread.
     *  3. Process the meeting request.
     *  4. Push the request to resultsOutputArray 
     */
    public void run() {

        try {

            // Read employees calendar information from <employee id>.dat
            File calendarFile = new File(employee_id + ".dat");
            Scanner fileScanner = new Scanner(calendarFile);

            while (fileScanner.hasNextLine()) {

                String   meetingInformationString = fileScanner.nextLine();
                String[] meetingInformationArray  = meetingInformationString.split(",");

                this.calendar.AddMeeting(meetingInformationArray[0],
                                         meetingInformationArray[1],
                                         LocalDateTime.parse(meetingInformationArray[2]),
                                         Integer.valueOf(meetingInformationArray[3]));
            }
        } catch (FileNotFoundException e) {
            System.out.println("An error occured when openining " + this.calendar_filename);
        }

        while (true) {
            try {
                MeetingRequest mtgReq = (MeetingRequest)this.incomingRequests.take();

                if (mtgReq.request_id > 0) {

                    boolean added = calendar.AddMeeting(mtgReq.description, 
                                                        mtgReq.location, 
                                                        LocalDateTime.parse(mtgReq.datetime), 
                                                        mtgReq.duration);

                    if (added) {
                        // Send response stating that the meeting was accepted
                        this.outgoingResponse.put(new MeetingResponse(mtgReq.request_id, 1));
                    }
                    else {
                        // Send response stating that the meeting was rejected
                        this.outgoingResponse.put(new MeetingResponse(mtgReq.request_id, 0));
                    }
                } else {

                    // Back up the calendar and exit the loop
                    Meeting[] meetingArray = calendar.GetCalendarArray();

                    try {
                        // https://stackoverflow.com/a/52581496
                        FileWriter fileWriter = new FileWriter(this.calendar_filename + ".bak");

                        for (Meeting meeting : meetingArray) {
                            fileWriter.write(meeting.description + "," + 
                                             meeting.location    + "," + 
                                             meeting.time        + "," + 
                                             meeting.duration    + "\n");
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
