/*Author: TARUNA AGRAWAL
Admission.cpp
sendDep_UDPdata : creates UDP connection for Department and send the results
send_UDPdata: Creates UDP connection for Student and send the result
read_processData: Reads the data sent by the client and perfoems admission processing 
                  to get the result. In vase of INVALID entry Admission department sends 0 to student.
check_clientName: Checks which client connection has been initiated.
main(): creates aprent connection and accepst any incoming TCP client connection.
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sstream>

#define SERVER_PORT "3985"
#define STU1_PORT "22085"
#define STU2_PORT "22185"
#define STU3_PORT "22285"
#define STU4_PORT "22385"
#define STU5_PORT "22485"
#define DEPA_PORT "21885"
#define DEPB_PORT "21985"
#define QUEUELEN 10
#define MAXDATASIZE 1024
//#define HOSTNAME "taruna-Lenovo-B50-70"
#define HOSTNAME "nunki.usc.edu"

using namespace std;


string** mydepAInfo;
string** mydepBInfo;
string** stu1Info;
string** stu2Info;
string** stu3Info;
string** stu4Info;
string** stu5Info;
int countStu = 0;
unsigned int depAsock = 0;
unsigned int depBsock = 0;
string depAdatasend[5] = "0";
string depBdatasend[5] = "0";
struct sockaddr_in myaddr;  //address info of server
int addlen;


/*Function to create UDP connection for department and send the result*/
int sendDep_UDPdata(string port, string datasend[5])
{
  int udpsockfd;
  struct addrinfo hintsudp, *udpservinfo, *udp;
  int rv1, addlenUdp;
  int udpnumbytes;
  struct sockaddr_in their_addr, myaddrUdp;

 
  memset(&hintsudp,0, sizeof(hintsudp));
  hintsudp.ai_family = AF_INET;
  hintsudp.ai_socktype = SOCK_DGRAM;
  
  char *portnew = new char[port.length() + 1];
  strcpy(portnew, port.c_str());

  if ((rv1 = getaddrinfo(HOSTNAME, portnew, &hintsudp, &udpservinfo)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv1));
      return 1;
  }

  // loop through all the results and make a socket
  for(udp = udpservinfo; udp != NULL; udp = udp->ai_next) {
     if ((udpsockfd = socket(udp->ai_family, udp->ai_socktype,udp->ai_protocol)) == -1)
     {
       perror("udp: socket");
       continue;
     }
     break;
  }
  if (udp == NULL) {
     fprintf(stderr, "department: failed to bind socket\n");
     return 2;
  }
 
  /*Send data*/
  for (int d = 0; d <= 5; d++)
  {
    if (datasend[d] == "0")
      break;
    else
    {
       char *cstr = new char[datasend[d].length() + 1];
       strcpy(cstr, datasend[d].c_str());
       size_t len = strlen(cstr);

       if ((udpnumbytes = sendto(udpsockfd, cstr, len, 0, udp->ai_addr, udp->ai_addrlen)) == -1) {
          perror("department: sendto");
          exit(1);
       }
       if (port == DEPA_PORT)
       {
          cout << "The admission office has send one admitted student DepartmentA"<<endl;
       }
       else
       {
          cout << "The admission office has send one admitted student DepartmentB"<<endl;
       }
       delete []cstr;
    }
  }
  if ((udpnumbytes = sendto(udpsockfd, "1", 1, 0, udp->ai_addr, udp->ai_addrlen)) == -1) {
          perror("department: sendto");
          exit(1);
  }
 

  addlenUdp = sizeof(myaddrUdp);
  int getsock_check = getsockname(udpsockfd,(struct sockaddr *)&myaddrUdp,(socklen_t *)&addlenUdp) ;
  if (getsock_check== -1) {
      perror("getsockname");
      exit(1);
  }
  printf("Admission office has ip address: %s", inet_ntoa(myaddr.sin_addr));
  printf(" and UDP port number: %d for phase 2\n", (int) ntohs(myaddrUdp.sin_port));
 
 delete[]portnew; 
 freeaddrinfo(udpservinfo);
 close(udpsockfd);

 return 0;
}  //end sendDep_UDPdata()

/*Function to create UDP connection for Student and send the result*/
int send_UDPdata(string port, string datasend)
{
  int udpsockfd;
  struct addrinfo hintsudp, *udpservinfo, *udp;
  int rv1, addlenUdp;
  int udpnumbytes;
  struct sockaddr_in their_addr, myaddrUdp;

 
  memset(&hintsudp,0, sizeof(hintsudp));
  hintsudp.ai_family = AF_INET;
  hintsudp.ai_socktype = SOCK_DGRAM;
  
  char *portnew = new char[port.length() + 1];
  strcpy(portnew, port.c_str());

  if ((rv1 = getaddrinfo(HOSTNAME, portnew, &hintsudp, &udpservinfo)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv1));
      return 1;
  }

  // loop through all the results and make a socket
  for(udp = udpservinfo; udp != NULL; udp = udp->ai_next) {
     if ((udpsockfd = socket(udp->ai_family, udp->ai_socktype,udp->ai_protocol)) == -1)
     {
       perror("udp: socket");
       continue;
     }
     break;
  }
  if (udp == NULL) {
     fprintf(stderr, "student: failed to bind socket\n");
     return 2;
  }
  //Send reply to Student
  if ((udpnumbytes = sendto(udpsockfd, "10", 2, 0, udp->ai_addr, udp->ai_addrlen)) == -1) {
     perror("student: sendto");
     exit(1);
  }

  char *cstr = new char[datasend.length() + 1];
  strcpy(cstr, datasend.c_str());
  size_t len = strlen(cstr);

  if ((udpnumbytes = sendto(udpsockfd, cstr, len, 0, udp->ai_addr, udp->ai_addrlen)) == -1) {
     perror("student: sendto");
     exit(1);
  }
  addlenUdp = sizeof(myaddrUdp);
  int getsock_check = getsockname(udpsockfd,(struct sockaddr *)&myaddrUdp,(socklen_t *)&addlenUdp) ;
  if (getsock_check== -1) {
      perror("getsockname");
      exit(1);
  }
  printf("Admission office has ip address: %s", inet_ntoa(myaddr.sin_addr));
  printf(" and UDP port number: %d for phase 2\n", (int) ntohs(myaddrUdp.sin_port));

  delete []portnew;
  delete []cstr;
  freeaddrinfo(udpservinfo);
  close(udpsockfd);
  return 0;
} //end send_UDPdata

/*Function to read contents of file and store in an array
Also carry out Admission processing if reading student file*/
string** read_processData(int numval, string filename)
{
  ifstream myfile(filename.c_str());
  string** depInfo = 0;
  string** stuInfo = 0;
  string line;
  int row =0;
  depInfo = new string*[numval];
  stuInfo = new string*[1];
  //Open file contain ing data sent by client
  if (myfile.is_open())
  {
      while(getline(myfile, line))
      {
        depInfo[row] = new string[2];
        stringstream fileline(line);
        string value;
        int col=0;
        while(getline(fileline,value,'#'))
        {
          depInfo[row][col]= value;
          col++;
        }
        row++;
      }   
      /*processing of data*/
      /*
       -- check if not dep data
       -- dep arrays should not be NULL
       -- do the processing
      */
      /*check if student file*/
     if (strncmp(filename.c_str(), "Stu", 3) == 0)
     {
        int accept = 0; //to keep track of accept applications
        int reject = 0; 
        int y;
        if (mydepAInfo != NULL && mydepBInfo !=NULL)
        {
           for(y = 1; y<=2; y++)
           {
               if (accept == 1)
                 break;
               stuInfo[0] = new string[6];
               for (int z = 0;  z<=2; z++)
               {   // check if GPA > department A criteria
                  if (depInfo[y][1] == mydepAInfo[z][0])
                  {
                    if (depInfo[0][1] >= mydepAInfo[z][1])
                    {
                      //store all info in temp array
                      string str1 = filename.substr(0,4);
                      stuInfo[0][0] = "DepartmentA";
                      stuInfo[0][1] = "Accept"; 
                      stuInfo[0][2] = mydepAInfo[z][0]; //program admitted
                      stuInfo[0][3] = depInfo[0][1];  //GPA 
                      stuInfo[0][4] = str1; //name of student
                      stuInfo[0][5] = "valid"; //valid application
                      accept = 1;
                      // break;
                    }
                    else
                    {
                      string str1 = filename.substr(0,4);
                      stuInfo[0][0] = "DepartmentA";
                      stuInfo[0][1] = "Reject"; 
                      stuInfo[0][2] = mydepAInfo[z][0]; //program rejected
                      stuInfo[0][3] = depInfo[0][1];  //GPA 
                      stuInfo[0][4] = str1; //name of student
                      stuInfo[0][5] = "valid"; //valid application
                      reject = 1;
                    }
                    break;
                   }
                   else if (depInfo[y][1] == mydepBInfo[z][0])
                   { //Department B
                     if (depInfo[0][1] >= mydepBInfo[z][1])
                     {
                      //store all info in stuInfo array
                       string str1 = filename.substr(0,4);
                       stuInfo[0][0] = "DepartmentB";
                       stuInfo[0][1] = "Accept"; 
                       stuInfo[0][2] = mydepBInfo[z][0]; //program admitted
                       stuInfo[0][3] = depInfo[0][1];  //GPA 
                       stuInfo[0][4] = str1; //name of student
                       stuInfo[0][5] = "valid"; //valid application
                       accept =1;
                     }
                     else
                     {
                       string str1 = filename.substr(0,4);
                       stuInfo[0][0] = "DepartmentB";
                       stuInfo[0][1] = "Reject"; 
                       stuInfo[0][2] = mydepAInfo[z][0]; //program rejected
                       stuInfo[0][3] = depInfo[0][1];  //GPA 
                       stuInfo[0][4] = str1; //name of student
                       stuInfo[0][5] = "valid"; //valid application
                       reject = 1;
                     }
                     break;
                   }
               } //end for department loop
            } //end for interest loop
        } //end of NULL loop
        else
        {
           cout << "Error: Dereferencing NULL pointer. Run Department A and Department B first"<<endl;
        }
        if ((y >= 2) && (accept == 0) && (reject == 0))
        {
          stuInfo[0][0] = "Invalid";
          stuInfo[0][1] = "Invalid"; 
          stuInfo[0][2] = "Invalid"; //program rejected
          stuInfo[0][3] = "Invalid";  //GPA 
          stuInfo[0][4] = "Invalid"; //name of student
          stuInfo[0][5] = "Invalid"; //Invalid application
        }
        myfile.close(); 
        return stuInfo;
    } //string compare
  }
  else
  {
    perror("unable to locate the file");
  }
    myfile.close(); 
   return depInfo;
}

/*Check which client has sent the request
 Create UDP socket to send the admission result*/

void check_clientName(char *arr)
{
 
  string datasend = "1";
  string port;
  int status;
  if (strncmp(arr, "DepA", 4) == 0)
  {
    mydepAInfo = read_processData(3, "DepA.txt");
  }
  else if (strncmp(arr, "DepB", 4) == 0)
  {
     mydepBInfo = read_processData(3, "DepB.txt");
  }
  else if (strncmp(arr, "Stu1", 4) == 0)
  {
     stu1Info = read_processData(3, "Stu1.txt");
     /*If accept store student data to send to department*/
     if (stu1Info[0][1] == "Accept")
     {
        datasend = stu1Info[0][1]+"#"+stu1Info[0][2]+"#"+stu1Info[0][0];
        if (stu1Info[0][0] == "DepartmentA")
        {
           depAdatasend[depAsock] = stu1Info[0][4]+"#"+stu1Info[0][3]+"#"+stu1Info[0][2];
           depAsock++;
        }
        else if (stu1Info[0][0] == "DepartmentB")
        {
           depBdatasend[depBsock] = stu1Info[0][4]+"#"+stu1Info[0][3]+"#"+stu1Info[0][2];
           depBsock++;
        }
     }
     else if (stu1Info[0][1] == "Reject")
     {
        datasend = "Reject";
     } 
     else
     {
        datasend = "0";
     }
     port = STU1_PORT;   
  }
  else if (strncmp(arr, "Stu2", 4) == 0)
  {
     stu2Info = read_processData(3, "Stu2.txt");
     if (stu2Info[0][1] == "Accept")
     {
        /*If accept store student data to send to department*/
        datasend = stu2Info[0][1]+"#"+stu2Info[0][2]+"#"+stu2Info[0][0];
        if (stu2Info[0][0] == "DepartmentA")
        {
           depAdatasend[depAsock] = stu2Info[0][4]+"#"+stu2Info[0][3]+"#"+stu2Info[0][2];
           depAsock++;
        }
        else if (stu2Info[0][0] == "DepartmentB")
        {
           depBdatasend[depBsock] = stu2Info[0][4]+"#"+stu2Info[0][3]+"#"+stu2Info[0][2];
           depBsock++;
        }
     }
     else if (stu2Info[0][1] == "Reject")
     {
        datasend = "Reject";
     } 
     else
     {
        datasend = "0";
     }
     port = STU2_PORT;   
  }
  else if (strncmp(arr, "Stu3", 4) == 0)
  {
     stu3Info = read_processData(3, "Stu3.txt");
     if (stu3Info[0][1] == "Accept")
     {
        /*If accept store student data to send to department*/
        datasend = stu3Info[0][1]+"#"+stu3Info[0][2]+"#"+stu3Info[0][0];
        if (stu3Info[0][0] == "DepartmentA")
        {
           depAdatasend[depAsock] = stu3Info[0][4]+"#"+stu3Info[0][3]+"#"+stu3Info[0][2];
           depAsock++;
        }
        else if (stu3Info[0][0] == "DepartmentB")
        {
           depBdatasend[depBsock] = stu3Info[0][4]+"#"+stu3Info[0][3]+"#"+stu3Info[0][2];
           depBsock++;
        }
     }
     else if (stu3Info[0][1] == "Reject")
     {
        datasend = "Reject";
     } 
     else
     {
        datasend = "0";
     }
     port = STU3_PORT;   
  }
  else if (strncmp(arr, "Stu4", 4) == 0)
  {
     stu4Info = read_processData(3, "Stu4.txt");
     if (stu4Info[0][1] == "Accept")
     {
        /*If accept store student data to send to department*/
        datasend = stu4Info[0][1]+"#"+stu4Info[0][2]+"#"+stu4Info[0][0];
        if (stu4Info[0][0] == "DepartmentA")
        {
           depAdatasend[depAsock] = stu4Info[0][4]+"#"+stu4Info[0][3]+"#"+stu4Info[0][2];
           depAsock++;
        }
        else if (stu4Info[0][0] == "DepartmentB")
        {
           depBdatasend[depBsock] = stu4Info[0][4]+"#"+stu4Info[0][3]+"#"+stu4Info[0][2];
           depBsock++;
        }
     }
     else if (stu4Info[0][1] == "Reject")
     {
        datasend = "Reject";
     } 
     else
     {
        datasend = "0";
     }
     port = STU4_PORT;   
  }
  else if (strncmp(arr, "Stu5", 4) == 0)
  {
     stu5Info = read_processData(3, "Stu5.txt");
     if (stu5Info[0][1] == "Accept")
     {
        /*If accept store student data to send to department*/
        datasend = stu5Info[0][1]+"#"+stu5Info[0][2]+"#"+stu5Info[0][0];
        if (stu5Info[0][0] == "DepartmentA")
        {
           depAdatasend[depAsock] = stu5Info[0][4]+"#"+stu5Info[0][3]+"#"+stu5Info[0][2];
           depAsock++;
        }
        else if (stu5Info[0][0] == "DepartmentB")
        {
           depBdatasend[depBsock] = stu5Info[0][4]+"#"+stu5Info[0][3]+"#"+stu5Info[0][2];
           depBsock++;
        }
     }
     else if (stu5Info[0][1] == "Reject")
     {
        datasend = "Reject";
     } 
     else
     {
        datasend = "0";
     }
     port = STU5_PORT;   
  }
  else
  {
    cout << "Could not locate the client" <<endl;
  }
  if (datasend != "1")  //if datasend has been filled by student processing send data to student
  {
     status = send_UDPdata(port, datasend);
     if (!status)
     {
       countStu++;  //keeps track of number of students 
       cout << "The admission office has send the application result to " << arr <<endl;
       cout <<'\n';
     }
  }
  // Sending result data to both departments
  if (countStu >= 5) //if all students data has been sent
  {
     status = sendDep_UDPdata(DEPA_PORT, depAdatasend);
     status = sendDep_UDPdata(DEPB_PORT, depBdatasend);
     if (!status)
       cout << "End of Phase 2 for the admission office"<<endl;
     for ( int h = 0; h < 3; h++)
     {
        delete [] mydepAInfo[h];
        delete [] mydepBInfo[h];
     }
     delete [] stu1Info[0];
     delete [] stu2Info[0];
     delete [] stu3Info[0];
     delete [] stu4Info[0];
     delete [] stu5Info[0];

     delete [] mydepAInfo;
     delete [] mydepBInfo;
     delete [] stu1Info;
     delete [] stu2Info;
     delete [] stu3Info;
     delete [] stu4Info;
     delete [] stu5Info;

     mydepAInfo = 0;
     mydepBInfo = 0;
     stu1Info =0;
     stu2Info = 0;
     stu3Info = 0;
     stu4Info = 0;
     stu5Info =0;
  }

}

void sigchld_handler(int s)
{
while(waitpid(-1, NULL, WNOHANG) > 0);
}
/*Get sock address*/
void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(void)
{
  struct addrinfo hints, *servinfo, *i;
  struct sigaction sa;
  struct sockaddr_storage thr_addr; // connector's address information
  socklen_t sin_size;
  memset(&myaddr.sin_zero,0,8);
 
  fd_set master;   //master file descriptor list
  fd_set read_fds; //temp file descriptor list for select()
  int fdmax;       //max file descriptor number
  
  int serv_sockfd, numbytes;
  int new_fd,p;
  int k=0; int counterServ = 0;
  int status;
  int yes = 1;
  int counter; //to keep track of file name sent
 
  char buf[MAXDATASIZE]; //recieve data
  char array[MAXDATASIZE];
  char s[INET6_ADDRSTRLEN];
  memset(&hints, 0, sizeof(hints));
  memset(array, 0, sizeof(array));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  FD_ZERO(&master);
  FD_ZERO(&read_fds);

  ofstream depfile; 

  if ((status = getaddrinfo(NULL, SERVER_PORT, &hints, &servinfo)) != 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
  }
   
  for (i = servinfo; i != NULL; i = i->ai_next)
  {
    /*Open the socket*/
    if ((serv_sockfd = socket(i->ai_family, i->ai_socktype,
            i->ai_protocol)) == -1)
    {
       perror("Server: open socket");
       continue;
    }
    /*Clearing the address for port already in use*/
    if(setsockopt(serv_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
       perror("setsockopt:");
       exit(1);
    }
    /*Bind the socket to address*/
    if (bind(serv_sockfd, i->ai_addr, i->ai_addrlen) == -1)
    {
       close(serv_sockfd);
       perror("Server: trying to bind");
       continue;
    }
    
    break;    
  }
   /*Check if socket has been opened*/
    if (i == NULL)
    {
       fprintf(stderr, "Server failed to bind\n");
       return 2;
    }
    freeaddrinfo(servinfo);
    
    if (listen(serv_sockfd, QUEUELEN) == -1)
    {
       perror("listen");
       exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
       perror("sigaction");
       exit(1);
    }
   
   //add listener to the master set
   FD_SET(serv_sockfd, &master);
   //keep track of biggest file descriptor
   fdmax = serv_sockfd;

   /*accept client connections*/ 
    for(;;)
    {
       read_fds = master; //copy it
       if(select(fdmax+1, &read_fds, NULL, NULL,NULL) == -1)
       {
          perror("server:select");
          exit(4);
       }
       //run through existing connection looking for data to read
       for(p = 0; p <= fdmax; p++) 
       {
           if (FD_ISSET(p, &read_fds)) 
           {                                  // we got one!!
               if (p == serv_sockfd) 
               {                              // handle new connections
                   counter = 0;
                   sin_size = sizeof(thr_addr);
                   new_fd = accept(serv_sockfd,(struct sockaddr *)&thr_addr,&sin_size);
                   if (new_fd == -1) 
                   {
                       perror("accept");
                   } 
                   else 
                   {
                       FD_SET(new_fd, &master); // add to master set
                       if (new_fd > fdmax) 
                       {            // keep track of the max
                           fdmax = new_fd;
                       }
                       /*if parent connection then print IPaddress and port info*/
                       if (counterServ < 1)
                       {
                          addlen = sizeof(myaddr);
                          int getsock_check = getsockname(new_fd,(struct sockaddr *)&myaddr,(socklen_t *)&addlen) ;
                          if (getsock_check== -1) {
                             perror("getsockname");
                             exit(1);
                           }
                           printf("\nAdmission office has ip address: %s", inet_ntoa(myaddr.sin_addr));
                           printf(" and TCP port number: %d for phase 1\n", (int) ntohs(myaddr.sin_port));
                       }
                       ++counterServ; //to keep track of number of connections
                       if (counterServ == 3) //upon start of phase2
                       {
                           printf("\nAdmission office has ip address: %s", inet_ntoa(myaddr.sin_addr));
                           printf(" and TCP port number: %d for phase 2\n", (int) ntohs(myaddr.sin_port));
                       }
                   }
               }
               else
               {  //recieve data from client
                  
                  /*recieve filename and open file*/
                  counter++;
                  if (counter == 1)
                  {
                       numbytes = recv(p, buf, MAXDATASIZE-1, 0);
                       buf[numbytes] = '\0';
                       depfile.open(buf);
                       strncpy(array,buf,4);
                       memset(buf, 0, sizeof(buf));
                  }
                  /*Check for other data recieved*/
                  if ((numbytes = recv(p, buf, MAXDATASIZE-1, 0)) <= 0)
                  {
                      // got error or connection closed by client
                      if (numbytes == 0)
                      {
                         /*Compare string to check if sender is student or Department*/
                         if (strncmp(array, "Stux", 3) == 0)
                         {
                            if (array) 
                               cout<<"Admission office recieved the application from "<< array<<endl;
                             
                         }
                         else
                         {
                            // connection closed
                           if(array) 
                               cout << "Recieved the program list from" <<array <<endl;
                             
                              /*If both department info recieved*/
                             if (counterServ == 2)
                             {    
                                  cout << "End of phase 1 for Admission office" <<endl;
                                  cout <<'\n'<<endl;
                             }  
                         }
                        close(p);
                        depfile.close();
                        FD_CLR(p, &master); // remove from master set
                        check_clientName(array);
                        memset(array, 0, sizeof(array)); 
                      } 
                      else 
                      {
                        close(p);
                        depfile.close();
                        FD_CLR(p, &master); // remove from master set
                        perror("recv");
                      }
                  }
                  else
                  {   //got some data from client
                      buf[numbytes] = '\0';
                      depfile << buf <<endl;
                      //write data to filena
                      memset(buf, 0, sizeof(buf));
                   }
               } //end handle data from client
           } //end got new incoming connection
       } //end looping through file descriptor
   }  //end for(;;)

   return 0;
}
