package edu.cs300;

import java.time.LocalDateTime;

public class MeetingRequest {

	public MeetingRequest(int request_id, String empId, String description, String location, String datetime,
			int duration) {
		this.request_id = request_id;
		this.empId = empId;
		this.description = description;
		this.location = location;
		this.datetime = datetime;
		this.duration = duration;
	}
	  
	int request_id;
	String empId;
	String description;
	String location;
	String datetime;//2022-12-20T08:30
	int duration;
	
	@Override
	public String toString() {
		return "MeetingRequest [request_id=" + request_id + ", empId=" + empId + ", description="
				+ description + ", location=" + location + ", datetime=" + datetime + ", duration=" + duration + "]";
	}
	
	public boolean isEndMsg() {
		return (request_id==0);
	}

}
