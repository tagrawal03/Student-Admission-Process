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
#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <cstring>
#include <sstream>

using namespace std;

#define SERVER_PORT "3985"
#define HOSTNAME "nunki.usc.edu"
#define DEPB_UDPPORT "21985"
//#define HOSTNAME "taruna-Lenovo-B50-70"


int sendall(int s, char *buf, int *len)
{
  int total = 0;
  // how many bytes we've sent
  int bytesleft = *len; // how many we have left to send
  int n;
  while(total < *len) {
  n = send(s, buf+total, bytesleft, 0);
  if (n == -1) { break; }
  total += n;
  bytesleft -= n;
   }
  *len = total; // return number actually sent here
  return n==-1?-1:0; // return -1 on failure, 0 on success
}

int main (void)
{
  struct addrinfo hints,hintsudp, *servinfo,*udpservinfo, *i, *p;
  struct sockaddr_in myaddr, myaddrUdp;
  struct sockaddr_storage their_addr;
  int addlen,addlenUdp,udpnumbytes;
  int sockfd,sockfd2, sockfd3;
  int status, rv;
  char data[100];
  socklen_t addr_len;
  ofstream depfile;  
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((status = getaddrinfo(HOSTNAME, SERVER_PORT, &hints, &servinfo)) != 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
  }
  sleep(1);  //delay of 1 sec before setting up Department B connection
  for (i = servinfo; i != NULL; i = i->ai_next)
  {
    /*Open 1st socket*/
     if ((sockfd = socket(i->ai_family, i->ai_socktype,
            i->ai_protocol)) == -1)
     {
        perror("Client: open socket");
        continue;
     }
     if (connect(sockfd, i->ai_addr, i->ai_addrlen) == -1) {
        close(sockfd);
        perror("client: connect");
        continue;
      }
      break;
  }
  if (i == NULL) {
      fprintf(stderr, "client: failed to connect\n");
      return 2;
   } 
  printf("\nDepartment B is now connected to Admission office\n");
  
  /*Send file name to Admission*/ 
  if(send(sockfd, "DepB.txt",8 , 0) < 0)
  {
      perror("sending of filename failed...");
  }

  /*Get IP address and port number of client*/ 
  addlen = sizeof(myaddr);
  int getsock_check = getsockname(sockfd,(struct sockaddr *)&myaddr,(socklen_t *)&addlen) ;
  if (getsock_check== -1) {
    perror("getsockname");
    exit(1);
  }

   printf("Department B has ip address: %s", inet_ntoa(myaddr.sin_addr));
   printf(" and TCP port number: %d for phase 1\n", (int) ntohs(myaddr.sin_port));
   
  /*Sending file*/
  ifstream myfile("departmentB.txt");
  string line;
  if (myfile.is_open())
  {
      while(getline(myfile, line))
      {
        char *cstr = new char[line.length() + 1];
        strcpy(cstr, line.c_str());
        int len = strlen(cstr); 
        /*Send each department information*/
        if(sendall(sockfd, cstr, &len) < 0)
        {
          perror("sending failed...");
          break;
        }
        usleep(10000); 
        stringstream fileline(line);
        string value;
        if(getline(fileline,value,'#'))
        {
           cout<<"Department B has sent " <<value<< " to the admission office"<< endl;
        }

        delete []cstr;
      }
      cout << "Updating the admission office is done for Department B" <<endl;
  }
  else
  {
    perror("unable to open file\n");
  }
  printf("End of phase 1 for Department B \n\n");
  close(sockfd);
      
  /*UDP socket*/
  
  memset(&hintsudp, 0, sizeof(hintsudp));
  hintsudp.ai_family = AF_INET;
  hintsudp.ai_socktype = SOCK_DGRAM;
  hintsudp.ai_flags = AI_PASSIVE;
  
  if ((rv = getaddrinfo(NULL, DEPB_UDPPORT, &hintsudp, &udpservinfo)) != 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
  }

  //sleep(1);
  for (p = udpservinfo; p != NULL; p = p->ai_next)
  {
    /*Open 1st socket*/
     if ((sockfd3 = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1)
     {
        perror("UDP: open socket");
        continue;
     }
     if (bind(sockfd3, p->ai_addr, p->ai_addrlen) == -1) {
        close(sockfd3);
        perror("UDP: bind");
        continue;
      }
      break;
  }
  if (p == NULL) {
      fprintf(stderr, ":UDP: failed to connect\n");
      return 2;
  }
  freeaddrinfo(udpservinfo);
  
  printf("DepartmentB: Waiting for admission result ...\n");      

  addr_len = sizeof(their_addr);
  depfile.open("DepartmentB_result.txt");
  
   while(1)
   {
      udpnumbytes = recvfrom(sockfd3, data, 99 , 0,(struct sockaddr *)&their_addr, &addr_len);
      if (udpnumbytes == -1){
         perror("recvfrom");
         exit(1);
      }
      else if (udpnumbytes > 1)
      {
         data[udpnumbytes] = '\0';
         cout <<data<< " has been admitted to department B" <<endl;
         /*Write result recievde to file*/
         depfile <<data <<endl;
      }
      else
      {
         break;
      }
      memset(data, 0, sizeof(data));
  }

  /*printing ip address*/ 
  addlenUdp = sizeof(myaddrUdp);
  int getsock_checknew = getsockname(sockfd3,(struct sockaddr *)&myaddrUdp,(socklen_t *)&addlenUdp) ;
  if (getsock_checknew== -1) {
     perror("getsockname");
     exit(1);
  }

  printf("Department B has ip address: %s", inet_ntoa(myaddr.sin_addr));
  printf(" and UDP port number: %d for phase 2\n", (int) ntohs(myaddrUdp.sin_port));

  cout << "End of phase2 for Department B"<<endl;
  depfile.close();
  close(sockfd3);

  return 0;
}
