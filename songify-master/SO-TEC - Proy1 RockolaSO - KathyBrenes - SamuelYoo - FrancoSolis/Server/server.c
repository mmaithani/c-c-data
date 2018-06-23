#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>

#include <string.h> 

void obtenerTexto(char* username, int sock){
   
   FILE *fp; 
   FILE *logFile = fopen("log" ,"a");
   char namePath[50];
   strcpy(namePath, "playlists from users/");
   strcat(namePath, username);
   fp = fopen(namePath, "wb"); 
   rewind(fp);
   int bytesReceived = 0;
   char recvBuff[256];

    /* Create file where data will be stored */
   if(fp == NULL)
   {  
      
      printf("Error abriendo archivo.\n");
      fprintf(logFile, "Error abriendo archivo.\n");
      exit(1); 
   } 
   /* Receive data in chunks of 256 bytes */
   while((bytesReceived = read(sock, recvBuff, 256)) > 0)
   {
      printf("Bytes recibidos: %d\n",bytesReceived);    
      fwrite(recvBuff, 1,bytesReceived,fp);
      if(bytesReceived < 256) {
         break;
      }

   }
   fprintf(logFile, "Texto recibido de %s.\n", username);
   if(bytesReceived < 0)
   {
      printf("Error de lectura.\n");
      fprintf(logFile, "Error de lectura.\n");
   }
   fclose(fp);
   fclose(logFile);
 
}

void enviarCancion(int sock,char* cual){
    /* Open the file that we wish to transfer */
   char *src;
   FILE *logFile = fopen("log","a");
   src = calloc(1000,sizeof(char));
   sprintf(src,"/home/franco/Documents/SO/Proyecto 1/Final/Server/source/%s",cual);
   printf(logFile, "Canción siendo enviada.\n");
   fprintf(logFile, "Canción siendo enviada.\n");
   FILE *fp = fopen(src,"rb");

    if(fp==NULL)
    { 
        printf("Error abriendo archivo.\n");
        fprintf(logFile, "Error abriendo archivo.\n");
    }   
    /* Read data from file and send it */
    while(1)
    {
        /* First read file in chunks of 256 bytes */

        unsigned char buff[256]={0};
        int nread = fread(buff,1,256,fp);
        printf("Bytes leidos: %d \n", nread);        

        /* If read was success, send data. */
        if(nread > 0)
        {
            printf("Enviando\n");
            write(sock, buff, nread);
        }

        /*
         * There is something tricky going on with read .. 
         * Either there was error, or we reached end of file.
         */
        if (nread < 256)
        {
            if (feof(fp))
                printf("Fin del archivo\n");
                fprintf(logFile, "Archivo enviado.\n");
            if (ferror(fp))
                printf("Error de lectura\n");
            break;
            
        }
    }
    fprintf(logFile, "Canción recibida.\n");
    fclose(logFile);
    fclose(fp);
}



char* scanner(FILE* fp){
    int c;
    int i = 1;
    char* palabra;
    palabra =(char*)calloc(i,1+sizeof(char)); 

 
    while((c = getc(fp))!=EOF){ 
         
        if (isspace(c)){
            palabra[i-1]= c;
            i++;
            palabra =(char*)realloc(palabra, i*sizeof(char));
            if (!isspace(c=getc(fp))||!((c=getc(fp))==':')|| !((c=getc(fp))==';')||!((c=getc(fp))=='=')){
                palabra[i-1]= c;
                i++;
                palabra =(char*)realloc(palabra, i*sizeof(char));

            }else{
                continue;    
            }
            
        }else if (c==':' || c==';'||c =='='){   
            
            return palabra;   
        }else if (c == '-'){
            if ((c=getc(fp))=='>')
                return palabra;
        }else if  (c == '\n'){
            continue;
        }else{
            palabra[i-1]= c;
            i++;
            palabra =(char*)realloc(palabra, i*sizeof(char));
        } 
    } 
    return "EOF"; 
}

char* scannerPlaylist(FILE* fp,char* tipo){
    int c;
    int i = 1;
    char* palabra;
    char* queEs;
    palabra =(char*)calloc(i,1+sizeof(char)); 
    queEs= calloc(2,sizeof(char));
 
    while((c = getc(fp))!=EOF){ 
         
        if (isspace(c)){
            palabra[i-1]= c;
            i++;
            palabra =(char*)realloc(palabra, i*sizeof(char));
            if (!isspace(c=getc(fp))||!((c=getc(fp))==':')|| !((c=getc(fp))==';')||!((c=getc(fp))=='=')){
                palabra[i-1]= c;
                i++;
                palabra =(char*)realloc(palabra, i*sizeof(char));

            }else{
                continue;    
            }
            
        }else if (c==':' || c==';'||c =='='){   
            strcpy(tipo,"cancion");
            return palabra;   
        }else if (c == '-'){
            if ((c=getc(fp))=='>'){
                strcpy(tipo,"playlist");
                return palabra; 
            }
                
        }else if  (c == '\n'){
            continue;
        }else{
            palabra[i-1]= c;
            i++;
            palabra =(char*)realloc(palabra, i*sizeof(char));
        } 
    } 
    strcpy(tipo,"EOF");
    return "EOF"; 
}


void guardarPlaylist(char* nombrePlaylist, char canciones[50][1024],char artista[50][1024],char mp3[50][1024],int indice,char* username){
   char* str;
   str =calloc(1024,sizeof(char));
   int i ;
   FILE *fp;
   FILE *fpTemp;
   sprintf(str,"/home/franco/Documents/SO/Proyecto 1/Final/Server/playlists from users/%s",username);
   fp = fopen(str,"ab+");
   if (fp ==NULL){
    printf("does not exists in guardarPlaylist\n");
   }

   fseek(fp,0,SEEK_END);
   fprintf(fp, "%s->",nombrePlaylist);
   for (i = 0;i <indice;i++){
        artista[i][strlen(artista[i])-1] = 0;
      
      fprintf(fp, "%s:%s:%s;",canciones[i],artista[i],mp3[i]);
   }
   
   fclose(fp);

}

void desplegarPlaylist(int socket,char* playlist,char* username){
    char url[1000];
    char c[512];
    char tipo[30];
    char message[512];
    sprintf(url,"/home/franco/Documents/SO/Proyecto 1/Final/Server/playlists from users/%s",username);
    FILE* fp = fopen(url,"r");
    strcpy(c,scannerPlaylist(fp,tipo));

    while (strcmp(tipo,"EOF")){
        if(!strcmp(tipo,"playlist")){
            if (!strcmp(c,playlist)){
                strcpy(c,scannerPlaylist(fp,tipo));
                strcpy(message,c);
                write(socket,message,strlen(message));
                memset(message,0,strlen(message));
                read(socket,message,256);
                memset(message,0,strlen(message));
                strcpy(c,scannerPlaylist(fp,tipo)); 
                strcpy(message,c);
                while (strcmp(tipo,"playlist") && strcmp(tipo,"EOF")){

                    int i;
                    for (i = 0; i < 3; i++){
                      write(socket,message,strlen(message));
                      memset(message,0,strlen(message));
                      read(socket,message,256);
                      memset(message,0,strlen(message));
                      strcpy(c,scannerPlaylist(fp,tipo));
                      strcpy(message,c);
                    }
                }

                strcpy(message,"termino");
                write(socket,message,strlen(message));
            }
        }
        strcpy(c,scannerPlaylist(fp,tipo));
    }

}

void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    FILE *logFile = fopen("log", "a");
    int sock = *(int*)socket_desc;
    int read_size;
    char message[256] , client_message[2000]; 
    int keepGoing = 1; 
    char* username;
    int hola=0;
    username = calloc(1024,sizeof(char));
    read_size = read(sock,message,256);
    strcpy(username,message);

    printf("Usuario %s conectado.\n", username);
    fprintf(logFile, "Usuario %s conectado.\n", username);

    write(sock,message,strlen(message));  
    memset(message,0,strlen(message));


    read_size = read(sock, message, 256);
    
    

    if (read_size<=0){
      exit(1);
    }
    while(keepGoing) {
        
        if(!strcmp(message, "1")) {

            FILE* fp = fopen("musica.txt","r");
            char* c;
            int seguir = 1;
            int i;
            char* palabra;
            c = calloc(1024,sizeof(char*));
            if (fp == NULL){
               printf("file does not exist.\n");
               exit(1);
            }
            c =scanner(fp);
            strcpy(message , c);
            write(sock, message, 256);
            memset(message,0,strlen(message));
            read(sock, message, 256);
            
            c =scanner(fp);
            if (strcmp(message,"empezar")==0){
               int k = 0;
               while (strcmp(c,"EOF")){
                  strcpy(message, c);
                  write(sock, message, strlen(message));
                  memset(message,0,strlen(message));
                  read(sock, message, 256);
                  c =scanner(fp);
                  strcpy(message, c);
                  write(sock, message, strlen(message));
                  memset(message,0,strlen(message));

                  read (sock, message, 256);
                  c =scanner(fp);
                  strcpy(message, c);
                   write(sock, message, strlen(message));
                  memset(message,0,strlen(message));

                  read (sock, message, 256);
                 
                  k++;
                  
                  c =scanner(fp);

                 }

             }

            strcpy(message, "stop");
            write(sock, message, strlen(message));
            memset(message,0,strlen(message));
         
            
      }else if (!strcmp(message,"2")){
        memset(message,0,strlen(message));
         strcpy(message,"cual cancion desesa?");
         write(sock, message, strlen(message));
         memset(message,0,strlen(message));
         read(sock,message,256);
         enviarCancion(sock,message);
         memset(message,0,strlen(message));
  


      }else if (!strcmp(message,"3")){
            char canciones[50][1024];
            char artista[50][1024];
            char mp3[50][1024];
            int index= 0;
            strcpy(message,"nombre del playlist\n");
            write(sock,message,strlen(message));
            memset(message,0,strlen(message)); 
            read(sock,message,256);
            message[strlen(message)-1] = 0;
            char* nombrePlaylist;
            nombrePlaylist = calloc(1000,sizeof(char));
            strcpy(nombrePlaylist,message);
            memset(message,0,strlen(message));
            strcpy(message,"Esperando canción.\n ");
            write(sock,message,strlen(message));
            memset(message,0,strlen(message));
            read(sock,message,256);
            while (strcmp(message,"done")){
               message[strlen(message)-1] = 0;
               strcpy(canciones[index],message);
               memset(message,0,strlen(message)); 
               read(sock,message,256);
               strcpy(artista[index],message);
               memset(message,0,strlen(message));
               sprintf(mp3[index],"%s.mp3",canciones[index]);
               read(sock,message,256);
               index++;
            }
            guardarPlaylist(nombrePlaylist,canciones,artista,mp3,index,username);
            memset(message,0,strlen(message));
        }else if (!strcmp(message,"4")){
  
            memset(message,0,strlen(message));
            strcpy(message,"Enviando playlist.\n");            
            write(sock, message, strlen(message));
            memset(message,0,strlen(message));
            read(sock,message,256);
            char str[1024];
            sprintf(str,"/home/franco/Documents/SO/Proyecto 1/Final/Server/playlists from users/%s",message);
            memset(message,0,strlen(message));
            strcpy(message,"Enviando playlist."); 
            char *c;
            char tipo[50];
            c = calloc(1024,sizeof(char));

            FILE* fp;
            fp = fopen(str,"r");
            if(fp == NULL) {
              fp = fopen(str,"w");
              fprintf(fp, ":");
              fclose(fp);
            } else {
              fclose(fp);
            }
            fp = fopen(str,"ab+");
            strcpy(c,scannerPlaylist(fp,tipo));
            
            while (strcmp(c,"EOF")){
                if (!strcmp(tipo,"playlist")){
                    sprintf(message,"%s->",c);
                }else{
                    sprintf(message,"%s:",c);
                }
                write(sock,message,strlen(message));
                memset(message,0,strlen(message));
                read(sock,message,256);
                memset(message,0,strlen(message));
                strcpy(c,scannerPlaylist(fp,tipo));
            }
            strcpy(message,"stop");

            write(sock,message,strlen(message));
            memset(message,0,strlen(message));
            fclose(fp);
        }else if( !strcmp(message,"desplegar playlist") ){
            write(sock,message,strlen(message));
            memset(message,0,strlen(message));
            read(sock,message,256);
            desplegarPlaylist(sock,message,username);
        } else if(!strcmp(message,"file transfer")) {
            memset(message,0,strlen(message));
            strcpy(message, "Waiting for file");
            write(sock, message, strlen(message));
            obtenerTexto(username, sock);
            memset(message,0,strlen(message));
            strcpy(message, "Received");
            write(sock, message, strlen(message));
        }else {
            keepGoing = 0; 
        } 
        memset(message,0,strlen(message));
        read(sock, message, 256);

    }
    printf("Conexión con %s terminada.\n", username);
    fprintf(logFile, "Conexión con %s terminada.\n", username);
    fclose(logFile);
    return 0;
}  
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
    FILE *logFile = fopen("log", "a");
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
    fprintf(logFile, "Socket de servidor creado.\n");
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(5000);
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("Enlace completo.");
    fprintf(logFile, "Enlace completo.\n");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection_handler
    c = sizeof(struct sockaddr_in);
     
    //Accept and incoming connection
    puts("Esperando clientes...");
    fprintf(logFile, "Esperando clientes.\n");
    c = sizeof(struct sockaddr_in);
   pthread_t thread_id;
   
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Conexión aceptada.");
        fprintf(logFile, "Conexión aceptada.\n");

         
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        puts("Handler asignado.");
        fprintf(logFile, "Handler asignado.\n");
    }
    fclose(logFile);
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    
    return 0;
}