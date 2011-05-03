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
        int corrector;

        int descriptor= socket(AF_INET,SOCK_STREAM,0); //creo socket listener
        int remote_client;


        struct sockaddr_in *local_address= malloc(sizeof (struct sockaddr_in));
        struct sockaddr_in *remote_address= malloc(sizeof (struct sockaddr_in));


        local_address->sin_family = AF_INET;
        local_address->sin_addr.s_addr=INADDR_ANY;
        local_address->sin_port = htons (5300);

        bind (descriptor,(struct sockaddr *) local_address, sizeof (struct sockaddr_in));

	listen(descriptor,100);

        printf("estoy escuchando el puerto de mierda\n");

	//acepto conexion de datos
        remote_client = accept (descriptor, (struct sockaddr *)remote_address, &addrlen);

	printf("nueva conexion, el nuevo socket es:   %d\n ",remote_client);
	

	//	arranco envio de prueba
	printf("arranco el envio de datos ASCII\n");

        send(remote_client,"-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo1.txt\r\n",strlen("-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo1.txt\r\n"),0);
        send(remote_client,"-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo2.txt\r\n",strlen("-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo2.txt\r\n"),0);
        send(remote_client,"drwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 directorio1\r\n",strlen("drwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 directorio1\r\n"),0);

	printf("datos enviados!!\n");

	sleep(2);

	close(descriptor);

}
