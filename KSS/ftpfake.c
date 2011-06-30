#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SOCK_PATH "echo_socket"

//agregar esta funcion!!

int parseo_parentesis (char*,char*,char *);



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
	struct sockaddr_in remote;
	char Buffer[1200];
	MPS_Package *mensaje;
	char comando[10];
	char argumento[100];
	char comandos_posibles[5][10]={"sys_open","sys_read","sys_write","sys_list","sys_close"};
	int i=0;

	mensaje = (MPS_Package*) malloc(sizeof(MPS_Package));
	generar_DescriptorID(mensaje->DescriptorID);
//	strcpy(mensaje->DescriptorID, "aiofnusany");
	mensaje->PayloadDescriptor='1';
	mensaje->PayloadLenght=0;
	strcpy(mensaje->Payload, "\0" );

// lo inicializo como para arrancar directo con el Handshake 
// NOTA:  para el handshake es CLAVE agregar el \0 para qe pise el mensaje anterior porque sino queda en buffer

    	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        	perror("socket");
    		exit(1);
	}

	printf("Trying to connect...\n");

	remote.sin_family = AF_INET;
	remote.sin_addr.s_addr=INADDR_ANY;
	remote.sin_port = htons (5300);

	len = sizeof(struct sockaddr);
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
		while(strcmp(mensaje->Payload,"halt()")/*&&0!=recv(s,Buffer,sizeof(Buffer),0)*/){
//			mensaje->PayloadDescriptor='2';
			printf("NTVC>>");
			gets(mensaje->Payload);
 			parseo_parentesis(mensaje->Payload,comando,argumento);
			i=0;

			printf("%s\n",comando);
			while (strcmp(comandos_posibles[i],comando)&&i<6){

//			printf("%s\n",comandos_posibles[i]);
				i++;
			}

//analizo si el comando que llego era uno valido o no
			if (i<5){
				printf("metiste un comando piolaaaaaaa\n");
				mensaje->PayloadLenght=strlen(mensaje->Payload);
				generar_DescriptorID(mensaje->DescriptorID);
				send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0);
				
				recv(s,Buffer,sizeof(Buffer),0);
				print_pkg((MPS_Package *)Buffer);
				
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


// para las llamadas al sistema!!!!
int parseo_parentesis (char* cadena,char* comando,char *argumento){
	int i=0;
	int j=0;

	while (cadena[i]!= '('&&i<strlen(cadena)){
		comando[i]=cadena[i];
		i++;
	}
	comando[i]='\0';

	i++;
	
	while (cadena[i]!=')'&&i<strlen(cadena)){
		argumento[j]=cadena[i];
		i++;
		j++;
	}

	argumento[j]='\0';

	return 0;
}
