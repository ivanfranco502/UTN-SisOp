#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "funcionesftp.h"

#define SOCKET_MAX_BUFFER 100
 

int main (){


        int addrlen = sizeof (struct sockaddr_in);
        char buffer[SOCKET_MAX_BUFFER];
        char msj_bienvenida[20]="bienvenido sorete\0";
        char mensaje[50];

	int corrector;

        int fd_vec[10];         //se me ocurre 1 server y 9 clientes
        int contador_descriptores=0;
        fd_set *set_sockets=malloc(sizeof(fd_set));
        int i=0;
        int j;

        //creo el socket

        int descriptor= socket(AF_INET,SOCK_STREAM,0);
	int data_socket= socket(AF_INET,SOCK_STREAM,0);
        int remote_client;

        struct sockaddr_in *local_address= malloc(sizeof (struct sockaddr_in));
        struct sockaddr_in *remote_address= malloc(sizeof (struct sockaddr_in));
	struct sockaddr_in *data_socket_address= malloc (sizeof (struct sockaddr_in));


        local_address->sin_family = AF_INET;
        local_address->sin_addr.s_addr=INADDR_ANY;
        local_address->sin_port = htons (21);


	data_socket_address->sin_family = AF_INET;
	data_socket_address->sin_addr.s_addr= INADDR_ANY;
	data_socket_address->sin_port=htons(5300);

        bind (descriptor,(struct sockaddr *) local_address, sizeof (struct sockaddr_in));

//	bind (data_socket,(struct sockaddr *) data_socket_address, sizeof (struct sockaddr_in));

        listen(descriptor,100);

        printf("estoy escuchando el puerto de mierda\n");

        fd_vec[contador_descriptores]=descriptor;            //coloco el descriptor del socket que hace de listener en la pos 0

        FD_ZERO(set_sockets);
        FD_SET(descriptor, set_sockets);                //agrego el socket al SET

//      sleep(2);

        while (strcmp(buffer,"apagar")){

                for (j=0;j<=contador_descriptores;j++){                 //agrego todos los descriptores al set
                FD_SET(fd_vec[j],set_sockets);

                }

                printf("estoy con el select...\n");
                select(fd_vec[contador_descriptores]+1, set_sockets,NULL,NULL,NULL);

                while(!FD_ISSET(fd_vec[i],set_sockets) ){               //busco quien llamo

                                        i++;
                }

                FD_ZERO(set_sockets);                           //limpio la bolsa de sockets

                if (fd_vec[i]==descriptor){                      //llamo el listener o sea hay una nueva conexion

                        remote_client = accept (descriptor, (struct sockaddr *)remote_address, &addrlen);
                        contador_descriptores++;
                        fd_vec[contador_descriptores]=remote_client;
                        printf("nueva conexion, el nuevo socket es:   %d\n",remote_client);
                        printf("el contador de descriptores esta en %d\n",contador_descriptores);

				//arranco charla con ftp

			enviar_comando(remote_client,"220 POWER\r\n",buffer);
/*
			send(remote_client,"220 POWER\r\n",strlen("220 POWER\r\n"),0);
			printf("le mande el 220 de mierda\n");
			corrector=recv(remote_client,buffer,SOCKET_MAX_BUFFER,0);
			buffer[corrector]='\0';
			printf("%s\n",buffer);
*/
			enviar_comando(remote_client,"230 POWER\r\n",buffer);
/*
                        send(remote_client,"230 POWER\r\n",strlen("230 POWER\r\n"),0);
                        printf("le mande el 230 de mierda\n");
                        corrector=recv(remote_client,buffer,SOCKET_MAX_BUFFER,0);
                        buffer[corrector]='\0';
                        printf("%s\n",buffer);
*/
			//espero por PWD y le indico el directorio

			wait_for(remote_client,"PWD\r\n",buffer);

/*			while(strcmp(buffer,"PWD\r\n")){

                        	send(remote_client,"500 NO PERMITIDO\r\n",strlen("500 NO PERMITIDO\r\n"),0);
                        	printf("le mande el 500 de mierda\n");
                        	corrector=recv(remote_client,buffer,SOCKET_MAX_BUFFER,0);
                        	buffer[corrector]='\0';
                        	printf("%s\n",buffer);
			
				sleep(2);
			}
*/
			enviar_comando(remote_client,"257 \"/\" Is the current directory \r\n",buffer);
/*
                        send(remote_client,"257 \"/\" Is the current directory \r\n",strlen("257 \"/\" Is the current directory \r\n"),0);
                        printf("le mande el 257 de mierda\n");
                        corrector=recv(remote_client,buffer,SOCKET_MAX_BUFFER,0);
                        buffer[corrector]='\0';
                        printf("%s\n",buffer);

*/
			//espero a que me diga el tipo 

			wait_for(remote_client,"TYPE A\r\n",buffer);

/*			while(strcmp(buffer,"TYPE A\r\n")){
                  		send(remote_client,"500 NO PERMITIDO\r\n",strlen("500 NO PERMITIDO\r\n"),0);
                        	printf("le mande el 500 de mierda\n");
                        	corrector=recv(remote_client,buffer,SOCKET_MAX_BUFFER,0);
                        	buffer[corrector]='\0';
                        	printf("%s\n",buffer);

				sleep(1);

			}
*/


			enviar_comando(remote_client,"200 Type set to A\r\n",buffer);
/*
                        send(remote_client,"200 Type set to A\r\n",strlen("200 Type set to A\r\n"),0);
                        printf("le mande el 200 de mierda\n");
                        corrector=recv(remote_client,buffer,SOCKET_MAX_BUFFER,0);
                        buffer[corrector]='\0';
                        printf("%s\n",buffer);
*/


			//arranco con el otro puerto (el de datos)  // le paso mi ip y el puerto 5300 al que se tiene que conectar el cliente
			sleep(5);
			enviar_comando(remote_client,"227 Entering Pasive Mode (192,168,1,104,20,180)\r\n",buffer);

/*
			send(remote_client,"227 Entering Pasive Mode (192,168,1,104,20,180)\r\n",strlen("227 Entering Pasive Mode (192,168,1,104,20,180)\r\n"),0);
                        printf("le mande el 227 de mierda\n");
//	                data_client = accept (data_socket, (struct sockaddr *)remote_address, &addrlen);
                        corrector=recv(remote_client,buffer,SOCKET_MAX_BUFFER,0);
                        buffer[corrector]='\0';
                        printf("%s\n",buffer);

			sleep(2);
*/

			sleep(10);
			enviar_comando_no_recv(remote_client,"150 Openning ASCII mode\r\n",buffer);

/*
                        send(remote_client,"150 Openning ASCII mode\r\n",strlen("150 Openning ASCII mode\r\n"),0);
                        printf("le mande el 150 de mierda\n");
//                      data_client = accept (data_socket, (struct sockaddr *)remote_address, &addrlen);
//                      corrector=recv(remote_client,buffer,SOCKET_MAX_BUFFER,0);
//                      buffer[corrector]='\0';
//                      printf("%s\n",buffer);
			sleep(10);
*/
 			sleep(10);
			enviar_comando(remote_client,"226 Transfer Complete\r\n",buffer);

/*
                        send(remote_client,"226 Transfer Complete\r\n",strlen("226 Transfer Complete\r\n"),0);
                        printf("le mande el 226 de mierda\n");
//                      data_client = accept (data_socket, (struct sockaddr *)remote_address, &addrlen);
                        corrector=recv(remote_client,buffer,SOCKET_MAX_BUFFER,0);
                        buffer[corrector]='\0';
                        printf("%s\n",buffer);
*/



			close(remote_client);
			sleep(10);
                }


                else{                           //mando un mensaje algun cliente en el socket fd_vec[i]

                        recv(fd_vec[i],buffer,SOCKET_MAX_BUFFER,0);
                        printf("el mensaje recibido de %d  fue: %s\n",fd_vec[i],buffer);

                        for(j=1;j<=contador_descriptores;j++) {

                                if (j!=i){
                                        send(fd_vec[j],buffer,strlen(buffer)+1,0);
                                }
                        }
                }

        i=0;

        }

        close(descriptor);


return 0;

 

}
