package edu.cs300;

import java.util.Set;
import java.util.TreeSet;
import java.util.Comparator;
import java.util.ListIterator;
import java.time.LocalDateTime;
import java.time.temporal.ChronoUnit;
import java.util.*;

import edu.cs300.MeetingRequest;

public class CalendarList {
    
    Set<Meeting> calendar;

    public CalendarList() {
        calendar = new TreeSet<Meeting>();
    }

    public boolean AddMeeting(String        meetingDescription, 
                              String        meetingLocation, 
                              LocalDateTime meetingStartTime, 
                              int           meetingDuration) {

        Meeting meetingToAdd = new Meeting(meetingDescription, 
                                           meetingLocation, 
                                           meetingStartTime, 
                                           meetingDuration);

        if (calendar.contains(meetingToAdd))
        {
            return false;
        }

        Meeting lowerMeeting  = ((TreeSet<Meeting>)calendar).lower(meetingToAdd);
        Meeting higherMeeting = ((TreeSet<Meeting>)calendar).higher(meetingToAdd);

        if (lowerMeeting == null && higherMeeting == null) { // calendar is empty

            calendar.add(meetingToAdd);
            return true;

        } else if (lowerMeeting == null) { // only higherMeeting exists

            LocalDateTime meetingToAddEndTime = meetingToAdd.time.plus(meetingToAdd.duration, ChronoUnit.MINUTES);

            if (meetingToAddEndTime.isBefore(higherMeeting.time) || meetingToAddEndTime.isEqual(higherMeeting.time)) {
                calendar.add(meetingToAdd);
                return true;
            }
            return false;

        } else if (higherMeeting == null) { // only lowerMeeting exists

            LocalDateTime lowerMeetingEndTime = lowerMeeting.time.plus(lowerMeeting.duration, ChronoUnit.MINUTES);

            if (meetingToAdd.time.isAfter(lowerMeetingEndTime) || meetingToAdd.time.isEqual(lowerMeetingEndTime)) {
                calendar.add(meetingToAdd);
                return true;
            }
            return false;

        } else { // both lowerMeeting and higherMeeting exist
            LocalDateTime lowerMeetingEndTime    = lowerMeeting.time.plus(lowerMeeting.duration, ChronoUnit.MINUTES);
            LocalDateTime higherMeetingStartTime = higherMeeting.time;
            LocalDateTime meetingToAddStartTime  = meetingToAdd.time;
            LocalDateTime meetingToAddEndTime    = meetingToAdd.time.plus(meetingToAdd.duration, ChronoUnit.MINUTES);

            if ((meetingToAddStartTime.isAfter(lowerMeetingEndTime) || meetingToAddStartTime.isEqual(lowerMeetingEndTime)) &&
                (meetingToAddEndTime.isBefore(higherMeetingStartTime) || meetingToAddEndTime.isEqual(higherMeetingStartTime))) {

                calendar.add(meetingToAdd);
                return true;
            }
            return false;
        }
    }

    public Meeting[] GetCalendarArray() {
        return calendar.toArray(new Meeting[0]);
    }

    public int NumberOfMeetings() {
        return calendar.size();
    }
}
