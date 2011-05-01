#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
 
#define SOCKET_MAX_BUFFER 100
 
int main (){
 
        int addrlen = sizeof (struct sockaddr_in);
        char buffer[SOCKET_MAX_BUFFER];
        char msj_bienvenida[20]="bienvenido sorete\0";
        char mensaje[50];
        int pid=0;
 
 
        //creo el socket
 
        int descriptor= socket(AF_INET,SOCK_STREAM,0);
        int remote_client;
 
        struct sockaddr_in *local_address= malloc(sizeof (struct sockaddr_in));
 
        struct sockaddr_in *remote_address= malloc(sizeof (struct sockaddr_in));
 
 
        local_address->sin_family = AF_INET;
        local_address->sin_addr.s_addr=INADDR_ANY;
        local_address->sin_port = htons (5300);
 
        bind (descriptor,(struct sockaddr *) local_address, sizeof (struct sockaddr_in));
 
 
        listen(descriptor,100);
 
        printf("estoy escuchando el puerto de mierda\n");
 
        remote_client = accept (descriptor, (struct sockaddr *)remote_address, &addrlen);
 
        printf("el descriptor del socket es: %d , y el cliente remoto es %d \n" , descriptor, remote_client);
 
        printf("llego conexion del puto de  %d\n ",remote_client);
        //recv(remote_client, buffer, SOCKET_MAX_BUFFER,0);
        send(remote_client, msj_bienvenida, strlen(msj_bienvenida)+1,0);
 
//      printf("tire el send\n");
        mensaje[0]='\0';
        printf("manda algo: ");
        scanf("%s",mensaje);
//      printf("escribiste %s \n",mensaje);
 
        pid=fork ();
 
        if (pid!=0){
                while (strcmp (mensaje,"off")) {        //si mando salir corto el envio
 
                send(remote_client,mensaje,strlen(mensaje)+1,0);
//              printf("el strlen dio: %d \n",strlen(mensaje)+1);
                scanf("%s",mensaje);
//              printf("escribiste %s \n",mensaje);
 
 
                }
//              close(remote_client);
//              close(descriptor);
                wait(pid);
                exit(0);
        }
        else{
                buffer[0]='\0';
                while (strcmp(buffer,"logout")){    
            //si llega  chau dejo de recibir
                     printf("el recv dio :%d\n",recv(remote_client, buffer, SOCKET_MAX_BUFFER,0));
 
//      close(descriptor);
 
                     printf("el cliente de mierda mando: %s\n",buffer);
                }
 
                exit(0);
        }
 
 
//        printf("el inet recibido es: %d\n",remote_address->sin_addr.s_addr);
//      printf("mi htons es %d\n",remote_address->sin_port);
        close(descriptor);
//      printf("te mande el mensaje de mierda\n");
//      printf("%s",buffer);
 
//      close(descriptor);
 
 
return 0;
 
}
