#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include "kss.h"

#define SOCK_PATH "echo_socket"

// PAyload descriptors: 0 shell   1 ftp        2 filesystem    3 vda
//int parseo_parentesis (char *,char *,char *);


/*
typedef struct{
	char DescriptorID[16];   //único, si usamos la hora + un random es suficiente.
	char PayloadDescriptor;  //tipo de operacion (handshake, vda, fss, ftp, list, cd, etc)
	int PayloadLenght;  //longitud del payload, 0 si no enviamos nada.
	char Payload[1050];  //mensaje.
	}__attribute__ ((__packed__)) MPS_Package;



typedef struct nodoL{
	int socket;
	char nombre[5];
	struct nodoL* puntero_siguiente;
	}nodo_lista_sockets;

typedef struct sect{
	unsigned int sector;
	struct sect *punteroSector;
}Sector;

typedef struct nTDD{
	unsigned int descriptor; // especie de "handle" al archivo abierto
	char nombreVDA[5];  // ubicacion del archivo. VDA1, VDA2, etc.
	char nombreArchivo[31];  //secuencia de hasta 30 caracteres combinando alfanuméricos y el carácter punto
	int tipoApertura; // lectura o escritura (0 ó 1)
	long size; // tamaño del archivo. Si es lectura permanece constante, si es escritura se debe actualizar.
	char buffer[1025]; // toda lectura o escritura se realiza en bloques de tamaño fijo 1K
	Sector sector; // struct con el numero de sector + puntero al proximo num de sector
	struct nTDD* siguiente;
}nodoTDD;


typedef struct {
		char nombre_syscall[10];
		nodoTDD* (*punt_syscall)(nodoTDD*, nodo_lista_sockets*, char*, int, char*);
}posible_syscall;



nodoTDD* sys_open(nodoTDD*, nodo_lista_sockets*, char*, int, char*);

int init_vector_syscalls(posible_syscall*);

int atender_handshake(nodo_lista_sockets*, MPS_Package*);
int atender_request(nodo_lista_sockets*, MPS_Package*);

int print_pkg (MPS_Package*);
nodo_lista_sockets* insertar_nodo_ordenado(nodo_lista_sockets*, nodo_lista_sockets*, int);
nodo_lista_sockets* generar_nodo();
void sockets_activos(nodo_lista_sockets *);
nodo_lista_sockets*  eliminar_nodo(nodo_lista_sockets*, nodo_lista_sockets*, nodo_lista_sockets*,int);
*/

int main(void)
{
    	extern int errno;
	int largo;
//	struct sockaddr socket_prueba;

	int descriptor_un,descriptor_in,i,s, s2, t, len, retval;
	int remote_client,remote_shell;
	int contador_sockets;
	int addrlen_in=0;
	int addrlen_un=0;

	nodo_lista_sockets* lista_sockets;
	nodo_lista_sockets* j;
	nodo_lista_sockets* p_ant;
	nodo_lista_sockets* p_sig;
	nodo_lista_sockets* p_aux;

	nodoTDD* Tdd;
	
	char Buffer[1200];

	posible_request vector_requests[11];


	init_vector_requests (vector_requests);

//	MPS_Package *mensaje=malloc (sizeof(MPS_Package);
	
	
	
	
	//int contador_descriptores;		//mantengo la posicion del ultimo socket
	//int fd_vec[10];				//vector donde almaceno los descriptores

//	Tdd = (nodoTDD*) malloc(sizeof(nodoTDD));
	Tdd = NULL;
	fd_set *set_sockets=malloc(sizeof(fd_set));	//bolsa de descriptores

            
	struct sockaddr_in *local_in= malloc(sizeof (struct sockaddr_in));
        struct sockaddr_in *remote_in= malloc(sizeof (struct sockaddr_in));

        struct sockaddr_un *local_un= malloc(sizeof (struct sockaddr_un));
        struct sockaddr_un *remote_un= malloc(sizeof (struct sockaddr_un));

/*
	mensaje = (MPS_Package*) malloc(sizeof(MPS_Package));
	lista_sockets = (nodo_lista_sockets*) malloc(sizeof(nodo_lista_sockets));
	lista_sockets = NULL;
	j = (nodo_lista_sockets*) malloc(sizeof(nodo_lista_sockets));
	p_ant = (nodo_lista_sockets*) malloc(sizeof(nodo_lista_sockets));
	p_sig = (nodo_lista_sockets*) malloc(sizeof(nodo_lista_sockets));
	p_aux = (nodo_lista_sockets*) malloc(sizeof(nodo_lista_sockets));

*/
	lista_sockets= NULL;



        local_in->sin_family = AF_INET;
        local_in->sin_addr.s_addr=INADDR_ANY;
        local_in->sin_port = htons (5300);

        local_un->sun_family = AF_UNIX;
        strcpy(local_un->sun_path, SOCK_PATH);
        unlink(local_un->sun_path);

	descriptor_un= socket(AF_UNIX,SOCK_STREAM,0);
	descriptor_in= socket(AF_INET,SOCK_STREAM,0);
	
	bind (descriptor_un,(struct sockaddr *) local_un, sizeof (struct sockaddr_un));
	bind (descriptor_in,(struct sockaddr *) local_in, sizeof (struct sockaddr_in));


	listen(descriptor_un,10);
	listen(descriptor_in,10);


    	printf("Waiting for a connection...\n");


/*	contador_descriptores=0;
	fd_vec[contador_descriptores]=descriptor_un;
	contador_descriptores++;
	fd_vec[contador_descriptores]=descriptor_in;*/

	p_aux =generar_nodo_sockets();
	lista_sockets = insertar_nodo_ordenado(lista_sockets, p_aux, descriptor_un);
	printf("%d\n", lista_sockets->socket);
	p_aux =generar_nodo_sockets();
	lista_sockets = insertar_nodo_ordenado(lista_sockets, p_aux, descriptor_in);
	printf("%d\n", (lista_sockets->puntero_siguiente)->socket);



	FD_ZERO(set_sockets);
//	FD_SET(descriptor_un,set_sockets);
//	FD_SET(descriptor_in,set_sockets);


// aca falta un WHILEEEEE

	while(1){
		i=0;

		j=lista_sockets;
		do{
			FD_SET(j->socket, set_sockets);
			printf("agrego socket %d\n",j->socket);
			j=j->puntero_siguiente;
		}while(j != NULL);

		
/*		for(j=0;j<=contador_descriptores;j++){
			if (fd_vec[j]!=0){			//lleno el set con todos los sockets que tengo hasta ahora 
				FD_SET(fd_vec[j],set_sockets);
				printf("agrego socket %d\n",fd_vec[j]);
			}
		}*/


		printf("Arranco con el select \n");

		select(encontrar_mayor(lista_sockets)+1,set_sockets,NULL,NULL,NULL);

		printf("Entro socket\n");


		j = lista_sockets;
		while(!FD_ISSET(j->socket,set_sockets) ){               //busco quien llamo

        		j=j->puntero_siguiente;
	        }

		printf("llamo %d\n",j->socket);



		FD_ZERO(set_sockets);                           //limpio la bolsa de sockets


							//listener del Shell
		if (j->socket == descriptor_un){
                        	remote_shell = accept (descriptor_un, (struct sockaddr *) remote_un, &addrlen_un);
                        	//contador_descriptores++;
                        	printf("el addrlen_un da : %d\n", addrlen_un);
				p_aux = generar_nodo_sockets();
				lista_sockets = insertar_nodo_ordenado(lista_sockets, p_aux, remote_shell);
							//fd_vec[contador_descriptores]=remote_shell;
				printf("entro una Shell \n");


		}

//		sockets_activos(lista_sockets);          //esto produce el segmentation fault

					//listener de algun AF_INET
		if (j->socket==descriptor_in){
	            remote_client = accept (descriptor_in, (struct sockaddr *)remote_in, &addrlen_in);
        	    //contador_descriptores++;
                p_aux = generar_nodo_sockets();
				lista_sockets = insertar_nodo_ordenado(lista_sockets, p_aux, remote_client);
				//fd_vec[contador_descriptores]=remote_client;
				printf("entro un AF_inet \n");


		}

		printf("llega aca\n");


		if ((j->socket!=descriptor_in )&&(j->socket!=descriptor_un)){				//algun socket existente (AF_INET o AF_UNIX)
				printf("llamo un socket existente\n");
				retval=recv(j->socket,Buffer,sizeof(Buffer),0);
				printf("el recv dio %d\n",retval);


						//hubo una desconexion 
				if (retval==0){
					close(j->socket);
					p_ant = generar_nodo_sockets();
					p_sig = generar_nodo_sockets();
					lista_sockets = eliminar_nodo(lista_sockets, p_ant, p_sig, j->socket);
					//fd_vec[i]=0;
					//contador_descriptores--;
				}
						//hay un pedido
				else {
					print_pkg((MPS_Package*)Buffer);
					if (((MPS_Package*)Buffer)->PayloadLenght==0){
						//es un handshakeeee
						switch (((MPS_Package*)Buffer)->PayloadDescriptor){
							case '0':
//                        					j->request=NULL;
                        		break;
                			case '1':
//								j->request=vector_requests;
								break;
							default:
//								j->request=NULL;
								break;
						}
		                lista_sockets = atender_handshake(j, lista_sockets,(MPS_Package*)Buffer);

					}
					else{
						Tdd = atender_request(j,(MPS_Package*)Buffer,Tdd,lista_sockets,vector_requests);
					}
				}
		sleep(1);
		}

		printf("llega\n");

	}


/*    t = sizeof(remote);
    if ((s2 = accept(s, (struct sockaddr *)remote, &t)) == -1) {
        perror("accept");
        exit(1);
    }
*/
    printf("Connected.\n");

//	retval = recv(s2, Buffer, sizeof(Buffer), 0);
//	mensaje = Buffer;
//	printf("Server: recibi %d bytes, que dicen DescriptorID = %s\n", retval, mensaje->DescriptorID);
//	printf("Server: \"Pay Desc = %c\"\n", mensaje->PayloadDescriptor);
//	printf("Server: \"Lenght = %d\"\n", mensaje->PayloadLenght);
//	printf("Server: \"Payload = %s\"\n", mensaje->Payload);  
        

    close(s2);
    close(s);

    return 0;
}


nodo_lista_sockets* atender_handshake(nodo_lista_sockets* nodo, nodo_lista_sockets* lista_sockets, MPS_Package* Buffer){
	int respuesta;
	nodo_lista_sockets* aux;
	nodo_lista_sockets* aux2;
	char buff[1072];
	MPS_Package* mensaje;
	nodo_lista_sockets* p_ant;
 	nodo_lista_sockets* p_sig;


	aux = lista_sockets;
	aux2=lista_sockets;
	switch(Buffer->PayloadDescriptor){
		case '0': //no puede haber mas de una shell
			respuesta = 1;
			while(aux != NULL){
				if(!strcmp(aux->nombre, "SH")){
					respuesta = 0;
				}
				aux = aux->puntero_siguiente;
			}
			break;
		case '1': //no puede haber ftp si no hay fss y al menos un vda montado
			respuesta = 0;
			while(aux!= NULL){
				if(!strcmp(aux->nombre, "FSS")){
					aux2 = lista_sockets;
					while(aux2!= NULL){
						if(!strncmp(aux2->nombre,"VDA",3) && !strcmp(aux2->estado, "montado")){
							respuesta = 1;
						}
						aux2 = aux2->puntero_siguiente;
					}
				}
				aux=aux->puntero_siguiente;
			}
			break;
		case '2': //no puede haber mas de un fss
			respuesta = 1;
			while(aux != NULL){
				if(!strcmp(aux->nombre, "FSS")){
					respuesta = 0;
				}
				aux = aux->puntero_siguiente;
			}
			break;
		case '3': //no puede haber dos vdas con el mismo nombre
			respuesta = 1;
			recv(nodo->socket,buff, sizeof(buff),0);
			while(aux != NULL){
				if(!strcmp(aux->nombre, ((MPS_Package*)buff)->Payload)){
					respuesta = 0;
				}
				aux = aux->puntero_siguiente;
			}
			break;
	}

//	if(1){                       //comentado para probar !!!
	if(respuesta){                     
//		printf("llegue al if \n");
		mensaje=(MPS_Package*)malloc (sizeof(MPS_Package));
		strcpy(mensaje->DescriptorID,Buffer->DescriptorID);
		mensaje->PayloadDescriptor='1';			//1 indica OK  0 Fail
		mensaje->PayloadLenght=0;
		strcpy(mensaje->Payload,"\0");
		send(nodo->socket,(char *)mensaje,21+mensaje->PayloadLenght+1,0);	//como acepte mando el OK
		free(mensaje);
		switch(Buffer->PayloadDescriptor){
			case '0':
				strcpy(nodo->nombre, "SH");
				break;
			case '1':
				strcpy(nodo->nombre, "FTP");
				break;
			case '2':
				strcpy(nodo->nombre, "FSS");
				break;	
			case '3':
				strcpy(nodo->nombre, ((MPS_Package*)buff)->Payload);
				strcpy(nodo->estado, "nomontado");
				break;
		}
	}
	else{
		mensaje=(MPS_Package*)malloc (sizeof(MPS_Package));
		strcpy(mensaje->DescriptorID,Buffer->DescriptorID);
		mensaje->PayloadDescriptor='0';			//1 indica OK  0 Fail
		mensaje->PayloadLenght=0;
		strcpy(mensaje->Payload,"\0");
		send(nodo->socket,(char *)mensaje,21+mensaje->PayloadLenght+1,0);	//como rechace el hs mando fail
		free(mensaje);
		close(nodo->socket);
 		p_ant = generar_nodo_sockets();
  		p_sig = generar_nodo_sockets();
  		lista_sockets = eliminar_nodo(lista_sockets, p_ant, p_sig, nodo->socket);
	}
	return lista_sockets;

}
nodoTDD* atender_request(nodo_lista_sockets* nodo, MPS_Package* Buffer,nodoTDD* Tdd, nodo_lista_sockets* lista_sockets, posible_request* vector_requests){
	char comando[10];
	char argumento[1040]; //ojo con la sys_write
	int pos;
        MPS_Package *mensaje=malloc (sizeof(MPS_Package));

	//Parseo tanto para Shell como para FTP
	switch (Buffer->PayloadDescriptor){
		case '0':          //don shell
		//podriamos llamar a  una funcion completa para cada caso
			sscanf(Buffer->Payload,"%s %s",comando,argumento);
			printf("el comando es %s\n",comando);
			printf("el argumento es %s\n",argumento);

                        pos = buscar_request(comando,vector_requests);
			printf("posicion %d\n",pos);
			
                        if (pos!=-1)
                               Tdd = ( *(vector_requests[pos].punt_request))(Tdd,lista_sockets,argumento,nodo->socket,Buffer->DescriptorID, Buffer->PayloadLenght);
                                        //  llamada a syscall.. a ver casteo el punteor a void, busco el puntero de la syscall en posicion pos, y la llamo
                        else printf("comando invalido!!\n");



			break;
		case '1':		//don ftp
			parseo_parentesis(Buffer->Payload,comando,argumento);
                        printf("el comando es %s\n",comando);
                        printf("el argumento es %s\n",argumento);

			pos = buscar_request(comando,vector_requests);
                        printf("posicion %d\n",pos);
			printf("la direccion es %0x  y debe ser %0x \n",vector_requests[pos].punt_request,&sys_open);

			if (pos!=-1)
				Tdd = (*(vector_requests[pos].punt_request))(Tdd,lista_sockets,argumento,nodo->socket,Buffer->DescriptorID, Buffer->PayloadLenght);
					//  llamada a syscall.. a ver casteo el punteor a void, busco el puntero de la syscall en posicion pos, y la llamo
			else printf("syscall invalida!!\n");

			break;
		default:
			//si es VDA o FSS veremos..
			break;

	}


	free(mensaje);
	return(Tdd);
}







int print_pkg(MPS_Package* mensaje){
        
	printf("Server: DescriptorID = %s\n", mensaje->DescriptorID);
        printf("Server: \"Pay Desc = %c\"\n", mensaje->PayloadDescriptor);
        printf("Server: \"Lenght = %d\"\n",mensaje->PayloadLenght);
        printf("Server: \"Payload = %s\"\n",mensaje->Payload);	
}
	

nodo_lista_sockets* insertar_nodo_ordenado(nodo_lista_sockets* lista_sockets, nodo_lista_sockets* p_aux, int socket){
	p_aux->socket=socket;
	p_aux->puntero_siguiente = lista_sockets;
	lista_sockets = p_aux;
	return(lista_sockets);
}

nodo_lista_sockets*  eliminar_nodo(nodo_lista_sockets* lista_sockets, nodo_lista_sockets* p_ant, nodo_lista_sockets* p_sig,int socket){
	p_ant = lista_sockets;
	p_sig = lista_sockets->puntero_siguiente;
	
	if(p_ant->socket == socket){
		lista_sockets = p_ant->puntero_siguiente;
		free(p_ant);
	}else{
		p_sig=p_ant->puntero_siguiente;
		while(p_sig->socket!= socket){
			p_ant=p_sig;
			p_sig = p_sig->puntero_siguiente;
		}
		p_ant->puntero_siguiente = p_sig->puntero_siguiente;
		free(p_sig);
	}
	return lista_sockets;

}

nodo_lista_sockets* generar_nodo_sockets(){
	nodo_lista_sockets *nodo_nuevo;
	nodo_nuevo = (nodo_lista_sockets*) malloc(sizeof(nodo_lista_sockets));
	return(nodo_nuevo);
}

void sockets_activos(nodo_lista_sockets *lista_sockets){
	nodo_lista_sockets *nodo;
	while(nodo!=NULL){
		printf("%d", nodo->socket);
		nodo=nodo->puntero_siguiente;
	}
}


/*
int init_vector_comandos(posible_comando *vector_comandos){

	strcpy(vector_comandos[0].nombre_comando,"mount");
	vector_comandos[0].punt_comando=
*/

int init_vector_requests(posible_request *vector_requests){

	strcpy(vector_requests[0].nombre_request,"sys_open");
	vector_requests[0].punt_request=&sys_open;
	strcpy(vector_requests[1].nombre_request,"sys_read");
	vector_requests[1].punt_request=&sys_read;
	strcpy(vector_requests[2].nombre_request,"sys_write");
	vector_requests[2].punt_request=&sys_write;
	strcpy(vector_requests[3].nombre_request,"sys_close");
	vector_requests[3].punt_request=&sys_close;
	strcpy(vector_requests[4].nombre_request,"sys_list");
	vector_requests[4].punt_request=&sys_list;
        strcpy(vector_requests[5].nombre_request,"mount");
        vector_requests[5].punt_request=&mount;
        strcpy(vector_requests[6].nombre_request,"umount");
        vector_requests[6].punt_request=&umount;
        strcpy(vector_requests[7].nombre_request,"tdd_dump");
        vector_requests[7].punt_request=&tdd_dump;
        strcpy(vector_requests[8].nombre_request,"format");
        vector_requests[8].punt_request=&format;
        strcpy(vector_requests[9].nombre_request,"ls");
        vector_requests[9].punt_request=&ls;
        strcpy(vector_requests[10].nombre_request,"md5sum");
        vector_requests[10].punt_request=&md5sum;



	return 0;
}


int buscar_request(char *request,posible_request* vector_requests){


	printf("busco a: %s\n",request);
	int i=0;
	while (strcmp(request,vector_requests[i].nombre_request) && i<12){
		i++;
	}
	if (i==12){
		printf("request not found \n");
		return -1;
	}
	else{
		printf("Request OK \n");
		return i;
	}
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

int encontrar_mayor(nodo_lista_sockets* lista_sockets){
 int mayor = 0;
 while(lista_sockets != NULL){
  if(lista_sockets->socket > mayor){
   mayor = lista_sockets->socket;
  }
  lista_sockets= lista_sockets->puntero_siguiente;
 } 
 return(mayor); 
}
