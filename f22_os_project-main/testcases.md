# Test Cases

##Inital Employee Data Files

**1234.dat**
```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat 1234.dat
"staff meeting","conference room",2022-12-20T15:30,60
"status meeting","conference room",2022-12-20T11:30,30
"team lunch","Chipotle",2022-12-21T11:30,90
"doctor appointment","UMC",2022-12-19T15:30,45
```

**4567.dat**
```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat 4567.dat
"staff meeting","conference room",2022-12-20T15:30,60
"stand up","hallway",2022-12-20T08:30,30
"team lunch","Chipotle",2022-12-21T11:30,90
"dentist","capstone dental",2022-12-18T11:30,60
```

**6789.dat**
```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat 6789.dat
"staff meeting","conference room",2022-12-20T15:30,60
"stand up","hallway",2022-12-19T08:30,30
"parent teacher conference","TMS",2022-12-21T11:30,90
```

**012345678.dat** < Empty
```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat 0123456789.dat
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> 
```

## Test Compilation

**Make**
```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> make
javac -d . -classpath . ./edu/cs300/*.java
gcc -std=c99 -lpthread -D_GNU_SOURCE  request_mtgs.c -o request_mtgs
javac -cp ./ -h . ./edu/cs300/MessageJNI.java
gcc -c -fPIC -I/usr/java/latest/include -I/usr/java/latest/include/linux -Dlinux system5_msg.c -o edu_cs300_MessageJNI.o
gcc -shared -o libsystem5msg.so edu_cs300_MessageJNI.o -lc
gcc -std=c99 -D_GNU_SOURCE -Dlinux msgsnd_mtg_request.c -o msgsnd
gcc -std=c99 -D_GNU_SOURCE msgrcv_mtg_response.c -o msgrcv
linux
```

## Test Basic Input Message

**Input**

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat input.msg 
1,1234,"morning mtg","conf room",2022-12-17T14:30,60
2,1234,"conflict mtg","zoom",2022-12-17T15:00,60
0,9999,"any","any",any,60
```

**C Ouput**
```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> ./request_mtgs < input.msg 
Meeting request 1 for employee 1234 was accepted (morning mtg @ conf room starting 2022-12-17T14:30 for 60 minutes
Meeting request 2 for employee 1234 was rejected due to conflict (conflict mtg @ zoom starting 2022-12-17T15:00 
for 60 minutes
```

**File Output**
```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat 1234.dat.bak 
"morning mtg","conf room",2022-12-17T14:30,60
"doctor appointment","UMC",2022-12-19T15:30,45
"status meeting","conference room",2022-12-20T11:30,30
"staff meeting","conference room",2022-12-20T15:30,60
"team lunch","Chipotle",2022-12-21T11:30,90
```

## Test Maximum Amount of Requests

**Input**

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat test_max_input.msg 
1,1234,"Meeting 1","zoom",2022-12-19T14:30,60
2,1234,"Meeting 1","zoom",2022-12-19T14:30,60
3,1234,"Meeting 1","zoom",2022-12-19T14:30,60
...
197,1234,"Meeting 1","zoom",2022-12-19T14:30,60
198,1234,"Meeting 1","zoom",2022-12-19T14:30,60
199,1234,"Meeting 1","zoom",2022-12-19T14:30,60
200,1234,"Meeting 1","zoom",2022-12-19T14:30,60
0,9999,"any","any",any,60
```

**C Output**

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> ./request_mtgs < test_max_input.msg 
Meeting request 1 for employee 1234 was accepted (Meeting 1 @ zoom starting 2022-12-19T14:30 for 60 minutes
Meeting request 2 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 3 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 5 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 4 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 6 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 7 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 8 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 9 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 12 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 11 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 10 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 13 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 14 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 18 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 16 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 15 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 17 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 19 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 20 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 21 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 24 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 25 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 26 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 27 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 28 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 29 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 23 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 22 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 32 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 30 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 33 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 31 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 35 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 34 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 36 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 37 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 38 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 41 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 42 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 39 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 40 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 44 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 45 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 46 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 43 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 47 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 48 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 49 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 50 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 53 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 54 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 55 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 56 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 57 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 58 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 59 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 60 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 51 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 52 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 61 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 62 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 63 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 64 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 65 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 67 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 68 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 66 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 69 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 70 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 71 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 72 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 74 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 76 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 75 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 77 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 78 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 79 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 80 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 73 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 81 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 84 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 82 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 83 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 85 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 86 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 88 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 89 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 91 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 92 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 87 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 90 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 94 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 93 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 95 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 96 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 97 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 98 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 99 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 100 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for
 60 minutes
Meeting request 101 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 102 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 103 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 104 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 105 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 106 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 107 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 108 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 109 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 110 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 112 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 111 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 113 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 114 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 116 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 118 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 119 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 122 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 124 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 115 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 117 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 120 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 121 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 123 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 125 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 126 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 127 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 128 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 129 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 131 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 133 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 135 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 137 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 139 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 141 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 143 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 145 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 147 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 148 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 150 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 152 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 155 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 154 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 151 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 159 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 153 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 130 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 160 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 132 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 157 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 134 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 136 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 138 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 140 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 142 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 144 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 146 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 149 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 163 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 167 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 165 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 169 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 161 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 168 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 166 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 164 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 162 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 156 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 158 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 170 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 171 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 179 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 174 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 175 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 177 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 172 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 173 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 176 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 178 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 180 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 182 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 181 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 183 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 184 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 185 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 186 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 187 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 188 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 189 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 190 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 192 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 191 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 194 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 193 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 195 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 196 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 197 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 198 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 199 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
Meeting request 200 for employee 1234 was rejected due to conflict (Meeting 1 @ zoom starting 2022-12-19T14:30 for 
60 minutes
```

**File Output**

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat 1234.dat.bak 
"Meeting 1","zoom",2022-12-19T14:30,60
"doctor appointment","UMC",2022-12-19T15:30,45
"status meeting","conference room",2022-12-20T11:30,30
"staff meeting","conference room",2022-12-20T15:30,60
"team lunch","Chipotle",2022-12-21T11:30,90
```

## Test Maximum Length Input Request

**Input**

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat test_max_length_request.msg 
1,1234,"","",2022-12-19T14:30,60
2,1234,"","",2022-12-19T14:30,60
3,1234,"","",2022-12-19T14:30,60
4,1234,"","",2022-12-19T14:30,60
5,1234,"","",2022-12-19T14:30,60
6,1234,"","",2022-12-19T14:30,60
7,1234,"","",2022-12-19T14:30,60
8,1234,"","",2022-12-19T14:30,60
9,1234,"","",2022-12-19T14:30,60
10,1234,"","",2022-12-19T14:30,60
11,1234,"","",2022-12-19T14:30,60
12,1234,"","",2022-12-19T14:30,60
13,1234,"","",2022-12-19T14:30,60
14,1234,"","",2022-12-19T14:30,60
15,1234,"","",2022-12-19T14:30,60
16,1234,"","",2022-12-19T14:30,60
17,1234,"","",2022-12-19T14:30,60
18,1234,"","",2022-12-19T14:30,60
19,1234,"","",2022-12-19T14:30,60
20,1234,"","",2022-12-19T14:30,60
21,1234,"","",2022-12-19T14:30,60
22,1234,"","",2022-12-19T14:30,60
23,1234,"","",2022-12-19T14:30,60
24,1234,"","",2022-12-19T14:30,60
25,1234,"","",2022-12-19T14:30,60
26,1234,"","",2022-12-19T14:30,60
27,1234,"","",2022-12-19T14:30,60
28,1234,"","",2022-12-19T14:30,60
29,1234,"","",2022-12-19T14:30,60
30,1234,"","",2022-12-19T14:30,60
31,1234,"","",2022-12-19T14:30,60
32,1234,"","",2022-12-19T14:30,60
33,1234,"","",2022-12-19T14:30,60
34,1234,"","",2022-12-19T14:30,60
35,1234,"","",2022-12-19T14:30,60
36,1234,"","",2022-12-19T14:30,60
37,1234,"","",2022-12-19T14:30,60
38,1234,"","",2022-12-19T14:30,60
39,1234,"","",2022-12-19T14:30,60
40,1234,"","",2022-12-19T14:30,60
41,1234,"","",2022-12-19T14:30,60
42,1234,"","",2022-12-19T14:30,60
43,1234,"","",2022-12-19T14:30,60
44,1234,"","",2022-12-19T14:30,60
45,1234,"","",2022-12-19T14:30,60
46,1234,"","",2022-12-19T14:30,60
47,1234,"","",2022-12-19T14:30,60
48,1234,"","",2022-12-19T14:30,60
49,1234,"","",2022-12-19T14:30,60
50,1234,"","",2022-12-19T14:30,60
51,1234,"","",2022-12-19T14:30,60
52,1234,"","",2022-12-19T14:30,60
53,1234,"","",2022-12-19T14:30,60
54,1234,"","",2022-12-19T14:30,60
55,1234,"","",2022-12-19T14:30,60
56,1234,"","",2022-12-19T14:30,60
57,1234,"","",2022-12-19T14:30,60
58,1234,"","",2022-12-19T14:30,60
59,1234,"","",2022-12-19T14:30,60
60,1234,"","",2022-12-19T14:30,60
61,1234,"","",2022-12-19T14:30,60
62,1234,"","",2022-12-19T14:30,60
63,1234,"","",2022-12-19T14:30,60
64,1234,"","",2022-12-19T14:30,60
65,1234,"","",2022-12-19T14:30,60
66,1234,"","",2022-12-19T14:30,60
67,1234,"","",2022-12-19T14:30,60
68,1234,"","",2022-12-19T14:30,60
69,1234,"","",2022-12-19T14:30,60
70,1234,"","",2022-12-19T14:30,60
71,1234,"","",2022-12-19T14:30,60
72,1234,"","",2022-12-19T14:30,60
73,1234,"","",2022-12-19T14:30,60
74,1234,"","",2022-12-19T14:30,60
75,1234,"","",2022-12-19T14:30,60
76,1234,"","",2022-12-19T14:30,60
77,1234,"","",2022-12-19T14:30,60
78,1234,"","",2022-12-19T14:30,60
79,1234,"","",2022-12-19T14:30,60
80,1234,"","",2022-12-19T14:30,60
81,1234,"","",2022-12-19T14:30,60
82,1234,"","",2022-12-19T14:30,60
83,1234,"","",2022-12-19T14:30,60
84,1234,"","",2022-12-19T14:30,60
85,1234,"","",2022-12-19T14:30,60
86,1234,"","",2022-12-19T14:30,60
87,1234,"","",2022-12-19T14:30,60
88,1234,"","",2022-12-19T14:30,60
89,1234,"","",2022-12-19T14:30,60
90,1234,"","",2022-12-19T14:30,60
91,1234,"","",2022-12-19T14:30,60
92,1234,"","",2022-12-19T14:30,60
93,1234,"","",2022-12-19T14:30,60
94,1234,"","",2022-12-19T14:30,60
95,1234,"","",2022-12-19T14:30,60
96,1234,"","",2022-12-19T14:30,60
97,1234,"","",2022-12-19T14:30,60
98,1234,"","",2022-12-19T14:30,60
99,1234,"","",2022-12-19T14:30,60
100,0123456789,"1234567890123456789012345678","1234567890123456789012345678",2022-12-19T14:30,2147483647
0,9999,"any","any",any,60
```

**C Output**

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> ./request_mtgs < 
test_max_length_request.msg 
Meeting request 3 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 4 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 7 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 6 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 8 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 11 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 9 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 10 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 2 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 5 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 15 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 16 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 12 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 13 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 14 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 17 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 1 for employee 1234 was accepted ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 18 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 19 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 20 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 21 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 22 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 23 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 24 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 25 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 26 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 27 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 28 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 30 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 31 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 32 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 33 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 34 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 35 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 36 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 37 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 38 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 39 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 40 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 41 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 42 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 29 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 43 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 46 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 49 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 51 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 52 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 50 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 48 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 47 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 55 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 58 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 53 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 54 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 56 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 57 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 59 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 60 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 62 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 63 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 64 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 65 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 66 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 67 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 61 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 68 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 69 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 70 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 44 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 45 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 71 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 72 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 73 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 74 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 75 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 76 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 77 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 78 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 79 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 80 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 82 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 81 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 88 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 94 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 100 for employee 0123456789 was accepted (1234567890123456789012345678 @ 1234567890123456789012345678 
starting 2022-12-19T14:30 for 2147483647 minutes
Meeting request 84 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 83 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 92 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 91 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 95 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 87 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 96 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 93 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 98 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 89 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 97 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 90 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 85 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 86 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
Meeting request 99 for employee 1234 was rejected due to conflict ( @  starting 2022-12-19T14:30 for 60 minutes
```

**File Output**

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat 1234.dat.bak 
"","",2022-12-19T14:30,60
"doctor appointment","UMC",2022-12-19T15:30,45
"status meeting","conference room",2022-12-20T11:30,30
"staff meeting","conference room",2022-12-20T15:30,60
"team lunch","Chipotle",2022-12-21T11:30,90
```

## Test Time Comparisons

**Input**

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat test_time_comparisons.msg 
1,1234,"Add to head","zoom",2022-12-19T14:30,60
2,1234,"Add to tail","zoom",2022-12-21T13:00,60
3,1234,"Middle start touching","zoom",2022-12-19T16:15,20
4,1234,"Middle end touching","zoom",2022-12-20T11:00,30
5,1234,"Middle not touching","zoom",2022-12-20T13:00,60
0,9999,"any","any",any,60
```

**C Output**

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> ./request_mtgs < 
test_time_comparisons.msg 
Meeting request 4 for employee 1234 was accepted (Middle end touching @ zoom starting 2022-12-20T11:00 for 30 minutes
Meeting request 2 for employee 1234 was accepted (Add to tail @ zoom starting 2022-12-21T13:00 for 60 minutes
Meeting request 3 for employee 1234 was accepted (Middle start touching @ zoom starting 2022-12-19T16:15 for 20 minutes
Meeting request 1 for employee 1234 was accepted (Add to head @ zoom starting 2022-12-19T14:30 for 60 minutes
Meeting request 5 for employee 1234 was accepted (Middle not touching @ zoom starting 2022-12-20T13:00 for 60 minutes
```

**File Output**

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat 1234.dat.bak 
"Add to head","zoom",2022-12-19T14:30,60
"doctor appointment","UMC",2022-12-19T15:30,45
"Middle start touching","zoom",2022-12-19T16:15,20
"Middle end touching","zoom",2022-12-20T11:00,30
"status meeting","conference room",2022-12-20T11:30,30
"Middle not touching","zoom",2022-12-20T13:00,60
"staff meeting","conference room",2022-12-20T15:30,60
"team lunch","Chipotle",2022-12-21T11:30,90
"Add to tail","zoom",2022-12-21T13:00,60
```

## Test Multiple Employee Requests

**Input**

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat test_multiple_employees.msg 
1,1234,"Add to 1234 emp","zoom",2022-12-22T00:00,1
2,6789,"Add to 6789 emp","zoom",2022-12-22T00:00,12345
3,4567,"Add to 4567 emp","zoom",2023-01-01T12:34,10
0,9999,"any","any",60
```

**C Output**

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> ./request_mtgs < 
test_multiple_employees.msg 
Meeting request 1 for employee 1234 was accepted (Add to 1234 emp @ zoom starting 2022-12-22T00:00 for 1 minutes
Meeting request 3 for employee 4567 was accepted (Add to 4567 emp @ zoom starting 2023-01-01T12:34 for 10 minutes
Meeting request 2 for employee 6789 was accepted (Add to 6789 emp @ zoom starting 2022-12-22T00:00 for 12345 minutes
```

**File Output**

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat 1234.dat.bak 
"doctor appointment","UMC",2022-12-19T15:30,45
"status meeting","conference room",2022-12-20T11:30,30
"staff meeting","conference room",2022-12-20T15:30,60
"team lunch","Chipotle",2022-12-21T11:30,90
"Add to 1234 emp","zoom",2022-12-22T00:00,1
```

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat 4567.dat.bak 
"dentist","capstone dental",2022-12-18T11:30,60
"stand up","hallway",2022-12-20T08:30,30
"staff meeting","conference room",2022-12-20T15:30,60
"team lunch","Chipotle",2022-12-21T11:30,90
"Add to 4567 emp","zoom",2023-01-01T12:34,10
```

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat 6789.dat.bak 
"stand up","hallway",2022-12-19T08:30,30
"staff meeting","conference room",2022-12-20T15:30,60
"parent teacher conference","TMS",2022-12-21T11:30,90
"Add to 6789 emp","zoom",2022-12-22T00:00,12345
```

## Test Delayed Input

**C Input/Output**

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> ./request_mtgs 
1,1234,"Add to 1234 emp","zoom",2022-12-22T00:00,1
Meeting request 1 for employee 1234 was accepted (Add to 1234 emp @ zoom starting 2022-12-22T00:00 for 1 minutes
2,6789,"Add to 6789 emp","zoom",2022-12-22T00:00,12345
Meeting request 2 for employee 6789 was accepted (Add to 6789 emp @ zoom starting 2022-12-22T00:00 for 12345 minutes
0,9999,"any","any",any,60
```

**File Output**

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat 1234.dat.bak 
"doctor appointment","UMC",2022-12-19T15:30,45
"status meeting","conference room",2022-12-20T11:30,30
"staff meeting","conference room",2022-12-20T15:30,60
"team lunch","Chipotle",2022-12-21T11:30,90
"Add to 1234 emp","zoom",2022-12-22T00:00,1
```

```
achankins@cs-operatingsystems01:~/CS300_ThreadingProject/f22_os_project-main> cat 6789.dat.bak 
"stand up","hallway",2022-12-19T08:30,30
"staff meeting","conference room",2022-12-20T15:30,60
"parent teacher conference","TMS",2022-12-21T11:30,90
"Add to 6789 emp","zoom",2022-12-22T00:00,12345
```