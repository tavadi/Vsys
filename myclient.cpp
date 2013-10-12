// ############################################################################
// ############################################################################
//
//              VSYS - Uebung 1
//
//            Hörmann Daniel - if12b067
//           Thorstensen Benjamin - if12b066
//
//
//
// ############################################################################
// ############################################################################

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;
#define BUF 1024
//#define PORT 6543
void ManPage();

int main (int argc, char **argv) {
  int create_socket;
  char buffer[BUF];
  struct sockaddr_in address;
  int size;
  int port;

  if (strcmp(argv[1], "help")  == 0){
  ManPage();
  return EXIT_SUCCESS;
  }else if( argc != 3){
     cout << "./myclient [IP] [PORT]" << endl;
     cout << "Manpage:" << endl << "./myclient help" << endl;
     exit(EXIT_FAILURE);
  }else
  {
    port = atoi(argv[2]);
  }

  if ((create_socket = socket (AF_INET, SOCK_STREAM, 0)) == -1)
  {
     perror("Socket error");
     return EXIT_FAILURE;
  }
  
  memset(&address,0,sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons (port);    
  inet_aton (argv[1], &address.sin_addr);   // ist schon in networkbyte order

  if (connect ( create_socket, (struct sockaddr *) &address, sizeof (address)) == 0) //kein bind sondern connect
  {
      printf ("Connection with server (%s) established\n", inet_ntoa (address.sin_addr));
      size=recv(create_socket,buffer,BUF-1, 0);  //willkommensnachricht wird eingelesen
      if (size>0){
        buffer[size]= '\0';
        printf("%s",buffer);
      }
  }
  else
  {
    perror("Connect error - no server available");
    return EXIT_FAILURE;
  } 
  printf ("Send message: ");
  do {
     fgets (buffer, BUF, stdin);  //eingabe wird eingelesen
     send(create_socket, buffer, strlen (buffer), 0); //gesendet
      size=recv(create_socket,buffer,BUF-1, 0);  //status wird gesendet OK oder ERR
      if (size>0){
        buffer[size]= '\0';
        printf("%s",buffer);
      }
  }while (strcmp (buffer, "QUIT\n") != 0);
  close (create_socket);
  return EXIT_SUCCESS;
}



void ManPage(){

 cout << endl << endl << "NAME" << endl;
 cout << "          TWMailer - FH project c++ Server" << endl << endl ;
 cout << "          Daniel Hörmann & Benjamin Thorstensen" << endl << endl ;
 cout << endl << endl;
 cout << "SYNOPSIS" << endl;
 cout << "      SEND" << endl;
 cout << "      <Sender max. 8 Zeichen>" << endl;
 cout << "      <Empfänger max. 8 Zeichen>" << endl;
 cout << "      <Nachricht, beliebige Anzahl an Zeilen" << endl;
 cout << "      <.>" << endl;
 cout << "      LIST" << endl;
 cout << "      <Username max. 8 Zeichen>" << endl  ;
 cout << "      READ" << endl;
 cout << "      <Username max. 8 Zeichen>" << endl;
 cout << "      <Nachrichten-Nummer>" << endl ;
 cout << "      DEL" << endl;
 cout << "      <Username max. 8 Zeichen>" << endl;
 cout << "      <Nachrichten-Nummer>" << endl  ;
 cout << "DESCRIPTION" << endl;
 cout << "      Client-Server Anwendung in C/C++ zum Senden und Empfangen" << endl;
 cout << "      von internen Mails mithilfe von Socket Kommunikation." << endl << endl;
 cout <<"       SEND: Senden einer Nachricht vom Client zum Server." << endl;
 cout <<"       LIST: Auflisten der Nachrichten eines Users."<< endl;
 cout <<"             Es soll die Anzahl der Nachrichten und pro"<< endl;
 cout <<"             Nachricht die Betreff Zeile angezeigt werden." << endl;
 cout <<"       READ: Anzeigen einer bestimmten Nachricht fur einen User." << endl;
 cout <<"             Löschen einer Nachricht eines Users." << endl ;
 cout <<"       QUIT: Logout des Clients" << endl << endl; ;
 cout << "      Der Server antwortet bei korrekten Parameter mit:" << endl;
 cout << "      OK" << endl;
 cout << "      Der Server antwortet bei falschen Parameter mit:" << endl;
 cout << "      ERR" << endl;



}