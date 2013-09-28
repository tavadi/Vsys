// ############################################################################
// ############################################################################
//
//              VSYS - Uebung 1
//
//            Hörmann Daniel - if12b067
//           Thorstensen Benjamin - if12b066
//
//
//lsof -i
//zeigt die ports an oderso
//
//
//Fehler abfragen noch anschauen
//Struktur der 
//
// ############################################################################
// ############################################################################
//
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
#define PORT 6543

int main (int argc, char **argv) {
  int create_socket;
  char buffer[BUF];
  struct sockaddr_in address;
  int size;

  if( argc < 2 ){
     printf("Usage: %s ServerAdresse\n", argv[0]);
     exit(EXIT_FAILURE);
  }

  if ((create_socket = socket (AF_INET, SOCK_STREAM, 0)) == -1)
  {
     perror("Socket error");
     return EXIT_FAILURE;
  }
  
  memset(&address,0,sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons (PORT);    
  inet_aton (argv[1], &address.sin_addr);   // ist schon in networkbyte order

  if (connect ( create_socket, (struct sockaddr *) &address, sizeof (address)) == 0) //kein bind sondern connect
  {
      printf ("Connection with server (%s) established\n", inet_ntoa (address.sin_addr));
      size=recv(create_socket,buffer,BUF-1, 0);  //willkommensnachricht wird eingelesen
      if (size>0)
      {
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
  } 
  while (strcmp (buffer, "quit\n") != 0);
  close (create_socket);
  return EXIT_SUCCESS;
}