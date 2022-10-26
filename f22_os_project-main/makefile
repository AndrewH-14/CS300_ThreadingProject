# Define a variable for classpath
CLASS_PATH = .
JAVA_HOME=/usr/java/latest
JAVA_PKG=edu/cs300
JAVA_SRC_ROOT=.

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	OSFLAG := linux
	SHARED_LIB := libsystem5msg.so
	LINK_FLAGS := -shared
	Q_COL := 1
endif
ifeq ($(UNAME_S),Darwin)
	JAVA_HOME=/Library/Java/JavaVirtualMachines/jdk-18.0.1.1.jdk/Contents/Home
	OSFLAG := darwin
	SHARED_LIB := libsystem5msg.dylib
	LINK_FLAGS := -dynamiclib
	Q_COL := 3
endif

.SUFFIXES: .java .class

all:
	@echo $(OSFLAG)


.java.class:
	@mkdir -p $(CLASS_PATH)
	javac -d $(CLASS_PATH) -classpath $(CLASS_PATH) $(JAVA_SRC_ROOT)/$(JAVA_PKG)/*.java

CLASSES = \
	$(JAVA_SRC_ROOT)/$(JAVA_PKG)/CalendarManager.java \
	$(JAVA_SRC_ROOT)/$(JAVA_PKG)/Worker.java \
	$(JAVA_SRC_ROOT)/$(JAVA_PKG)/MessageJNI.java \
	$(JAVA_SRC_ROOT)/$(JAVA_PKG)/DebugLog.java 

default: classes

all: classes request_mtgs $(SHARED_LIB) msgsnd msgrcv

classes: $(CLASSES:.java=.class)

archive: 
	make clean
	tar -cvzf ${USER}-f22os.tar.gz  makefile *.c *.h edu/cs300/*.java pom.xml *.csv *.dat *msg 

edu_cs300_MessageJNI.h: ./$(JAVA_SRC_ROOT)/$(JAVA_PKG)/MessageJNI.java
	javac -cp ${CLASS_PATH}/ -h . $(JAVA_SRC_ROOT)/$(JAVA_PKG)/MessageJNI.java
    
request_mtgs: request_mtgs.c meeting_request_formats.h
	gcc -std=c99 -lpthread -D_GNU_SOURCE $(MAC_FLAG) request_mtgs.c -o request_mtgs

edu_cs300_MessageJNI.o:meeting_request_formats.h edu_cs300_MessageJNI.h system5_msg.c queue_ids.h
	gcc -c -fPIC -I${JAVA_HOME}/include -I${JAVA_HOME}/include/$(OSFLAG) -D$(OSFLAG) system5_msg.c -o edu_cs300_MessageJNI.o

$(SHARED_LIB):meeting_request_formats.h edu_cs300_MessageJNI.h edu_cs300_MessageJNI.o
	gcc $(LINK_FLAGS) -o $(SHARED_LIB) edu_cs300_MessageJNI.o -lc

test: msgsnd msgrcv $(CLASSES) $(SHARED_LIB)
	./msgsnd 1 "1234" "meeting for 1234" "online" "2022-12-17T14:30" 60
	java -cp ${CLASS_PATH} -Djava.library.path=. edu/cs300/MessageJNI
	./msgrcv


msgsnd: msgsnd_mtg_request.c meeting_request_formats.h queue_ids.h
	gcc -std=c99 -D_GNU_SOURCE -D$(OSFLAG) msgsnd_mtg_request.c -o msgsnd

msgrcv: msgrcv_mtg_response.c meeting_request_formats.h queue_ids.h
	gcc -std=c99 -D_GNU_SOURCE msgrcv_mtg_response.c -o msgrcv


clean :
	@rm -f *.o $(SHARED_LIB) request_mtgs edu_cs300_MessageJNI.h msgsnd msgrcv 
	@rm -f ${USER}-f22os.tar.gz
	@rm -f ${CLASS_PATH}/edu/cs300/*class

clean_queues:
	@ipcs -q|grep ${USER}|while read line; do id=`echo $$line|cut -d' ' -f${Q_COL}`; echo $$id; ipcrm -Q $$id;done

