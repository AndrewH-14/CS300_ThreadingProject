package edu.cs300;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Hashtable;
import java.util.Scanner;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.atomic.AtomicInteger;

public class CalendarManager {
	
	Hashtable<String,ArrayBlockingQueue<MeetingRequest>> empQueueMap;
	ArrayBlockingQueue<MeetingResponse> resultsOutputArray;
	
	public CalendarManager() {
		this.resultsOutputArray = new ArrayBlockingQueue<MeetingResponse>(30);
		empQueueMap = new Hashtable<String,ArrayBlockingQueue<MeetingRequest>>();
		
		ArrayBlockingQueue<MeetingRequest> queue1234 = new  ArrayBlockingQueue<MeetingRequest>(10);
		ArrayBlockingQueue<MeetingRequest> queue4567 = new  ArrayBlockingQueue<MeetingRequest>(10);
		empQueueMap.put("1234", queue1234);
		empQueueMap.put("4567", queue4567);
		new Worker("1234",queue1234, this.resultsOutputArray).start();      
		new Worker("4567",queue4567, this.resultsOutputArray).start();      

		new OutputQueueProcessor(this.resultsOutputArray).start();
		new InputQueueProcessor(this.empQueueMap).start();
			   
	}

	public static void main(String args[]) {
		
		CalendarManager mgr = new CalendarManager();


	}
	
	class OutputQueueProcessor extends Thread {
		
		ArrayBlockingQueue<MeetingResponse> resultsOutputArray;
		
		OutputQueueProcessor(ArrayBlockingQueue<MeetingResponse> resultsOutputArray){
			this.resultsOutputArray=resultsOutputArray;
		}
		
		public void run() {
			DebugLog.log(getName()+" processing responses ");
			while (true) {
				try {
					MeetingResponse res = resultsOutputArray.take();
	
						MessageJNI.writeMtgReqResponse(res.request_id, res.avail);
						DebugLog.log(getName()+" writing response "+res);
					
				} catch (Exception e) {
					System.out.println("Sys5OutputQueueProcessor error "+e.getMessage());
				}

			}
			
		}
		
	}

	class InputQueueProcessor extends Thread {
		Hashtable<String,ArrayBlockingQueue<MeetingRequest>> empQueueMap;
		
		InputQueueProcessor(Hashtable<String,ArrayBlockingQueue<MeetingRequest>> empQueueMap){
			this.empQueueMap=empQueueMap;
		}

		public void run(){
			while (true) {
				MeetingRequest req = MessageJNI.readMeetingRequest();
				try {
					DebugLog.log(getName()+"recvd msg from queue for "+req.empId);
					if (empQueueMap.containsKey(req.empId)) {
						empQueueMap.get(req.empId).put(req);
						DebugLog.log(getName()+" pushing req "+req+" to "+req.empId);
					}
					
				} catch (InterruptedException e) {
					DebugLog.log(getName()+" Error putting to emp queue"+req.empId);					
					e.printStackTrace();
				}
			}
		}
		
	}

}


