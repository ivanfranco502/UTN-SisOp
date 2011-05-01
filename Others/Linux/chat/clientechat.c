#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
 
#define SOCKET_MAX_BUFFER 100
 
int main() {
        int a;
        int descriptor = socket(AF_INET,SOCK_STREAM,0);
 
  // CREO NUEVO SOCKET
        char buffer[100];
        int pid=0;
        char mensaje[20];
 
        struct sockaddr_in *local_address= malloc (sizeof (struct sockaddr_in));
        struct sockaddr_in *remote_address= malloc (sizeof (struct sockaddr_in));
 
        local_address->sin_family= AF_INET;
        local_address->sin_addr.s_addr=INADDR_ANY;
        local_address->sin_port=htons(5301);
 
        bind(descriptor, (struct sockaddr*) local_address, sizeof(struct sockaddr_in));
 
 
        remote_address->sin_family= AF_INET;
        remote_address->sin_addr.s_addr=inet_addr("127.0.0.1");
        remote_address->sin_port=htons(5300);
 
        printf("voy a conectar.. el descriptor es: %d  \n",descriptor);
 
        a=connect(descriptor, (struct sockaddr *)remote_address, sizeof(struct sockaddr_in));
        printf("%d\n",a);
 
        buffer[0]='\0';
        //send(descriptor, "hola mundo\n", strlen("hola mundo\n")+1,0);
 
        pid=fork();
 
        if (pid!=0){
                while (strcmp(buffer,"chaucliente")){
                printf("el recv dio :%d\n",recv(descriptor, buffer, SOCKET_MAX_BUFFER,0));
 
//      close(descriptor);
 
                printf("el servidor de mierda mando: %s\n",buffer);
 
 
                }
                printf("No puede recibir mas mensajes \n");
                wait(pid);
                exit(0);
        }
        else{   while (strcmp(mensaje,"logout")){
                        scanf("%s",mensaje);
                        send(descriptor,mensaje,strlen(mensaje)+1,0);
                }
        exit(0);
        }
 
 
        printf("mi inet addr es: %d\n",remote_address->sin_addr.s_addr);
        printf("mi htons es: %d \n",local_address->sin_port);
        close(descriptor);
 
        return 0;
 
}
