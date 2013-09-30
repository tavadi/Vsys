// ############################################################################
// ############################################################################
//
//              VSYS - Uebung 1
//
//            Hörmann Daniel - if12b067
//           Thorstensen Benjamin - if12b066
//
//
/*
                  TODO
SEND OPTION:
Sender name muss noch gespeichert werden.
Max zeichenanzahl muss noch überprüft werden 
Formatierung <blub ...> 
Muss in Sende funktion gepackt werden              

*/
//
// ############################################################################
// ############################################################################
//




#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <time.h>  
#include <sstream>
using namespace std;
#define BUF 1024
#define PORT 6543

// ##################################################################################################################################################################
// Prototypen
int send(char * buffer, int status,string & mailname);
int list();

// ##################################################################################################################################################################

  
int main (void) {
  int create_socket, new_socket;
  socklen_t addrlen;
  char buffer[BUF];
  int size;
  struct sockaddr_in address, cliaddress; // speichert ip von client und server
  int status = 0;  //dient als info für den aktuellen zustand
  int stat; //fürs ordner erstellen
  FILE *fp;
  string mailname;
  



  create_socket = socket (AF_INET, SOCK_STREAM, 0); //Socket descriptor angelegt // af_inet = ipv4 // SOCK_Stream = TCP ... SOCK DATAGRAM = UPD

  memset(&address,0,sizeof(address)); // Wird mit 0 initalisiert damit kein müll drinnen steht
  address.sin_family = AF_INET;   //ipv4
  address.sin_addr.s_addr = htonl(INADDR_ANY);  // htonl(INADDR_ANY)
  address.sin_port = htons (PORT);  //Port wird angegeben (6543), Byte order wichtig

  if (bind ( create_socket, (struct sockaddr *) &address, sizeof (address)) != 0) { //Socket descriptor wird an den port gebunden
     perror("bind error");
     return EXIT_FAILURE;
  }
  listen (create_socket, 5);
  
  addrlen = sizeof (struct sockaddr_in);

    while (1) {
     printf("Waiting for connections...\n");
     new_socket = accept ( create_socket, (struct sockaddr *) &cliaddress, &addrlen );  //Socket wird zurückgegeben
     if (new_socket > 0)
     {
        printf ("Client connected from %s:%d...\n", inet_ntoa (cliaddress.sin_addr),ntohs(cliaddress.sin_port));
        strcpy(buffer,"Welcome to myserver, Please enter your command:\n");
        send(new_socket, buffer, strlen(buffer),0);
     }
     do {
        size = recv (new_socket, buffer, BUF-1, 0); //holt sich die nachricht
        if( size > 0)
        {
           buffer[size] = '\0';   //ende angeben sonst möglicher overflow

           ///****************************************
           cout << "status: {"<< status << "}-------------" <<endl;
           if (strcmp(buffer, "SEND\n") == 0  || status == 1 ||status == 2 ||status == 3||status == 4) {
           if (strcmp(buffer, "SEND\n") == 0){status =0;} 
          status=(send(buffer, status, mailname));}
           else if (strcmp(buffer, "LIST\n") == 0) {status = 4;
            list();}
           else if (strcmp(buffer, "READ\n") == 0) {status = 3;}
           else if (strcmp(buffer, "DEL\n") == 0)  {status = 4;}
           printf ("Message received: %s\n", buffer);   //schreibt bis zum \0
        }

        //**********************************************
        else if (size == 0)
        {
           printf("Client closed remote socket\n");
           break;
        }
        else
        {
           perror("recv error");
           return EXIT_FAILURE;
        }
     } while (strncmp (buffer, "quit", 4)  != 0);
     close (new_socket);
  }
  close (create_socket);
  return EXIT_SUCCESS;
}


const string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

int send(char * buffer,int status, string & mailname){
  FILE *fp;
  int stat;//fürs ordner erstellen

  
  if(status == 4)
  {
    if (strcmp(buffer, ".\n") != 0) {
    fp = fopen(mailname.c_str(), "ab+");
    fputs(buffer,fp); 
    fclose(fp);

    }else if(strcmp(buffer, ".\n") == 0){status = 0;chdir("..");}
   
  }
  else if(status == 3) // <Nachricht, beliebige Anzahl an Zeilen\n>
  {
    fp = fopen(mailname.c_str(), "ab+");
    fputs(buffer,fp); 
    fclose(fp);
    
    status = 4;    
  }
  else if(status == 2) //<Betreff max. 80 Zeichen>\n 
  {
    mailname = currentDateTime();
    mailname.append("_");
    mailname.append(buffer);
    mailname.erase(remove(mailname.begin(), mailname.end(), '\n'), mailname.end());
    mailname.append(".txt");
    fp = fopen(mailname.c_str(), "ab+");
    status = 3;              
  }
  else if(status == 1) // <Empfänger max. 8 Zeichen>\n
  {
    stat = mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    chdir(buffer); // in den ordner rein für messages.txt
    status = 2;
  }
  else if(status == 0)
  {
    status = 1;
  }
  return status;
}


int list (){



}