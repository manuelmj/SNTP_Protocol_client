
/*
MANUNEL MANJARRES RIVERA. 

sntpv4 for ipv4 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define _IP_ "162.159.200.1"
#define _PORT_ 123
#define _Error_ -1
#define NTP_TIMESTAMP 2208988800ull 
#define DELTA(x)   ((x)-(NTP_TIMESTAMP))
typedef struct sockaddr_in sockaddr_in;

typedef struct campo{
  char Mode : 3;  /*modo del protocolo*/
  char Vn : 3;   /*numero de la version ipv4-ipv6-iso*/
  char Li : 2;  /*indicador de salto*/
} Campo_LiVnMode;

typedef union numeros{
  Campo_LiVnMode LiVnMode;
  unsigned char LiVnMode_Complete;
} sync_version_mode;

typedef struct protocol_stnp
{

  unsigned char li_vn_mode; // 8bits bits. li, vn, and mode.
 
  unsigned char stratum;     //  Egiht bits. Stratum level of the local clock.
  unsigned char poll;       // Eight bits. Maximum interval between successive messages.
  unsigned char precision; // Eight bits. Precision of the local clock.

  unsigned int root_Delay;            // 32 bits. Total round trip delay time.
  unsigned int root_Dispersion;      // 32 bits. Max error aloud from primary clock source.
  unsigned int Reference_Identifier;// 32 bits. Reference clock identifier.
  
  /*MARCA DE TIEMPO DE REFERENCIA (ultima vez que fue establecida o corregida  la hora del sistema)*/
  unsigned int Reference_Timestamp_Seconds;       // 32 bits. Reference time-stamp seconds.
  unsigned int Reference_Timestamp_Milisenconds; // 32 bits. Reference time-stamp fraction of a second.
  
  /*SOLICITUD DE TIEMPO ENVIADA POR EL CLIENTE*/
  unsigned int Originate_Timestamp_Seconds;      // 32 bits. Originate time-stamp seconds.
  unsigned int Originate_Timestamp_Miliseconds; // 32 bits. Originate time-stamp fraction of a second.
  
  /*SOLICITUD DE TIEMPO RECIVIDA POR EL SERVIDOR*/
  unsigned int Recived_Timestamp_Seconds; 
  unsigned int Recived_Timestamp_Miliseconds; 
 
  //*TIMEPO DE RESPUESTA ENVIADA POR EL SERVIDOR*/
  unsigned int Transmit_Timestamp_Seconds;  
  unsigned int Trasmit_Timestamp_Miliseconds;      

} sntp_protocol; // Total: 384 bits or 48 bytes.

void failed_protocol(int *const socket);

int main(void){
 
  sync_version_mode livnmode={3,3,0};/*mode(3)=client,Vn(3)=version 3, Li(0)=no warning*/
  printf("%d-%d\r\n", 0x1b, livnmode.LiVnMode_Complete);
  
  /*EN MODO UNIDIFUSION (unicast mode)*/
  sntp_protocol Protocol ={0};
  Protocol.li_vn_mode=livnmode.LiVnMode_Complete;

  int socket_ID = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  sockaddr_in server;
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = inet_addr(_IP_);
   server.sin_port = htons(_PORT_);

  if (connect(socket_ID, (struct sockaddr *)&server, sizeof(server)) == _Error_)
    failed_protocol(&socket_ID);
  
  if (send(socket_ID, (char *)&Protocol,sizeof(sntp_protocol), 0) == _Error_)
    failed_protocol(&socket_ID);
  
  if (recv(socket_ID, (char *)&Protocol, sizeof(sntp_protocol), 0) == _Error_)
    failed_protocol(&socket_ID);
  

  Protocol.Transmit_Timestamp_Seconds = ntohl(Protocol.Transmit_Timestamp_Seconds);
  time_t tiempo_segundos = (time_t)DELTA(Protocol.Transmit_Timestamp_Seconds);
  printf("Time: %s", ctime((const time_t *)&tiempo_segundos));

  close(socket_ID);
  printf("fin\r\n");
  return (0);
}

void failed_protocol(int *const socket)
{
  printf("fallo del protocolo");
  close(*socket);
  exit(EXIT_FAILURE);
}
