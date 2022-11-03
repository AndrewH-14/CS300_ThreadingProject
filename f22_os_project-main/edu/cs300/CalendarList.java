package edu.cs300;

import java.util.Set;
import java.util.TreeSet;
import java.util.Comparator;
import java.util.ListIterator;
import java.time.LocalDateTime;
import java.time.temporal.ChronoUnit;

import edu.cs300.MeetingRequest;

/**
 * The CalendarList class implements a data structure that will be used to
 * store the employee's current calendar.
 */
public class CalendarList {
    
    Set<Meeting> calendar;

    /**
     * Constructor for the CalendarList class.
     */
    public CalendarList() {
        // Use a balanced binary tree set to provide quick node addition and lookup.
        calendar = new TreeSet<Meeting>();
    }

    /**
     * Function to add a meeting to the calendar.
     * 
     * @param meetingDescription The description of the meeting.
     * @param meetingLocation    The location of the meeting.
     * @param meetingStartTime   The starting time of the meeting.
     * @param meetingDuration    The duration of the meeting.
     * 
     * @retval true  The meeting was added.
     * @retval false The meeting was not added.
     */
    public boolean AddMeeting(String        meetingDescription, 
                              String        meetingLocation, 
                              LocalDateTime meetingStartTime, 
                              int           meetingDuration) {

        Meeting meetingToAdd = new Meeting(meetingDescription, 
                                           meetingLocation, 
                                           meetingStartTime, 
                                           meetingDuration);

        // First check if a meeting with this start time is already in the tree,
        // if it is then the meeting automatically can't be added.
        // This was necessary to prevent issues regarding the availability return value.
        if (calendar.contains(meetingToAdd)) {
            return false;
        }

        // Get the meeting right before and right after the meeting we are attempting to add
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

    /**
     * A function to get a sorted array version of the calendar.
     * 
     * @return The calendar as a sorted array of meetings.
     */
    public Meeting[] GetCalendarArray() {
        return calendar.toArray(new Meeting[0]);
    }

    /**
     * A function to get the number of meetings currently in the calendar
     * 
     * @return The number of meetings in the calendar.
     */
    public int NumberOfMeetings() {
        return calendar.size();
    }
}
