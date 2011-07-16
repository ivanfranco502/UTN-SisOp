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
	struct infoGrabar{
		long dir1;
		char dato1[512];
		long dir2;
		char dato2[512];	
	};
	int s, t, len;
	struct sockaddr_in remote;
	char Buffer[1200];
	MPS_Package *mensaje;
	char comando[10];
	char argumento[100];
//	char comandos_posibles[5][10]={"existeArchivo","infoArchivo","eliminarArchivo","crearArchivo","actualizarTamanio","CrearTablaSectoresLibres","dosSectoresLibres","asignarSectores","liberarSectores","lstarDirectorio","formatear" };
	int i=0;
	char mensaje_aux[14];
	int recibidos;
	struct infoGrabar *estructura;
	char stru_aux[1032];

	mensaje = (MPS_Package*) malloc(sizeof(MPS_Package));
	generar_DescriptorID(mensaje->DescriptorID);
//	strcpy(mensaje->DescriptorID, "aiofnusany");
	mensaje->PayloadDescriptor='3';
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
		recv(s, Buffer, sizeof(Buffer),0);
	if(!strcmp(((MPS_Package*)Buffer)->Payload, "Mandame el nombre")){
	do{
		printf("Coloca el nombre de la VDA: ");			//identifico VDA
		gets(mensaje->Payload);
		mensaje->PayloadLenght=strlen(mensaje->Payload);
	}while(mensaje->PayloadLenght!=4);
	
	send(s,(char*)mensaje,21+mensaje->PayloadLenght+1,0);
	recv(s,Buffer,sizeof(Buffer),0);
	print_pkg((MPS_Package*)Buffer);
	

	if (((MPS_Package*)Buffer)->PayloadDescriptor=='0'){
	printf("Handshake Failed \n");
	}
	else {
		printf("Handshake OK\n");
		while(strcmp(mensaje->Payload,"halt()")/*&&0!=recv(s,Buffer,sizeof(Buffer),0)*/){
//			mensaje->PayloadDescriptor='2';
		    recibidos = recv(s,Buffer,sizeof(Buffer),0);
			printf("%d\n",recibidos);
			print_pkg((MPS_Package*)Buffer);
			printf("VDA>>");

			strncpy(mensaje_aux,((MPS_Package*)Buffer)->Payload,11);
			mensaje_aux[11]='\0';

			if (strcmp(mensaje_aux,"getSectores")){
				//es putsectores
				for(i=12;i<1045;i++){
					stru_aux[i-12]=((MPS_Package*)Buffer)->Payload[i];
				}
				estructura = (struct infoGrabar*)stru_aux;
				printf("\ndir1: %ld\ndir2: %ld\ndato1: %s\ndato2: %s\n", estructura->dir1, estructura->dir2, estructura->dato1, estructura->dato2);
				//gets(mensaje->Payload); //deberia devolver ok
			}
			else{					//es get sectores
				printf("Sector 1: ");
				gets(mensaje->Payload);
				printf("VDA>>Sector 2: ");
				gets(mensaje->Payload+512);
				printf("Enviados los sectores! \n");
			}
 //			parseo_parentesis(mensaje->Payload,comando,argumento);
//			i=0;

//			printf("%s\n",comando);
//			while (strcmp(comandos_posibles[i],comando)&&i<6){

//			printf("%s\n",comandos_posibles[i]);
//				i++;
//			}

//analizo si el comando que llego era uno valido o no
//			if (i<5){
//				printf("metiste un comando piolaaaaaaa\n");
				mensaje->PayloadLenght=strlen(mensaje->Payload+512)+512;
				generar_DescriptorID(mensaje->DescriptorID);
				send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0);
//				printf("el tanio de la struct es : %d\n", sizeof(MPS_Package));
//			}
//			else { 
//				printf("sos un forrooo ese comando es invalido! \n");
//                	}

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
