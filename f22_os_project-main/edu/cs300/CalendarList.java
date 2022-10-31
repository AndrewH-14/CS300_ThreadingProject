package edu.cs300;

import java.util.LinkedList;
import java.util.ListIterator;
import java.time.LocalDateTime;
import java.time.temporal.ChronoUnit;

import edu.cs300.MeetingRequest;

public class CalendarList {
    
    LinkedList<Meeting> calendar;

    public CalendarList() {
        calendar = new LinkedList<Meeting>();
    }

    public boolean AddMeeting(String        meetingDescription, 
                              String        meetingLocation, 
                              LocalDateTime meetingStartTime, 
                              int           meetingDuration) {

        LocalDateTime meetingEndTime = meetingStartTime.plus(meetingDuration, 
                                                             ChronoUnit.MINUTES);

        return true;
    }
}
