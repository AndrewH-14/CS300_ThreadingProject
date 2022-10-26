package edu.cs300;

import java.util.concurrent.*;


//statements only relevant on Mac; Use statements on readme instead
//javac -h . MessageJNI.java
//gcc -c -fPIC -I${JAVA_HOME}/include -I${JAVA_HOME}/include/darwin system5_msg.c -o edu_cs300_MessageJNI.o
//gcc -dynamiclib -o libsystem5msg.dylib edu_cs300_MessageJNI.o -lc
//java -cp . -Djava.library.path=. edu.cs300.MessageJNI

public class MessageJNI {

    static {
        System.loadLibrary("system5msg");
    }

    public static void main(String[] args) {
        try{
        //System.out.println(new MessageJNI().readStringMsg("anderson",65));
        	System.out.println("Running MessageJNI test routine");
        	MessageJNI.writeMtgReqResponse(1, 0);
        	System.out.println(MessageJNI.readMeetingRequest());
        	
        } catch (Exception e){
            System.err.println("Error:"+e);
        }
    }
    	
    public static native void writeMtgReqResponse(int mtgID, int availability);
    public static native MeetingRequest readMeetingRequest();

    // Declare a native method sayHello() that receives no arguments and returns void
    private static native String readStringMsg();

}