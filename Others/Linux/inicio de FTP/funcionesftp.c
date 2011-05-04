#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SOCKET_MAX_BUFFER 100

int enviar_comando (int cliente,char *comando,char *buffer){

        int corrector;

        send(cliente,comando,strlen(comando),0);
        printf("enviado comando %s\n",comando);
        corrector=recv(cliente,buffer,SOCKET_MAX_BUFFER,0);
        buffer[corrector]='\0';
        printf("%s\n",buffer);
        sleep(1);
        return 0;
}


int enviar_comando_no_recv (int cliente,char *comando,char *buffer){

        int corrector;

        send(cliente,comando,strlen(comando),0);
        printf("enviado comando %s\n",comando);

        sleep(1);
        return 0;
}


int wait_for (int cliente,char *comando,char *buffer){

        int corrector;

        while(strcmp(buffer,comando)){
                send(cliente,"500 NO PERMITIDO\r\n",strlen("500 NO PERMITIDO\r\n"),0);
                printf("le mande el 500\n");
                corrector=recv(cliente,buffer,SOCKET_MAX_BUFFER,0);
                buffer[corrector]='\0';
                printf("%s\n",buffer);
                sleep(2);
        }
        return 0;
}


