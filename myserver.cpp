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
List Option:
Ordner ausgabe  


*/
//
// ############################################################################
// ############################################################################
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
#include <fstream>
#include <errno.h>
#include <dirent.h>
#include <iomanip>
#define BUF 1024
//define PORT 6543
#define OK "OK\n"
#define ERR "ERR\n"
#define UNKNOWN "read the fking man page!\n"

using namespace std;
//debug
int CWD()
{
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd() error");
    }
    else
    {
        printf("CWD: %s\n", cwd);
    }
    return 1;
}

//*************************************************************************
//******************************Prototypen*********************************
int SaveMail(char * buffer, int status,string & mailname, string & Mail, int size, int new_socket);
int ListMail(char * buffer, int status, string & Mail, int size, int new_socket );
int ReadMail(char * buffer, int status, string & Mail, int size, int new_socket );
int DeleteMail(char * buffer, int status, string & Mail, int size, int new_socket );
void ManPage();
//*************************************************************************
//*************************************************************************

  
int main (int argc, char **argv) {
  int create_socket, new_socket;
  socklen_t addrlen;
  char buffer[BUF];
  int size;
  struct sockaddr_in address, cliaddress; // speichert ip von client und server
  int status = 0;  //dient als info für den aktuellen zustand
  int laststatus = 0;
  int stat; //fürs ordner erstellen
  FILE *fp;
  string mailname;
  string Mail;
  int port;

  ManPage();

 if( argc < 2 ){
     printf("Usage: %s ServerAdresse\n", argv[0]);
     exit(EXIT_FAILURE);
  }else
  {
    port = atoi(argv[1]);
  }
  create_socket = socket (AF_INET, SOCK_STREAM, 0); //Socket descriptor angelegt // af_inet = ipv4 // SOCK_Stream = TCP ... SOCK DATAGRAM = UPD

  memset(&address,0,sizeof(address)); // Wird mit 0 initalisiert damit kein müll drinnen steht
  address.sin_family = AF_INET;   //ipv4
  address.sin_addr.s_addr = htonl(INADDR_ANY);  // htonl(INADDR_ANY)
  address.sin_port = htons (port);  //Port wird angegeben (6543), Byte order wichtig

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
        size = recv (new_socket, buffer, BUF-1, 0); //holt sich die nachrich
        

       ///**************Arbeitsgebiet**************************

        if (status == -1) // FEHLER
        {
          status = laststatus;  //Status wird zurückgesetzt
        }
        else{             //Kein Fehler
          laststatus = status;  //Status wird gespeichert
        }
        if( size > 0)
        {

           buffer[size] = '\0';   //Ende angeben sonst möglicher Overflow
//###########################    SEND   #####################################################
           if (strcmp(buffer, "SEND\n") == 0){
              //Funktionsaufruf:
             if((status=(SaveMail(buffer, status, mailname, Mail, size,  new_socket))) == -1 ){
               send(new_socket, ERR, strlen(ERR),0); // FEHLER = return -1
             }else{
              send(new_socket, OK, strlen(OK),0); //OK 
             }  

           }
//###########################    LIST   #####################################################
           else if (strcmp(buffer, "LIST\n")  == 0) {
            
              if((status=(ListMail(buffer, status, Mail, size,  new_socket))) == -1 ){
                send(new_socket, ERR, strlen(ERR),0); // FEHLER = return -1
              }
              else{
                send(new_socket, OK, strlen(OK),0); // OK
              }
          }
//###########################    READ   #####################################################
          else if(strcmp(buffer, "READ\n" )  == 0 )
          {
            cout << "status: {"<< status << "}-------------" << "laststatus : {" << laststatus << "}" <<endl;
            if((status=(ReadMail(buffer, status, Mail, size, new_socket ))) == -1 ){
                send(new_socket, ERR, strlen(ERR),0); // FEHLER = return -1
              }else{
                send(new_socket, OK, strlen(OK),0); //OK
              }
          }//###########################    DELETE   #####################################################
          else if(strcmp(buffer, "DEL\n" )  == 0 )
          {
            cout << "status: {"<< status << "}-------------" << "laststatus : {" << laststatus << "}" <<endl;
            if((status=(DeleteMail(buffer, status, Mail, size, new_socket ))) == -1 ){
                send(new_socket, ERR, strlen(ERR),0); // FEHLER = return -1
              }else{
                send(new_socket, OK, strlen(OK),0); //OK
              }
          }
          else{
            send(new_socket, ERR, strlen(ERR),0); // OK
          }


//-----------------------------------------------STOP--------------------------
           printf ("Message received: %s\n", buffer);   //schreibt bis zum \0
        }

        //**********************************************
        else if (size == 0){
           printf("Client closed remote socket\n");
           break;
        }
        else{
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




//*************************************************************************
//*************************************************************************
//*************************************************************************


int ListMail (char * buffer, int status, string & Mail,int size, int new_socket){
    //Directory Pointer
    FILE * fp;
    DIR * dp;
    string file_name;
    int mailcount = 0;
    struct dirent *dir;
    stringstream ss;

  send(new_socket, "OK1\n", strlen("OK1\n"),0);
  size = recv (new_socket, buffer, BUF-1, 0);
  buffer[size] = '\0'; 

    if ((dp=opendir(buffer)) == NULL)
   { return status;

   } 
    while((dir=readdir(dp)) != NULL)
    {
      if(     strcmp( dir->d_name, "." ) == 0 || 
      strcmp( dir->d_name, ".." ) == 0 ||
      strcmp( dir->d_name, ".DS_Store" ) == 0 )
      {continue;}
      else{ 
       mailcount++;
      }
    }
    ss << mailcount;
    Mail = "Sie haben " + ss.str() + " Nachrichten.\n";
    ss.str("");
    mailcount = 1;
    rewinddir(dp);
    while((dir=readdir(dp)) != NULL)
    {
      if(     strcmp( dir->d_name, "." ) == 0 || 
      strcmp( dir->d_name, ".." ) == 0 ||
      strcmp( dir->d_name, ".DS_Store" ) == 0 )
      {continue;}
      else{  
        file_name = dir->d_name;  
        ss.str("");   //stringstream clear
        ss << mailcount;  //speichert Nummer der Mail
        stringstream ss2(file_name);
        getline(ss2,file_name, '_');
        cout << "file_name : " <<  file_name << endl;
        cout << "ss2 : "<< ss2 << endl;
        Mail.append("[" + ss.str() + "] " + file_name + "\n");
        mailcount++;
      }  
    }
    send(new_socket, Mail.c_str(), strlen(Mail.c_str()),0);    //schicke mail zum client
    status = 0;  //programm fertig
    Mail.erase();
    closedir(dp);
    status = 8;
    chdir("..");
    return status;

}
//*************************************************************************
//*************************************************************************
//*************************************************************************

int ReadMail (char * buffer, int status, string & Mail,int size, int new_socket)
{
  
  DIR * dp;
  struct dirent *dir;
  FILE *datei;
  int i = 0;
  send(new_socket, "OK1\n", strlen("OK1\n"),0);
  size = recv (new_socket, buffer, BUF-1, 0);
  buffer[size] = '\0';  
  cout <<"buffer: " << buffer  << endl;
  if ((dp=opendir(buffer)) == NULL)
  {
    return -1;
  }
  //wechselt in den Mailordner
  chdir(buffer);
  send(new_socket, "OK1\n", strlen("OK1\n"),0);
  size = recv (new_socket, buffer, BUF-1, 0);
  buffer[size] = '\0';  
  while((dir=readdir(dp)) != NULL && i < (atoi(buffer)-1) )
  {
    if(     strcmp( dir->d_name, "." ) == 0 || 
    strcmp( dir->d_name, ".." ) == 0 ||
    strcmp( dir->d_name, ".DS_Store" ) == 0 )
    {
     continue;
    }else{ 
      i++;
    }
  }
//Liest die Datei aus.
ifstream file(dir->d_name);
string str;
string file_contents;
while (std::getline(file, str))
{
  file_contents += str;
  file_contents.push_back('\n');
} 
//schickt den Mail-Inhalt
send(new_socket, file_contents.c_str(), strlen(file_contents.c_str()),0);

}


//*************************************************************************
//*************************************************************************
//*************************************************************************






int SaveMail(char * buffer, int status,string & mailname, string & Mail, int size, int new_socket)
{
  FILE *fp;
  int stat;//Zum Ordner erstellen

  send(new_socket, "OK1\n", strlen("OK1\n"),0);
  size = recv (new_socket, buffer, BUF-1, 0);
  buffer[size] = '\0';  
  Mail.append(buffer);//<Sender max. 8 Zeichen>\n

  send(new_socket, "OK1\n", strlen("OK1\n"),0);
  size = recv (new_socket, buffer, BUF-1, 0);
  buffer[size] = '\0'; 


    if((strlen(buffer)) <= 9){
      stat = mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      if (chdir(buffer) != 0){  // in den rdner rein um auf Mail zuzugreifen
        perror("chdir() failed");
      } 
      Mail.append(buffer);// <Empfänger max. 8 Zeichen>\n
      send(new_socket, "OK1\n", strlen("OK1\n"),0);
      size = recv (new_socket, buffer, BUF-1, 0);
      buffer[size] = '\0'; 
      if((strlen(buffer)) <= 81){
        mailname = buffer;
        mailname.append("_");
        mailname.append(currentDateTime());
        mailname.erase(remove(mailname.begin(), mailname.end(), '\n'), mailname.end());
        mailname.append(".txt");
        Mail.append(buffer);
        fp = fopen(mailname.c_str(), "ab+");  //File erstellen
        /*    if (fp == NULL){  //fehlerabfrage fehlt noch
        printf ("Error opening file unexist.ent: %s\n",strerror(errno));
        return EXIT_FAILURE;
        }
        else{*/
        fclose(fp);
        status = 4; 
      }else{
       return -1;
      }    
      send(new_socket, "OK1\n", strlen("OK1\n"),0);
      size = recv (new_socket, buffer, BUF-1, 0);
      buffer[size] = '\0';          
      // <Nachricht, beliebige Anzahl an Zeilen\n>
      Mail.append(buffer);

      while(strcmp(buffer, ".\n") != 0) {
        send(new_socket, "OK1\n", strlen("OK1\n"),0);
        size = recv (new_socket, buffer, BUF-1, 0);
        buffer[size] = '\0';    
        Mail.append(buffer);
      }
      fp = fopen(mailname.c_str(), "ab+");
      if (fp == NULL){
        printf ("Error opening file unexist.ent: %s\n",strerror(errno));
        return EXIT_FAILURE;
      }else{
        fputs(Mail.c_str(),fp); 
        fclose(fp);
        status = 0;
        if (chdir("..") != 0){  //zurück in den main ordner
          perror("chdir() to /tmp failed");
          return EXIT_FAILURE;
        } 
      Mail.erase();
      return status;
      }
   }
}

//*************************************************************************
//*************************************************************************
//*************************************************************************




int DeleteMail(char * buffer, int status, string & Mail,int size, int new_socket)
{
  DIR * dp;
  struct dirent *dir;
  FILE *datei;
  int i = 0;
  send(new_socket, "OK1\n", strlen("OK1\n"),0);
  size = recv (new_socket, buffer, BUF-1, 0);
  buffer[size] = '\0';  
  cout <<"buffer: " << buffer  << endl;
  if ((dp=opendir(buffer)) == NULL)
  {
    return -1;
  }
  //wechselt in den Mailordner
  chdir(buffer);
  send(new_socket, "OK1\n", strlen("OK1\n"),0);
  size = recv (new_socket, buffer, BUF-1, 0);
  buffer[size] = '\0';  
  while((dir=readdir(dp)) != NULL && i < (atoi(buffer)-1) )
  {
    if(     strcmp( dir->d_name, "." ) == 0 || 
    strcmp( dir->d_name, ".." ) == 0 ||
    strcmp( dir->d_name, ".DS_Store" ) == 0 )
    {
     continue;
    }else{ 
      i++;
    }
  }
  //Löscht die Mail
  if( remove( dir->d_name ) != 0 )
  {
      perror( "Error deleting file" );
    send(new_socket, "DELOK\n", strlen("DELOK\n"),0);
  }
  else
  {
    cout << "deleted file :" <<dir->d_name  << endl;
    puts( "File successfully deleted" );
    send(new_socket, "DELOK\n", strlen("DELOK\n"),0);
  }
   
  //schickt den Mail-Inhalt
 return 0;
}



void ManPage(){

 cout << endl << endl << "NAME" << endl;
 cout << "          TWMailer - FH project c++ Server" << endl << endl << endl;
 cout << endl << endl;
 cout << "SYNOPSIS" << endl;
 cout <<"       SEND: Senden einer Nachricht vom Client zum Server." << endl;
 cout<< "       LIST: Auflisten der Nachrichten eines Users. "<< endl;
 cout<< "             Es soll die Anzahl der Nachrichten und pro  "<< endl;
 cout<< "             Nachricht die Betreff Zeile angezeigt werden." << endl;
 cout<< "       READ: Anzeigen einer bestimmten Nachricht fur einen User." << endl;
 cout <<"             Löschen einer Nachricht eines Users." << endl << endl << endl;
 cout <<"       QUIT: Logout des Clients" << endl << endl << endl;
 cout << "DESCRIPTION" << endl;
 cout << "      Lorem ipsum dolor sit amet, consetetur sadipscing elitr" << endl;
 cout << "      Lorem ipsum dolor sit amet, consetetur sadipscing elitr" << endl;
 cout << "      Lorem ipsum dolor sit amet, consetetur sadipscing elitr" << endl;
 cout << "      Lorem ipsum dolor sit amet, consetetur sadipscing elitr" << endl << endl << endl;
 cout << "OPTIONS" << endl;
 cout << "      Lorem ipsum dolor sit amet, consetetur sadipscing elitr" << endl;
 cout << "      Lorem ipsum dolor sit amet, consetetur sadipscing elitr" << endl;
 cout << "      Lorem ipsum dolor sit amet, consetetur sadipscing elitr" << endl;
 cout << "      Lorem ipsum dolor sit amet, consetetur sadipscing elitr" << endl;

}