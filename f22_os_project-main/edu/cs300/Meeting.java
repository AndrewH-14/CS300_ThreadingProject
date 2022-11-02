package edu.cs300;

import java.time.LocalDateTime;
import java.util.Comparator;

/**
 * The Meeting class implements an object that can be used to store and compare
 * the meetings on an employee's calendar.
 */
public class Meeting implements Comparable<Meeting> {
    String        description;
    String        location;
    LocalDateTime time;
    Integer       duration;

    /**
     * Constructor for the Meeting class. 
     * 
     * @param description A string of the meeting's description.
     * @param location    A string specifying the meeting's location.
     * @param time        The starting time of the meeting in LocalDateTime form.
     * @param duration    An integer representing the duration of the meeting.
     */
    public Meeting(String        description, 
                   String        location, 
                   LocalDateTime time, 
                   Integer       duration) {

        this.description = description;
        this.location    = location;
        this.time        = time;
        this.duration    = duration;
    }

    /**
     * The comparator to be used when comparing meeting objects. This comparisson
     * is made based on the meeting object's time field.
     * 
     * @param meetingToCompare The meeting to compare to this meeting.
     * 
     * @retval -1 The given meeting's start time is before this meeting.
     * @retval  0 The given meeting's start time is equal to this meeting.
     * @retval  1 The given meeting's start time is after this meeting.
     */
    @Override
    public int compareTo(Meeting meetingToCompare) {
        if (this.time.isBefore(meetingToCompare.time)) {
            return -1;
        } else if (this.time.isAfter(meetingToCompare.time)) {
            return 1;
        } else {
            return 0;
        }
    }
}