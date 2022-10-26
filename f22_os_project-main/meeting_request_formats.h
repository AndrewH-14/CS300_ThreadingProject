#ifndef LWS_H
#define LWS_H


#define DESCRIPTION_MAX_LENGTH 30
#define LOCATION_MAX_LENGTH 30
#define EMP_ID_MAX_LENGTH 10
#define DESCRIPTION_FIELD_LENGTH DESCRIPTION_MAX_LENGTH+1
#define LOCATION_FIELD_LENGTH LOCATION_MAX_LENGTH+1
#define EMP_ID_FIELD_LENGTH EMP_ID_MAX_LENGTH+1
#define DATETIME_LENGTH 16

// Declare the message structure

//Report request from ReportingSystem
typedef struct meetingrequestbuf {
  long mtype;
  int request_id;
  char empId[EMP_ID_FIELD_LENGTH];
  char description_string[DESCRIPTION_FIELD_LENGTH];
  char location_string[LOCATION_FIELD_LENGTH];
  char datetime[17];//2022-12-20T08:30
  int duration;
} meeting_request_buf;

#define SEND_BUFFER_LENGTH sizeof(meeting_request_buf)-sizeof(long)


//Report record

typedef struct meetingresponsebuf {
	long mtype;
	int request_id;
  int avail;//0 is no and 1 is yes
} meeting_response_buf;
#endif
