package edu.cs300;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Scanner;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.Set;

public class CalendarManager {
	
	Hashtable<String,ArrayBlockingQueue<MeetingRequest>> empQueueMap;
	ArrayBlockingQueue<MeetingResponse> resultsOutputArray;
	
	public CalendarManager() {
		// A queue that reposne message will be added to
		this.resultsOutputArray = new ArrayBlockingQueue<MeetingResponse>(30);
		// A hashtable of queues that the requests will be added to
		empQueueMap = new Hashtable<String,ArrayBlockingQueue<MeetingRequest>>();

		// An array of the employee worker threads
		ArrayList<Employee> employeeArray = new ArrayList<>();
		

		// Read the employees information from the employees.csv file
		// File reading inspired by: https://www.w3schools.com/Java/java_files_read.asp
		try {
			File employeeFile   = new File("employees.csv");
			Scanner fileScanner = new Scanner(employeeFile);

			while (fileScanner.hasNextLine()) {

				String   employeeInformationString = fileScanner.nextLine();
				String[] employeeInformationArray  = employeeInformationString.split(",");

				// Create the incoming message queue for this employee that will
				// be passed on to the worker thread
				ArrayBlockingQueue<MeetingRequest> queue = new ArrayBlockingQueue<MeetingRequest>(10);
				empQueueMap.put(employeeInformationArray[0], queue);


				Employee Thread = new Employee(employeeInformationArray[0], 
							 				   employeeInformationArray[1], 
							 				   employeeInformationArray[2], 
							 				   queue, 
							 				   this.resultsOutputArray);
				Thread.start();
				employeeArray.add(Thread);
			}

			fileScanner.close();
		} catch (FileNotFoundException e) {
			System.out.println("An error occured when opening employess.csv");
		}

		// Create an outgoing meeting response thread that will recieve every
		// messge and send it to the system V queue
		new OutputQueueProcessor(this.resultsOutputArray).start();

		// Create an incoming meeting request thread that will add the incoming
		// requests to the appropriate employee's queue for the worker threads
		// to read from
		new InputQueueProcessor(this.empQueueMap).start();

		// Wait for all of the employee worker threads to finish before closing
		// off the output queue and exiting
		for (Employee employee : employeeArray)
		{
			try {
				employee.join();
			} catch (Exception e) {
				System.out.println("An error occured when joining the threads.");
			}
		}

		// Once here all of the employee worker threads have backed up their 
		// data and exited, therefore we should stop the output queue thread
		// and exit the program
		try {
			this.resultsOutputArray.put(new MeetingResponse(0, 0));
		} catch (InterruptedException e) {
			System.err.println(e.getMessage());
		}
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

						if (res.request_id != 0) {
							MessageJNI.writeMtgReqResponse(res.request_id, res.avail);
							DebugLog.log(getName()+" writing response "+res);
						} else {
							break;
						}
					
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
					} else if (req.request_id == 0) {
						// Signal to all employee worker threads that all input
						// messages have been received
						// Hashtable iteration inspired by:
						// https://www.geeksforgeeks.org/how-to-iterate-through-hashtable-in-java/
						Set<String> setOfKeys = empQueueMap.keySet();
						for (String key : setOfKeys) {
							empQueueMap.get(key).put(req);
						}
						// Exit the while loop
						break;
					}
					
				} catch (InterruptedException e) {
					DebugLog.log(getName()+" Error putting to emp queue"+req.empId);					
					e.printStackTrace();
				}
			}
		}
		
	}

}


