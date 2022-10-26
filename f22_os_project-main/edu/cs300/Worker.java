package edu.cs300;

import java.util.concurrent.ArrayBlockingQueue;

class Worker extends Thread{

	  ArrayBlockingQueue<MeetingRequest> incomingRequests;
	  ArrayBlockingQueue<MeetingResponse> outgoingResponse;
	  String empId;


	  public Worker(String empId,ArrayBlockingQueue<MeetingRequest> incomingRequests, ArrayBlockingQueue<MeetingResponse> outgoingResponse){
	    this.incomingRequests=incomingRequests;
	    this.outgoingResponse=outgoingResponse;
		this.empId=empId;
	  }

	  public void run() {
	    DebugLog.log(" Thread ("+this.empId+") thread started ...");
		try {
		MeetingRequest mtgReq=(MeetingRequest)this.incomingRequests.take();
		DebugLog.log("Worker-" + this.empId + " " + mtgReq+" pushing response "+mtgReq.request_id);
		this.outgoingResponse.put(new MeetingResponse(mtgReq.request_id,1));
			
		} catch(InterruptedException e){
			System.err.println(e.getMessage());
		}
	    
	  }

	}
