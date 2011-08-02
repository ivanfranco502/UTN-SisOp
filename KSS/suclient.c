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
	char consola[100];
	char comandos_posibles[6][10]={"mount","umount","tdd_dump","md5sum","format","ls"};
	int i=0;
	FILE* archivo;
	char nombre[31];
	char argumento[40];
	char desc[10];
	char llamada[40];
	
	mensaje = Buffer;
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
			gets(consola);
 			sscanf(consola,"%s",comando);
			strcpy(mensaje->Payload, consola);
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
				mensaje->PayloadDescriptor='0';
				if((strcmp(comando,"md5sum"))){
					send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0);
				}
				
				if(!(strcmp(comando,"mount"))){
					printf("entre al mount\n");
					recv(s,Buffer,sizeof(Buffer),0);
					mensaje = (MPS_Package*)Buffer;
					printf("%s",mensaje->Payload);
				}
				
				if(!(strcmp(comando,"umount"))){
					printf("entre al mount\n");
					recv(s,Buffer,sizeof(Buffer),0);
					mensaje = (MPS_Package*)Buffer;
					printf("%s",mensaje->Payload);
				}
				
				if(!(strcmp(comando,"tdd_dump"))){
					printf("entre al dump\n");
					recv(s,Buffer,sizeof(Buffer),0);
					mensaje = (MPS_Package*)Buffer;
					while(mensaje->PayloadDescriptor == '1'){
						printf("%s",mensaje->Payload);
						mensaje->PayloadDescriptor='0';
						strcpy(mensaje->Payload, "Mandame masss" );
						mensaje->PayloadLenght=strlen("Mandame masss");
						send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0);
						recv(s,Buffer,sizeof(Buffer),0);
						mensaje = (MPS_Package*)Buffer;
						//printf("%c\n",mensaje->PayloadDescriptor);
					}
					printf("sali del while\n");
				}
				if(!(strcmp(comando,"ls"))){
					printf("entre al ls\n");
					recv(s,Buffer,sizeof(Buffer),0);
					mensaje = (MPS_Package*)Buffer;
					printf("%s",mensaje->Payload);
				}
				
				if(!(strcmp(comando,"format"))){
					printf("entre al format\n");
					recv(s,Buffer,sizeof(Buffer),0);
					mensaje = (MPS_Package*)Buffer;
					printf("%s",mensaje->Payload);
				}
				
				if(!(strcmp(comando,"md5sum"))){
					printf("entre al md5sum\n");
					strcpy(argumento,mensaje->Payload);
					memcpy(nombre, argumento+13, mensaje->PayloadLenght - 12);
					mensaje->PayloadDescriptor = '1';
					strcpy(mensaje->Payload,"sys_open(0,");
					strcat(mensaje->Payload,argumento+7);
					strcat(mensaje->Payload,")");
					mensaje->PayloadLenght = strlen(mensaje->Payload);
					send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0);
					recv(s, Buffer,sizeof(Buffer),0);
					mensaje = (MPS_Package*)Buffer;
					if(mensaje->PayloadDescriptor == '1'){
						archivo = fopen(nombre,"w");
						strcpy(desc,mensaje->Payload);
						mensaje->PayloadDescriptor = '1';
						strcpy(mensaje->Payload,"sys_read(");
						strcat(mensaje->Payload,desc);
						strcat(mensaje->Payload,")");
						mensaje->PayloadLenght = strlen(mensaje->Payload);
						send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0);
						recv(s, Buffer,sizeof(Buffer),0);
						mensaje = (MPS_Package*) Buffer;
						while(mensaje->PayloadDescriptor == '1'){
							//fprintf(archivo, "%s", mensaje->Payload,sizeof(char));
							fwrite(mensaje->Payload, sizeof(char), mensaje->PayloadLenght, archivo);
							mensaje->PayloadDescriptor = '1';
							strcpy(mensaje->Payload,"sys_read(");
							strcat(mensaje->Payload,desc);
							strcat(mensaje->Payload,")");
							mensaje->PayloadLenght = strlen(mensaje->Payload);
							//print_pkg(mensaje);
							send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0);
							recv(s, Buffer,sizeof(Buffer),0);
							mensaje = (MPS_Package*) Buffer;
						}
						//fprintf(archivo, "%c",'\0');
						mensaje->PayloadDescriptor = '1';
						strcpy(mensaje->Payload,"sys_close(");
						strcat(mensaje->Payload,desc);
						strcat(mensaje->Payload,")");
						mensaje->PayloadLenght = strlen(mensaje->Payload);
						send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0);
						recv(s, Buffer,sizeof(Buffer),0);
						print_pkg((MPS_Package*)Buffer);
						fclose(archivo);
						strcpy(llamada, "md5sum ");
						strcat(llamada, nombre);
						system(llamada);
					}else{
						printf("%s", mensaje->Payload);
					}
				}
				
				
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


