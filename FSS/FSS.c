#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "funciones.h"

#define SOCK_PATH "echo_socket"



int main(){

	char func[50],f[50],vda[4],nomArch[31],rta[50000],sectores[50];
	int t=0,x=0,z=0,i=0,tam=0,val2=0;
	long val=0;
	char stru[1032];
	int s, len;
	char aux[1032];
	char Buffer[1032];       

	struct sockaddr_in remote;

	MPS_Package *mensaje;

	char mensaje_aux[20];	
	char stru_aux[1032];

	char comando[10];
	char argumento[100];
	mensaje = (MPS_Package*) malloc(sizeof(MPS_Package));
	generar_DescriptorID(mensaje->DescriptorID);
	mensaje->PayloadDescriptor='2';
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
	remote.sin_addr.s_addr=inet_addr("192.168.1.106");
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
		exit(1);
	}
	else {
      		printf("Handshake OK\n");
		/*while(strcmp(mensaje->Payload,"halt()")){
		    recv(s,Buffer,sizeof(Buffer),0);
                    print_pkg((MPS_Package*)Buffer);
		    strcpy(mensaje->Payload,"FSS");
			generar_DescriptorID(mensaje->DescriptorID);
			mensaje->PayloadLenght=strlen(mensaje->Payload);
			//send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0);
		}*/
        }

	while(1){
		recv(s,Buffer,sizeof(Buffer),0);
		print_pkg((MPS_Package*)Buffer);

		obtenerFuncion(((MPS_Package*)Buffer)->Payload, func);
	
		tam=strlen(func)+1;
		for(i=tam;i<1045;i++){
			stru[i-tam]=((MPS_Package*)Buffer)->Payload[i];;
		}
		i=0;
		if( !strcmp(func,"existeArchivo")){

			while(stru[x-1]!=')'){
					while(stru[x]!=',' && stru[x]!=')'){
						aux[i]=stru[x];	
						i++;x++;
					}
					aux[i]='\0';
					if(z==0){
						strcpy(vda,aux);
						i=0;
					}
					if(z==1){
						strcpy(nomArch,aux);
						i=0;
					}
					z++;x++;aux[i]='\0';
			}
				z=0;x=0;
				strcpy(rta,existeArchivo(vda,nomArch));
		}
	if( !strcmp(func,"infoArchivo")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
			if(z==1){
				strcpy(nomArch,aux);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		infoArchivo(vda,nomArch,rta);
	}
	if( !strcmp(func,"eliminarArchivo")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
			if(z==1){
				strcpy(nomArch,aux);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		strcpy(rta,eliminarArchivo(vda,nomArch));
	}
	if( !strcmp(func,"crearArchivo")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
			if(z==1){
				strcpy(nomArch,aux);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		strcpy(rta,crearArchivo(vda,nomArch));
	}
		if( !strcmp(func,"actualizarTamanio")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
			if(z==1){
				strcpy(nomArch,aux);
				i=0;
			}
			if(z==2){
				sscanf (aux, "%d", &val);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		strcpy(rta,actualizarTamanio(vda,nomArch,val));
	}
	if( !strcmp(func,"crearTablaSectoresLibres")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
			if(z==1){
				sscanf (aux, "%d", &val2);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		strcpy(rta,crearTablaSectoresLibres(vda,val2));
	}
	if( !strcmp(func,"dosSectoresLibres")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		dosSectoresLibres(vda,rta);
	}
	if( !strcmp(func,"asignarSectores")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')' && z<2){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
			if(z==1){
				strcpy(nomArch,aux);
				i=0;
			}			
			if(z==2){
		    	while(stru[x]!=')'){
			 	     aux[i]=stru[x];	
	                 i++;x++;
			     }
                 aux[i]='\0';
                 sprintf(sectores,"%s",aux);	
			}	
		aux[i]='\0';
		z++;x++;
		}
		z=0;x=0;t=0;	
		strcpy(rta,asignarSectores(vda,nomArch,sectores));
	}
	if( !strcmp(func,"liberarSectores")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
			if(z!=0){
				if(t==0){
					strcpy(sectores,aux);
					t=1;
				}else{
					strcat(sectores,",");
					strcat(sectores,aux);
				}
				i=0;
			}	
		aux[i]='\0';
		z++;x++;
		}
		z=0;x=0;t=0;
		strcpy(rta,liberarSectores(vda,sectores));
	}
	if( !strcmp(func,"listarDirectorio")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
        listarDirectorio(vda,rta);
	}
	if( !strcmp(func,"formatear")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
 				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
			if(z==1){
				sscanf (aux, "%d", &val2);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		strcpy(rta,formatear(vda,val2));
	}   
	    if( !strcmp(func,"tieneFormato")){
                while(stru[x-1]!=')'){
                        while(stru[x]!=',' && stru[x]!=')'){
                                aux[i]=stru[x];
                                i++;x++;
                        }
                        aux[i]='\0';
                        if(z==0){
                                strcpy(vda,aux);
                                i=0;
                        }
                z++;x++;aux[i]='\0';
                }
                z=0;x=0;
                strcpy(rta,tieneFormato(vda));
        }

			strcpy(mensaje->Payload,rta);
			mensaje->PayloadLenght=strlen(mensaje->Payload);
			printf("lo que respondo es: %s \n", mensaje->Payload);
			send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0);
			
			strcpy(rta,"\0");

	}

return 0;
}

