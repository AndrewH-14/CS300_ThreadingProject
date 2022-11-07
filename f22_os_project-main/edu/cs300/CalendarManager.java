package edu.cs300;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Scanner;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.Set;

/**
 * The CalendarManager class will be used to start the application. This class
 * is responsible for creating input queues for all of the employee's in the
 * employee.csv file, starting threads to concurrently process requests, creating
 * an output that the threads can write their responses to, and exiting once
 * all of the requests have been processed.
 */
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

				String employeeID 	    = employeeInformationArray[0];
				String employeeDataFile = employeeInformationArray[1];
				String employeeName     = employeeInformationArray[2];

				// Create the incoming message queue for this employee that will
				// be passed on to the worker thread. Only ten meeting requests
				// will be allowed on the queue at a single time.
				ArrayBlockingQueue<MeetingRequest> queue = new ArrayBlockingQueue<MeetingRequest>(10);
				empQueueMap.put(employeeID, queue);

				Employee thread = new Employee(employeeID, 
							 				   employeeDataFile, 
							 				   employeeName, 
							 				   queue, 
							 				   this.resultsOutputArray);
				thread.start();
				employeeArray.add(thread);
			}

			fileScanner.close();
		} catch (FileNotFoundException e) {
			System.out.println("An error occured when opening employess.csv");
		}

		// Create an single outgoing meeting response thread that will recieve every
		// messge and send it to the system V queue
		new OutputQueueProcessor(this.resultsOutputArray).start();

		// Create an incoming meeting request thread that will add the incoming
		// requests to the appropriate employee's queue for the worker threads
		// to read from
		new InputQueueProcessor(this.empQueueMap).start();

		// Wait for all of the employee worker threads to finish before closing
		// off the output queue and exiting
		for (Employee employee : employeeArray) {
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
	 
	/**
	 * The main function for the CalendarManager class. Immediately initialzes an 
	 * instance of the CalendarManager.
	 * 
	 * @param args Any arguments that are passed in upon run time. No arguemnts
	 * 			   will be used in this application.
	 */
	public static void main(String args[]) {
		
		CalendarManager mgr = new CalendarManager();
	}
	
	/**
	 * The OuputQueueProcessor class implements a thread that will handle all
	 * of the responses that need to be sent via the System V message queue.
	 */
	class OutputQueueProcessor extends Thread {
		
		ArrayBlockingQueue<MeetingResponse> resultsOutputArray;
		
		/**
		 * Constructor for the OutputQueueProcessor class.
		 * 
		 * @param resultsOutputArray The queue that responses should be read from.
		 */
		OutputQueueProcessor(ArrayBlockingQueue<MeetingResponse> resultsOutputArray){
			this.resultsOutputArray=resultsOutputArray;
		}
		
		/**
		 * Thread function which handles all of the responses added to the 
		 * output queue. This thread will exit once it has received a signal
		 * that all responses have been sent.
		 */
		public void run() {
			DebugLog.log(getName()+" processing responses ");

			// Blocking on the output queue will prevent the infinite loop from
			// wasting CPU cycles.
			while (true) {
				try {
					// ArrayBlockingQueues are thread safe so no need to worry about
					// concurrency issues
					// https://stackoverflow.com/questions/26543807/is-blockingqueue-completely-thread-safe-in-java
					MeetingResponse res = resultsOutputArray.take();

						if (res.request_id != 0) { // A valid message has been recieved
							MessageJNI.writeMtgReqResponse(res.request_id, res.avail);
							DebugLog.log(getName()+" writing response "+res);
						} else { // Ending signal recieved
							break;
						}
					
				} catch (Exception e) {
					System.out.println("Sys5OutputQueueProcessor error "+e.getMessage());
				}
			}
		}
	}

	/**
	 * The InputQueueProcessor class implements a thread that will handle all
	 * of the responses that need to be read via the System V message queue.
	 */
	class InputQueueProcessor extends Thread {
		Hashtable<String,ArrayBlockingQueue<MeetingRequest>> empQueueMap;
		
		/**
		 * Constructor for the InputQueueProcessor class.
		 * 
		 * @param empQueueMap A hashtable of ArrayBlockingQueues where the received
		 * 					  messages should be put.  
		 */
		InputQueueProcessor(Hashtable<String,ArrayBlockingQueue<MeetingRequest>> empQueueMap){
			this.empQueueMap=empQueueMap;
		}

		/**
		 * Thread function which will handle all of the incoming requests via the
		 * System V message queue. Once the signal has been recieved signalling the
		 * end of request messages, this thread will send a signal to all employee
		 * threads that it is time to backup their data and exit.
		 */
		public void run(){
			while (true) {
				MeetingRequest req = MessageJNI.readMeetingRequest();
				try {
					DebugLog.log(getName()+"recvd msg from queue for "+req.empId);
					if (empQueueMap.containsKey(req.empId)) {
						empQueueMap.get(req.empId).put(req);
						DebugLog.log(getName()+" pushing req "+req+" to "+req.empId);
					} else if (req.request_id == 0) { // Signal that all requests have been received
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


