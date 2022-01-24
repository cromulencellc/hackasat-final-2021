/*    This file is distributed with 42,                               */
/*    the (mostly harmless) spacecraft dynamics simulation            */
/*    created by Eric Stoneking of NASA Goddard Space Flight Center   */

/*    Copyright 2010 United States Government                         */
/*    as represented by the Administrator                             */
/*    of the National Aeronautics and Space Administration.           */

/*    No copyright is claimed in the United States                    */
/*    under Title 17, U.S. Code.                                      */

/*    All Other Rights Reserved.                                      */


#include "iokit.h"

/* #ifdef __cplusplus
** namespace Kit {
** #endif
*/

/**********************************************************************/
void ByteSwapDouble(double *A)
{
      char fwd[8],bak[8];
      long i;

      memcpy(fwd,A,sizeof(double));
      for(i=0;i<8;i++) bak[i] = fwd[7-i];
      memcpy(A,bak,sizeof(double));
}

/**********************************************************************/
SOCKET InitSocketClient(const char *IpAddr, int Port,int AllowBlocking)
{
   SOCKET sockfd;
   int flags;
   struct sockaddr_in Server;
   // struct hostent *Host;
   int DisableNagle = 1;

   sockfd = socket(AF_INET,SOCK_STREAM,0);
   if (sockfd < 0) {
      printf("Error opening client socket.\n");
      sockfd = -1;
      return(sockfd);
   }
   memset((char *) &Server,0,sizeof(Server));
   // Convert IPv4 and IPv6 addresses from text to binary form
   if(inet_pton(AF_INET, IpAddr, &Server.sin_addr.s_addr)<=0) 
   {
      printf("\nInvalid address/ Address not supported \n");
      return -1;
   }
   
   Server.sin_family = AF_INET;
   // inet_pton(IpAddr, &Server.sin_addr.s_addr);
   // memcpy((char *)&Server.sin_addr.s_addr,(char *)Host->h_addr_list[0],
   //    Host->h_length);
   Server.sin_port = htons(Port);
   printf("Client connecting to server %s:%i\n", IpAddr, Port);
   if (connect(sockfd,(struct sockaddr *) &Server,sizeof(Server)) < 0) {
      printf("Error connecting client socket: %s.\n",strerror(errno));
      return -1;
   }
   printf("Client side of socket established.\n");

   /* Keep read() from waiting for message to come */
   if (!AllowBlocking) {
      flags = fcntl(sockfd, F_GETFL, 0);
      fcntl(sockfd,F_SETFL, flags|O_NONBLOCK);
   }

   /* Allow TCP to send small packets (look up Nagle's algorithm) */
   /* Depending on your message sizes, this may or may not improve performance */
   setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY,&DisableNagle,sizeof(DisableNagle));

   return(sockfd);

}

/* #ifdef __cplusplus
** }
** #endif
*/
