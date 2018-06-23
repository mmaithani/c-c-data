#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h> 


int sockfd; 
char username[50];
void obtenerLista(char* buffer){ 
   int n;
   n =read(sockfd,buffer,256);
    if (n<=0){
      printf("failed\n");
      exit(1);
   }
   char buf[PATH_MAX+1];
   realpath("musicaCliente.txt",buf);

   FILE* fp = fopen(buf,"w");
   int cuantos = atoi(buffer); 
   int i = 0;
   fprintf(fp,"%d;", cuantos);
   strcpy(buffer,"empezar");
   write(sockfd,buffer,256);
   memset(buffer,0,strlen(buffer));     
   int noEof=1;  
   int k = 0;
   while (i < cuantos && noEof){

        read(sockfd, buffer, 256);
        if (strcmp(buffer,"stop")==0){
            noEof=0;
            memset(buffer,0,strlen(buffer));
         
        }else{
          fprintf(fp,"%s;", buffer ); 
          write(sockfd,buffer,strlen(buffer));//3
          memset(buffer,0,strlen(buffer)); 
          read(sockfd, buffer, 256);
          fprintf(fp,"%s;", buffer );
          write(sockfd,buffer,strlen(buffer));//4
          memset(buffer,0,strlen(buffer));
          read(sockfd, buffer, 256);
          fprintf(fp,"%s;", buffer );
          n = write(sockfd,buffer,strlen(buffer));//4
          memset(buffer,0,strlen(buffer));
          k++; 

        }
        
    }
    fclose(fp);
   
}





void obtenerCancion(){

   int seborro= unlink ("tempSong");
    if (seborro ==0){
      sleep(3);
    }else{
      printf("fallo en borrarse\n");
    }
   FILE *fp; 

   fp = fopen("tempSong", "ab"); 
   rewind(fp);
   int bytesReceived = 0;
   char recvBuff[256];


   printf("Recibiendo canción.\n");
    /* Create file where data will be stored */
    
   if(fp == NULL)
   {
      printf("Error abriendo archivo.\n");
      exit(1); 
   } 
   /* Receive data in chunks of 256 bytes */
   while((bytesReceived = read(sockfd, recvBuff, 256)) > 0)
   {
        
      fwrite(recvBuff, 1,bytesReceived,fp);
      if(bytesReceived < 256) {
         break;
      }

   }
   if(bytesReceived < 0)
   {
      printf("Error de lectura.\n");
   }
 
}
 
void refresh(char* buffer){
    strcpy(buffer, "1");
    write(sockfd, buffer, strlen(buffer));

    memset(buffer,0,strlen(buffer));
    obtenerLista(buffer);

    memset(buffer,0,strlen(buffer));
    
    strcpy(buffer,"4");
    write(sockfd,buffer,strlen(buffer));

    memset(buffer,0,strlen(buffer));
    read(sockfd,buffer,256);
    memset(buffer,0,strlen(buffer));
    strcpy(buffer,username);
    write(sockfd,buffer,strlen(buffer));
    memset(buffer,0,strlen(buffer));


    FILE* fpCliente = fopen(username, "w");
    read(sockfd,buffer,256);
    int contador =1;
    while (strcmp(buffer,"stop")){
        fprintf(fpCliente, "%s", buffer);
        memset(buffer,0,strlen(buffer));
        strcpy(buffer,"noting");
        write(sockfd,buffer,strlen(buffer));            
        memset(buffer,0,strlen(buffer));
        read(sockfd,buffer,256);
        contador++;
    }  
    fclose(fpCliente);
    memset(buffer,0,strlen(buffer));
    gtk_player(sockfd, username);
}
 

int main(int argc, char *argv[]) {
   int portno, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;
   
   char buffer[256];
   
   if (argc < 3) { 
      fprintf(stderr,"usage %s hostname port\n", argv[0]);
      exit(0);
   }
  
   portno = atoi(argv[2]);
   
   /* Create a socket point */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("Error abriendo socket.");
      exit(1);
   } 
  
   server = gethostbyname(argv[1]);
   
   if (server == NULL) {
      fprintf(stderr,"Error, host inexistente.\n");
      exit(0);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);
   
   /* Now connect to the server */
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("Error de conexión.");
      exit(1);
   }
   printf("Conexión establecida.\n");
   
   /* Now ask for a message from the user, this message
      * will be read by server
   */
  
   getUser();
   

   strcpy(buffer,username);
   n = write(sockfd, buffer, strlen(buffer));
   
   if (n < 0) { 
      perror("Error escribiendo al socket.");
      exit(1);
   }
   
   /* Now read server response */
   bzero(buffer,256);
   n = read(sockfd, buffer, 255);


   memset(buffer,0,strlen(buffer)); 
   
 
   int keepGoing = 1;
   while(keepGoing) {
         
        //bzero(buffer,256);
        //fgets(buffer, 256, stdin);
        strcpy(buffer,"1\n");
        refresh(buffer);

        if(!strcmp(buffer, "1\n")) {
            refresh(buffer);
        }
    }

   if (n < 0) {
      perror("Error leyendo del socket.");
      exit(1);
   }
   return 0;
}