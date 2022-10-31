package edu.cs300;

import java.time.LocalDateTime;

public class Meeting {
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
}
