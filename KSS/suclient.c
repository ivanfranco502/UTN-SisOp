#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "echo_socket"



typedef struct{
	char DescriptorID[16];   //único, si usamos la hora + un random es suficiente.
	char PayloadDescriptor;  //tipo de operacion (handshake, vda, fss, ftp, list, cd, etc)
	int PayloadLenght;  //longitud del payload, 0 si no enviamos nada.
	char Payload[1050];  //mensaje.
}__attribute__ ((__packed__)) MPS_Package;


void generar_DescriptorID(char *);
int print_pkg (MPS_Package*);




int main(void){

	int s, t, len;
	struct sockaddr_un remote;
	char Buffer[1200];
	MPS_Package *mensaje;
	char comando[10];
	char comandos_posibles[6][10]={"mount","umount","tdd_dump","md5sum","format","ls"};
	int i=0;

	mensaje = (MPS_Package*) malloc(sizeof(MPS_Package));
	generar_DescriptorID(mensaje->DescriptorID);
//	strcpy(mensaje->DescriptorID, "aiofnusany");
	mensaje->PayloadDescriptor='0';
	mensaje->PayloadLenght=0;
	strcpy(mensaje->Payload, "\0" );

// lo inicializo como para arrancar directo con el Handshake 
// NOTA:  para el handshake es CLAVE agregar el \0 para qe pise el mensaje anterior porque sino queda en buffer

    	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        	perror("socket");
    		exit(1);
	}

	printf("Trying to connect...\n");

	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, SOCK_PATH);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);
	if (connect(s, (struct sockaddr *)&remote, len) == -1) {
        	perror("connect");
        	exit(1);
	}

	printf("Connected.\n");


	printf("Iniciando Handshake..\n");

        send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0);
	recv(s,Buffer,sizeof(Buffer),0);
	print_pkg((MPS_Package*)Buffer);


	if (((MPS_Package*)Buffer)->PayloadDescriptor=='0'){
	printf("Handshake Failed \n");
	}
	else {
		printf("Handshake OK\n");
		while(strcmp(mensaje->Payload,"halt")/*&&0!=recv(s,Buffer,sizeof(Buffer),0)*/){
//			mensaje->PayloadDescriptor='2';
			printf("NTVC>>");
			gets(mensaje->Payload);
 			sscanf(mensaje->Payload,"%s ",comando);
			i=0;
			while (strcmp(comandos_posibles[i],comando)&&i<6){

//			printf("%s\n",comandos_posibles[i]);
				i++;
			}

//analizo si el comando que llego era uno valido o no
			if (i<6){
				printf("metiste un comando piolaaaaaaa\n");
				mensaje->PayloadLenght=strlen(mensaje->Payload);
				generar_DescriptorID(mensaje->DescriptorID);
				send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0);
//				printf("el tanio de la struct es : %d\n", sizeof(MPS_Package));
			}
			else { 
				printf("sos un forrooo ese comando es invalido! \n");
                	}

		}
	}
	close(s);

	return 0;
}



int print_pkg(MPS_Package* mensaje){

        printf("Server: DescriptorID = %s\n", mensaje->DescriptorID);
        printf("Server: \"Pay Desc = %c\"\n", mensaje->PayloadDescriptor);
        printf("Server: \"Lenght = %d\"\n",mensaje->PayloadLenght);
        printf("Server: \"Payload = %s\"\n",mensaje->Payload);
	return 0;
}


