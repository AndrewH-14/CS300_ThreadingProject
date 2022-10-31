package edu.cs300;

import java.time.LocalDateTime;
import java.util.Comparator;

public class Meeting implements Comparable<Meeting> {
    String        description;
    String        location;
    LocalDateTime time;
    Integer       duration;

    public Meeting(String        description, 
                   String        location, 
                   LocalDateTime time, 
                   Integer       duration) {

        this.description = description;
        this.location    = location;
        this.time        = time;
        this.duration    = duration;
    }

    @Override
    public int compareTo(Meeting meetingToCompare)
    {
        if (this.time.isBefore(meetingToCompare.time))
        {
            return -1;
        } else if (this.time.isAfter(meetingToCompare.time)) {
            return 1;
        } else {
            return 0;
        }
    }
}