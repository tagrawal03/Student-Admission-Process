NAME: TARUNA AGRAWAL
ID: 7650184685
****************************************************************************************************************************
IMPORTANT NOTES:
----------------
1)StuX indicates StudentX
2)Run Department files first in two seperate windows then Student file in seperate window.

*****************************************************************************************************************************
FILES INCLUDED and FUNCTIONALITIES:
-----------------------------------
Admission.cpp: Server->Accept files sent by department A and B and store it in another text file DepA.txt and DepB.txt.
These text files will be created at local folder.
Accept file ssent by Student1, Student2, Student3, Student4, Student5 and store it in local file Stu1.txt, Stu2.txt, Stu3.txt, Stu4.txt, Stu5.txt.
Once all the data is received Admission processing is performed on each Student file and results are returned to both Student and Department by creating 7 UDP connections.

DepartmentA.cpp: Client-> Open TCP connection with Admission office to sent programs one by one and opens UDP connection to receive Admission result.

DepartmentB.cpp: Client-> Open TCP connection with Admission office to sent programs one by one and opens UDP connection to receive Admission result. 

Student1.cpp: Opens TCP connection with Admission office to send data as packet to server(Admission.cpp). It opens another UDP connection to receive the result from Admission office.

Student2.cpp:Opens TCP connection with Admission office to send data as packet to server(Admission.cpp). It opens another UDP connection to receive the result from Admission office.

Student3.cpp:Opens TCP connection with Admission office to send data packet by packet to server(Admission.cpp). It opens another UDP connection to receive the result from Admission office

Student4.cpp:Opens TCP connection with Admission office to send data packet by packet to server(Admission.cpp). It opens another UDP connection to receive the result from Admission office.

Student5.cpp:Opens TCP connection with Admission office to send data packet by packet to server(Admission.cpp). It opens another UDP connection to receive the result from Admission office.

Makefile: Compiles all *.cpp to create executables Admission, DepartmentA, DepartmentB, Student1, Student2, Student3, Student4, Student5.

*********************************************************************************************************************************
STEPS TO RUN PROGRAM:
---------------------
1) Unzip the folder at nunki server and check if all files mentioned above are contained in folder, also input files need to be put in same folder.
2) Run following commands from the folder which contains Makefile:
   make clean
   make all
3) Run the executables in following order in four seperate windows (one for server and another three for     client):
  ./Admissionoutput - One window
  ./DepartmentAoutput - Second window
  ./DepartmentBoutput - third window
  ./Student1output - Fourth window
  ./Student2output
  ./Student3output
  ./Student4output
  ./Student5output

Note: Files should be run in order mentioned if Student file is run before Department the following error  will be displayed: "Error: Dereferencing NULL pointer. Run Department A and Department B first".
Also, results will be displayed only after all the students have submitted application.

4) You should see all the required messages as well as new file created DepA.txt, DepB.txt, Stu1.txt, Stu2.txt, Stu3.txt, Stu4.txt, Stu5.txt which contains the data sent by Department and Student client respectively.

********************************************************************************************************************************
RESULT
------
Final result will be displayed on Student as well as Department screen.
Department.cpp
--------------
<Student details> has been admiited to department A. (Stu<x> indicates Student<x>)
Student.cpp
-----------
Student 1 has received application result <Result>
In case Student application is Invalid then no results will be displayed since it is mentioned in the Project document that Student should receive 0 from Admission office in case of Invalid application.

Note: Final results can also be seen on files:
DepartmentA_result.txt
DepartmentB_result.txt
Student1_result.txt
Student2_result.txt
Student3_result.txt
Student4_result.txt
Student5_result.txt

***********************************************************************************************************************************
DETAILED DESCRIPTION OF IMPLEMENTATION
--------------------------------------
Client: DepartmentA.cpp, DepartmentB.cpp, Student1.cpp, Student2.cpp, Student3.cpp, Student4.cpp, Student5.cpp
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Part1:
1) Client opens a TCP connection and connects with server(PORT:3985). Ipaddress is assigned to client locally by the machine and port number is obtained dynamically.
2) getaddressinfo() is used to the address information of server.
3) Once the client-server connection is sucessful client first sends the name of file to server.(Example: DepartmentA.cpp sends DepA.txt to server).
4) After filename is sent client opens the input text file to read each line (getline()) and then send the line to server using sendall() function.Between each program sent delay of 10millisecond has been introduced inorder to give sufficient time for server to receive the packet and write it in file locally.
5) Sendall() tries to send the complete data from client to server and if whole data doesn't go out it returns an error.
6) close the socket and file once complete data goes out.
7) Also a delay of 1seconds is put in each file in order to make sure that other files starts sending data once previous files has finished sending data.
Part2:
1) UDP connection is created from each client to Admission office in order to receive results calculated byA dmission office.
2) Students with Valid application receives reply from Admission office. Students with Invalid application dont receive any reply.(resulst displayed on screen as well as stored in file StudentX_result.txt).
3) Department A and Department B receives result of all admitted students.(Displayed on screen as well as stored in file DepartmentA_result.txt and DepartmentB_result.txt)

Server: Admission.cpp
~~~~~~~~~~~~~~~~~~~~~~
Part1:
1) Opens the socket at PORT 3985 and uses setsockopt() to clear the address for port already in use.
2) QUEUELEN for maximum number of connections is 10.
3) Once socket is created server listens for any incoming connections.
4) I have used select() implementation to keep track of incoming client connections and read from sockets whenever data is ready to be read.
5) FD_ISSET() indicates which file descriptors are ready to read. If server socket is ready to read we will accept new connection otherwise if client socket is ready to read we will read the data sent by client. This data is locally received in buf array and then it is written to the file. 
6) Once client opens the connection with server, first message received is filename therefore as soon a server receives the filename it creates the text file with corresponding filename.(counter variable is used to keep track of first packet).
7) If number of bytes received is 0 or less than 0 client socket is closed along with file and client connection is removed from master set.
8) sigchld_handler() is used to reap dead process.

Part2:
1) Opens 7 UDP connections to send out Admission result to Student and Departments.
2) Once StudentX data is received Admission office starts the processing based on previously recored Department data and stores the result in stuInfo[] array.
3) After all five student data is received Admission office sends result of admitted student to Departments
4) Once results are sent to Department all memory allocated to store Student and Department data is freed up in order to avoid memory leaks. 

*********************************************************************************************************************************
IDIOSYNCRASY:
--------------
1) After running DepartmentA.cpp wait for program to write the contents to file DepA.txt then run DepartmentB.cpp since I have not taken lock in Phase1 therefore if both are run parallely server will have concurrent receive and writing on files might be mixed up.
2) If input text file don't follow the convention as mentioned in document then code fails to read file.
3) Makefile is only for running on SunOS, this makefile wont work for compilation on local PC.
4) HOSTNAME is given as nunki.usc.edu so client will only connect if server runs on nunki. However, this macro can be changed from client files based on need.
5) DepartmentA.cpp DepartmentB.cpp should be run before running Student files.
6) Exactly five student files should be run because Admission office deletes the data of Student and Department after sending out the results for five Students.

*********************************************************************************************************************************
REFERENCE:
----------
Server and client socket creation code has been used from Beej's guide.


