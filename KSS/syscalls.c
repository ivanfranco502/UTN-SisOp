#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "kss.h"
/*
typedef struct sect{
	unsigned int sector;
	struct sect *punteroSector;
}Sector;

typedef struct nTDD{
	unsigned int descriptor; // especie de "handle" al archivo abierto
	char nombreVDA[5];  // ubicacion del archivo. VDA1, VDA2, etc.
	char nombreArchivo[31];  //secuencia de hasta 30 caracteres combinando alfanuméricos y el carácter punto
	int tipoApertura; // lectura o escritura (0 o 1)
	long size;// size del archivo. Si es lectura permanece constante, si es escritura se debe actualizar.
	char buffer[1025]; // toda lectura o escritura se realiza en bloques de tamanio fijo 1k
	Sector* sector; // struct con el numero de sector + puntero al proximo num de sector
	struct nTDD* siguiente;
}nodoTDD;

typedef struct{
	char DescriptorID[16];   //unico, si usamos la hora + un random es suficiente.
	char PayloadDescriptor;  //tipo de operacion (handshake, vda, fss, ftp, list, cd, etc)
	int PayloadLenght;  //longitud del payload, 0 si no enviamos nada.
	char Payload[1050];  //mensaje.
	}__attribute__ ((__packed__)) MPS_Package;

typedef struct nodoL{
	int socket;
	char nombre[5];
	struct nodoL* puntero_siguiente;
	}nodo_lista_sockets;

nodoTDD*  generar_nodo();
nodoTDD* agregar_nodo_TDD(nodoTDD*, nodoTDD*);
unsigned int descriptor_nuevo(nodoTDD*);
int buscar_socket(nodo_lista_sockets*, char*);
int buscar_tipo_apertura(nodoTDD*, unsigned int);
int rellenar_sectores(char*);
nodoTDD* eliminar_nodo_TDD(nodoTDD*, unsigned int);


int main (void){
return 1;
}

*/



//          OPEN                 OPEN                     OPEN

nodoTDD* sys_open(nodoTDD* Tdd, nodo_lista_sockets* lista_sockets, char* argumento, int socket, char* desc){
	char tipo_de_apertura;
	char vda[5];
	char nombre_archivo[31];
	char buffer[30];
	nodoTDD* aux;
	int flag = 1;
	MPS_Package *mensaje;
	int Fss;
	unsigned int desc_tdd;
	nodoTDD* entradaTDD;
	int i =0, j, k;
	char tamanio_archivo[10];
	char sector_aux[10];
	int cantidad_sectores =0;
	unsigned int numero_sector;
	long cualquiera;
	

//	printf("entre a la syscall\n");
//	aux = (nodoTDD*) malloc(sizeof(nodoTDD));
	aux = Tdd;
	mensaje = (MPS_Package*) malloc(sizeof(MPS_Package));
	strcpy(mensaje->DescriptorID, desc);
	Fss = buscar_socket(lista_sockets, "FSS");
	
	printf("ya busque el socket y es %d\n",Fss);

//	entradaTDD = (nodoTDD*) malloc(sizeof(nodoTDD));
	

	tipo_de_apertura = argumento[i];
	for(i=3; i<7;i++){
		vda[i-3] = argumento[i];
 	}
	vda[i-3]= '\0';
 	i++;
 	while(argumento[i]!='\0'){
  		nombre_archivo[i-8] = argumento[i];
  		i++;
 	}
 	nombre_archivo[i-8] = '\0';
 	
	printf("llegue hasta aca\n");
//	printf("el tipo de operacion es , la vda es , el nombre del archivo es ");//, vda, nombre_archivo);


//	tipo_de_apertura='0';
//	strcpy(vda,"VDA1");
//	strcpy(nombre_archivo,"CACA");

	printf("el tipo de operacion es %c , la vda es %s, el nombre del archivo es %s \n",tipo_de_apertura, vda, nombre_archivo);

// 1) verifico que el archivo no este abierto
	while(aux!=NULL && flag){
		if(! strcmp(aux->nombreVDA, vda)){
			if(! strcmp(aux->nombreArchivo, nombre_archivo))
				flag = 0;
		}
		aux = aux->siguiente;
	}
	

//	printf("arranco ifff\n");
	if(flag){
		//no esta abierto
// 2) consulto al FSS que el archivo exista
		mensaje->PayloadDescriptor = '2';
		strcpy(mensaje->Payload, "existeArchivo(");
		strcat(mensaje->Payload, vda);
		strcat(mensaje->Payload, ",");
		strcat(mensaje->Payload, nombre_archivo);
		strcat(mensaje->Payload, ")");
		mensaje->PayloadLenght = strlen(mensaje->Payload);
		send(Fss, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);
		recv(Fss, buffer, sizeof(buffer), 0);
//		mensaje = (MPS_Package*) buffer;

//              printf("recibi %s\n",mensaje->Payload);

		if(! strcmp( ( (MPS_Package*)buffer )->Payload, "OK")){
			//existe el archivo
			desc_tdd = descriptor_nuevo(Tdd);
			switch(tipo_de_apertura){
				case '0':
// en caso de LECTURA
// 3) solicitar al FSS la lista de bloques y el tamanio
//				printf("el archivo se llama %s\n",nombre_archivo);
				mensaje->PayloadDescriptor = '3';
				strcpy(mensaje->Payload, "infoArchivo(");
				strcat(mensaje->Payload, vda);
				strcat(mensaje->Payload, ",");
//				printf("el archivo es %s",nombre_archivo);
				strcat(mensaje->Payload, nombre_archivo);
				strcat(mensaje->Payload, ")");
//				printf("el mensaje es %s\n",mensaje->Payload);
				mensaje->PayloadLenght = strlen(mensaje->Payload);
				send(Fss, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);
				printf("enviado! \n");
				//recv(Fss, buffer, sizeof(buffer), 0);
				mensaje = (MPS_Package*) buffer;
				printf("recibi esto %s\n", mensaje->Payload);
				//hay que parsear el mensaje !!!!!!!!!!!!!
// 4) Crear registro TDD y agregar lista de sectores				
				entradaTDD = generar_nodo();
				entradaTDD->descriptor = desc_tdd;
				strcpy(entradaTDD->nombreVDA, vda);
				strcpy(entradaTDD->nombreArchivo, nombre_archivo);
				entradaTDD->tipoApertura=tipo_de_apertura-'0';
				i=0;
				strcpy(mensaje->Payload, "1024,2,34,567,8900");
				printf("llega al strcpy\n");
				//guardo el tamanio del archivo en string
				while(mensaje->Payload[i] != ',' && mensaje->Payload[i] != '\0'){
					tamanio_archivo[i]=mensaje->Payload[i];
					i++;
				}
				tamanio_archivo[i]='\0';
				printf("tamaño archivo char %s",tamanio_archivo);
				//paso el tamaño del archivo a long y lo guardo en la tdd
				sscanf(tamanio_archivo, "%ld", &entradaTDD->size);
				printf("tamaño del archivo %ld\n", entradaTDD->size);
				
				strcpy(entradaTDD->buffer, "\0");
				j=i;
				//cuanto la cantidad de sectores
				while(mensaje->Payload[j] != '\0'){
					if(mensaje->Payload[j] == ','){
						cantidad_sectores++;
					}
					j++;
				}
				printf("cantidad de sectores %d\n", cantidad_sectores);
				i++;
				j=0;
				for(j; j<cantidad_sectores;j++){
					//agrego sectores de a uno
					k=0;
					while(mensaje->Payload[i] != '\0' && mensaje->Payload[i] != ','){
						sector_aux[k] = mensaje->Payload[i];
						i++;
						k++;
					}
					sector_aux[k]='\0';
					sscanf(sector_aux,"%u",&numero_sector);
					printf("voy a agregar sector %u\n", numero_sector);
					entradaTDD->sector=generar_insertar_sector(entradaTDD->sector, numero_sector);
					printf("lo agregue");
					i++;
				}
				entradaTDD->siguiente = NULL;
				Tdd = agregar_nodo_TDD(Tdd, entradaTDD);
				break;
				case '1':
// en caso de ESCRITURA
// 3) eliminar archivo del fss
				mensaje->PayloadDescriptor = '4';
				strcpy(mensaje->Payload, "eliminarArchivo(");
				strcat(mensaje->Payload, vda);
				strcat(mensaje->Payload, ",");
				strcat(mensaje->Payload, nombre_archivo);
				strcat(mensaje->Payload, ")");
				mensaje->PayloadLenght = strlen(mensaje->Payload);
				send(Fss, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);
				//no se si hay una respuesta del FSS
// 4) crear archivo en el fss
				mensaje->PayloadDescriptor = '5';
				strcpy(mensaje->Payload, "crearArchivo(");
				strcat(mensaje->Payload, vda);
				strcat(mensaje->Payload, ",");
				strcat(mensaje->Payload, nombre_archivo);
				strcat(mensaje->Payload, ")");
				mensaje->PayloadLenght = strlen(mensaje->Payload);
				send(Fss, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);
				//no se si hay una resp del FSS
// 5) crear regisrto en la TDD
				entradaTDD = generar_nodo();
				entradaTDD->descriptor = desc_tdd;
				strcpy(entradaTDD->nombreVDA, vda);
				strcpy(entradaTDD->nombreArchivo, nombre_archivo);
				entradaTDD->tipoApertura =tipo_de_apertura-'0';
				entradaTDD->size= 0;
				strcpy(entradaTDD->buffer, "\0");
				entradaTDD->sector = NULL;
				entradaTDD->siguiente = NULL;
				Tdd = agregar_nodo_TDD(Tdd, entradaTDD);
				break;
				case '2':
// en caso de ELIMINAR
// 3) crear registro TDD
				entradaTDD = generar_nodo();
				entradaTDD->descriptor = desc_tdd;
				strcpy(entradaTDD->nombreVDA, vda);
				strcpy(entradaTDD->nombreArchivo, nombre_archivo);
				entradaTDD->tipoApertura = tipo_de_apertura-'0';
				entradaTDD->size= 0;
				strcpy(entradaTDD->buffer, "\0");
				entradaTDD->sector = NULL;
				entradaTDD->siguiente = NULL;
				Tdd = agregar_nodo_TDD(Tdd, entradaTDD);
				break;				
			}
			
			mensaje->PayloadDescriptor='1';
			strcpy(mensaje->DescriptorID,desc);
			sprintf(mensaje->Payload,"%u",desc_tdd);
	                mensaje->PayloadLenght = strlen(mensaje->Payload);

			send(socket, mensaje, 21+mensaje->PayloadLenght+1, 0); //devuelvo al FTP el descriptor del archivo que abrio, no necesario ahora //PROTOCOLOO!!! DALEE PAPAAA
		}else{
			//no existe el archivo
		}
	}else{
		//ya esta abierto
	}
	
	return Tdd;
	
}



//          READ                 READ                     READ

nodoTDD* sys_read(nodoTDD* Tdd, nodo_lista_sockets* lista_sockets, char* argumento, int socket, char* desc){
	unsigned int desc_tdd;
	unsigned int sector1, sector2;
	nodoTDD* nodo_aux;
	int socket_vda;
	MPS_Package *mensaje;
	char buffer[1072];
	char sectores[1025];
	char sect1[6];
	char sect2[6];
	
	mensaje = (MPS_Package*) malloc(sizeof(MPS_Package));
	strcpy(mensaje->DescriptorID, desc);
//	nodo_aux = (nodoTDD*) malloc(sizeof(nodoTDD));
	nodo_aux = Tdd;
	sscanf(argumento, "%u", &desc_tdd);

// 1) vverificar que el archivo este abierto en modo lectura
	if(buscar_tipo_apertura(Tdd, desc_tdd)==0){
	// se abrio para lectura
//2) obtener sectores de la vda		
		//busco el nodo de la tdd
		while(nodo_aux->descriptor!=desc_tdd){
			nodo_aux=nodo_aux->siguiente;
		}
		//busco el socket de la vda
		socket_vda = buscar_socket(lista_sockets, nodo_aux->nombreVDA);
		sector1 = -1;
		if(nodo_aux->sector !=NULL){
			sector1 = (nodo_aux->sector)->sector;
			sector2= -1;
			nodo_aux->sector = (nodo_aux->sector)->punteroSector;
		}
		if(nodo_aux->sector !=NULL){
			sector2 = (nodo_aux->sector)->sector;
			nodo_aux->sector = (nodo_aux->sector)->punteroSector;
		}
		if(sector1 == -1){
		//no hay sectores
			strcpy(nodo_aux->buffer, "\0");
		}
		else{
		//hay sectores
			sprintf(sect1,"%d",sector1);
			sprintf(sect2,"%d",sector2);
			mensaje->PayloadDescriptor = '2';
			strcat(mensaje->Payload, "getSectores(");
			strcat(mensaje->Payload, sect1);
			strcat(mensaje->Payload, ",");
			strcat(mensaje->Payload, sect2);
			strcat(mensaje->Payload, ")");
			mensaje->PayloadLenght = strlen(mensaje->Payload);
			send(socket_vda, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);
			recv(socket_vda, buffer, sizeof(buffer),0);
			print_pkg((MPS_Package *)buffer);
// 3) enviar buffer al ftp
			mensaje = (MPS_Package*)buffer;
			strcpy(sectores,mensaje->Payload);
			rellenar_sectores(sectores);
			mensaje->PayloadDescriptor = '1';
			strcpy(mensaje->Payload, sectores);
			mensaje->PayloadLenght = 1024;
			send(socket, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);
		}
	}
	else{
	//no se abrio para lectura
		mensaje->PayloadDescriptor = '0';
		strcpy(mensaje->Payload, "No se abrio para lectura");
		mensaje->PayloadLenght = strlen("No se abrio para lectura");
		send(socket, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);
	}

	return Tdd;
}


//          WRITE                 WRITE                     WRITE

nodoTDD* sys_write(nodoTDD* Tdd, nodo_lista_sockets* lista_sockets, char* argumento, int socket, char* desc){
	unsigned int desc_tdd;
	char sectores[1025];
	nodoTDD* nodo_aux;
	long tamanio_archivo;
	MPS_Package *mensaje;
	int respuesta_sys_flush;
	char desctdd[10];
	int i=0;
	int j=0;
	
	mensaje = (MPS_Package*) malloc(sizeof(MPS_Package));
	strcpy(mensaje->DescriptorID, desc);
	nodo_aux = (nodoTDD*) malloc(sizeof(nodoTDD));
	nodo_aux = Tdd;
	//sscanf(argumento, "%u,%s", desc_tdd, sectores);
	while(argumento[i]!=','){
		desctdd[i]=argumento[i];
		i++;
	}
	desctdd[i]='\0';
	sscanf(desctdd, "%u", &desc_tdd);
	i++;
	while(argumento[i]!='\0'){
		sectores[j]=argumento[i];
		i++;
		j++;
	}
	sectores[i]='\0';
	
	tamanio_archivo = strlen(sectores);
	
	
	
// 1) vverificar que el archivo este abierto en modo escritura
	if(buscar_tipo_apertura(Tdd, desc_tdd)==1){
	// se abrio para escritura
// 2) escribir en el buffer de la Tdd		
		//busco el nodo de la tdd
		while(nodo_aux->descriptor!=desc_tdd){
			nodo_aux=nodo_aux->siguiente;
		}
		rellenar_sectores(sectores);
		strcpy(nodo_aux->buffer, sectores);
// 3) llamar a sysflush
		respuesta_sys_flush = sys_flush(Tdd, lista_sockets, socket, desc, desc_tdd);
// 4) actualizar tamanio en la TDD
		if(respuesta_sys_flush){
			nodo_aux->size += tamanio_archivo;
// 5) responder al ftp
			mensaje->PayloadDescriptor = '1';
			strcpy(mensaje->Payload, "sys_write exitosa");
			mensaje->PayloadLenght = strlen("sys_write exitosa");
			send(socket, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);
		}
		else{
			exit(1);
		}
	}
	else{
	// no se abrio para escritura
		mensaje->PayloadDescriptor = '0';
		strcpy(mensaje->Payload, "No se abrio para escritura");
		mensaje->PayloadLenght = strlen("No se abrio para escritura");
		send(socket, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);
	}
	return(Tdd);
}



//          FLUSH                 FLUSH                     FLUSH

int sys_flush(nodoTDD* Tdd, nodo_lista_sockets* lista_sockets, int socket, char* desc, unsigned int desc_tdd){
	struct infoGrabar{
		long dir1;
		char dato1[512];
		long dir2;
		char dato2[512];	
	};
	struct infoGrabar *estructura;
	char sectores[12];
	char sector1[6];
	char sector2[6];
	int FSS, i, j=0;
	MPS_Package *mensaje;
	char buffer[1200];
	int socket_vda;
	//char estructura_aux[1032];
	
	estructura = (struct infoGrabar*) malloc(sizeof(struct infoGrabar));
	mensaje = (MPS_Package*) malloc(sizeof(MPS_Package));
	strcpy(mensaje->DescriptorID, desc);
	i=0;
	FSS = buscar_socket(lista_sockets, "FSS");
	while(Tdd->descriptor != desc_tdd){
		Tdd = Tdd->siguiente;
	}
// 1) vverificar que el archivo este abierto en modo escritura
	if(buscar_tipo_apertura(Tdd, desc_tdd)==1){
	// se abrio para escritura
// 2) solicitar dos sectore slibres al FSS de la VDA correspondiente
		mensaje->PayloadDescriptor = '8';
		strcat(mensaje->Payload, "dosSectoresLibres(");
		strcat(mensaje->Payload, Tdd->nombreVDA);
		strcat(mensaje->Payload, ")");
		mensaje->PayloadLenght = strlen(mensaje->Payload);
		send(FSS, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);
		recv(FSS, buffer, sizeof(buffer), 0);
		mensaje = (MPS_Package*) buffer;
		strcpy(sectores, mensaje->Payload);
// 3) mandar el putSectores al VDA
		//lleno la estructura
		while(sectores[i]!=','){
			sector1[i]=sectores[i];
			i++;
		}
		sector1[i]='\0';
		i++;
		while(sectores[i]!='\0'){
			sector2[j]=sectores[i];
			i++;
			j++;
		}
		sector2[j]='\0';
		
		
		sscanf(sector1, "%ld",&estructura->dir1);
		sscanf(sector2, "%ld",&estructura->dir2);
		for(i=0;i<512;i++){
			estructura->dato1[i] = Tdd->buffer[i];
		}
		for(i=512;i<1024;i++){
			estructura->dato2[i-512] = Tdd->buffer[i];
		}
		//mando el putSectores
		mensaje->PayloadDescriptor = '3';
		strcpy(mensaje->Payload, "putSectores(");		
		for(i=0;i<1033;i++){
			mensaje->Payload[i+12] = ((char*)estructura)[i];
		}
		strcpy(mensaje->Payload+1044, ")");
		mensaje->PayloadLenght = 1045;
		socket_vda = buscar_socket(lista_sockets, Tdd->nombreVDA);
		send(socket_vda, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);
// 4) asignar ambos sectores al archivo en el FSS
		Tdd->sector=generar_insertar_sector(Tdd->sector, estructura->dir1);
		Tdd->sector=generar_insertar_sector(Tdd->sector, estructura->dir2);
		mensaje->PayloadDescriptor = '3';
		strcpy(mensaje->Payload, "asignarSectores(");
		strcat(mensaje->Payload, Tdd->nombreVDA);
		strcat(mensaje->Payload, ",");
		strcat(mensaje->Payload, Tdd->nombreArchivo);
		strcat(mensaje->Payload, ",");
		strcat(mensaje->Payload, sectores);
		strcat(mensaje->Payload, ")");
		mensaje->PayloadLenght = strlen(mensaje->Payload);
		send(FSS, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);
	}
	else{
		//no se abrio para escritura
		printf("Error sys_flush: el archivo no se abrió para escritura");
		return(0);
	}
// 5)responder el exito de la operacion
	return(1);
}



//          CLOSE                 CLOSE                    CLOSE

nodoTDD* sys_close(nodoTDD* Tdd, nodo_lista_sockets* lista_sockets, char* argumento, int socket, char* desc){
	nodoTDD* nodo_aux;
	MPS_Package *mensaje;
	unsigned int desc_tdd;
	char tamanio_archivo[20];
	int FSS;
	
	FSS = buscar_socket(lista_sockets, "FSS");
	sscanf(argumento, "%u", &desc_tdd);
//	desc_tdd=0;
	mensaje = (MPS_Package*) malloc(sizeof(MPS_Package));
	strcpy(mensaje->DescriptorID, desc);
//	nodo_aux = (nodoTDD*) malloc(sizeof(nodoTDD));
	nodo_aux = Tdd;

// 1) verificar que el archivo este abierto en la TDD
	//busco el nodo de la TDD
	while((nodo_aux->descriptor != desc_tdd) && nodo_aux != NULL){
		nodo_aux = nodo_aux->siguiente;
	}
	if(nodo_aux != NULL){
	// el archivo está abierto
		if(buscar_tipo_apertura(Tdd, desc_tdd)==0){
//LECTURA
// 2) ir directo a 3
		}
		if(buscar_tipo_apertura(Tdd, desc_tdd)==1){
//ESCRITURA
// 2) enviar al FSS el tamaño final del archivo
			sprintf(tamanio_archivo,"%l",nodo_aux->size);
			mensaje->PayloadDescriptor = '6';
			strcat(mensaje->Payload, "actualizarTamanio(");
			strcat(mensaje->Payload, nodo_aux->nombreVDA);
			strcat(mensaje->Payload, ",");
			strcat(mensaje->Payload, nodo_aux->nombreArchivo);
			strcat(mensaje->Payload, ",");
			strcat(mensaje->Payload, tamanio_archivo);
			strcat(mensaje->Payload, ")");
			mensaje->PayloadLenght = strlen(mensaje->Payload);
			send(FSS, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);
		}
		if(buscar_tipo_apertura(Tdd, desc_tdd)==2){
//ELIMINAR
// 2) solicitar al FSS que elimine el archivo
			mensaje->PayloadDescriptor = '4';
			strcat(mensaje->Payload, "eliminarArchivo(");
			strcat(mensaje->Payload, nodo_aux->nombreVDA);
			strcat(mensaje->Payload, ",");
			strcat(mensaje->Payload, nodo_aux->nombreArchivo);
			strcat(mensaje->Payload, ")");
			mensaje->PayloadLenght = strlen(mensaje->Payload);
			send(FSS, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);
		}
		
// 3) eliminar el registro de la TDD
		Tdd = eliminar_nodo_TDD(Tdd, desc_tdd);
	}
	else{
	//el archivo no está abierto
		mensaje->PayloadDescriptor = '0';
		strcpy(mensaje->Payload, "No esta abierto");
		mensaje->PayloadLenght = strlen("No esta abierto");
		send(socket, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);
	}
	
	return(Tdd);
}


//          LIST                 LIST                    LIST

nodoTDD* sys_list(nodoTDD* Tdd, nodo_lista_sockets* lista_sockets, char* argumento, int socket, char* desc){
	char vdas_montadas[500];
	int corrector;
	MPS_Package *mensaje;
	char vda_pedida[5];
	int i;
	int FSS;
	char buffer[1072];
	char lista_archivos_tamanio[1050];
	
	FSS = buscar_socket(lista_sockets, "FSS");
	mensaje = (MPS_Package*) malloc(sizeof(MPS_Package));
	strcpy(mensaje->DescriptorID, desc);
	
	if(strlen(argumento)==1){
//SI EL PARAMETRO ES /
//listar vdas montados
		strcpy(vdas_montadas, "\0");
		while(lista_sockets!=NULL){
			if(! strncmp("VDA", lista_sockets->nombre,3)){
				if(!strcmp(lista_sockets->estado,"montado")){
					strcat(vdas_montadas, lista_sockets->nombre);
					strcat(vdas_montadas, ",");
				}
			}
			lista_sockets=lista_sockets->puntero_siguiente;
		}
		if(corrector=strlen(vdas_montadas)>1){
			vdas_montadas[corrector] = '\0';
		}
		mensaje->PayloadDescriptor = '1';
		strcat(mensaje->Payload, vdas_montadas);
		mensaje->PayloadLenght = strlen(mensaje->Payload);
		send(socket, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);	
	}
	else{
// SI EL PARAMETRO ES /VDAx
// enviar lista de archivos con su tamaño
		i=0;
		for(i;i<6;i++){
			vda_pedida[i]=argumento[i+1];
		}
		//pedirle al fss
		mensaje->PayloadDescriptor = '9';
		strcat(mensaje->Payload, "listarDirectorio(");
		strcat(mensaje->Payload, vda_pedida);
		strcat(mensaje->Payload, ")");
		mensaje->PayloadLenght = strlen(mensaje->Payload);
		send(FSS, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);
		recv(FSS, buffer, sizeof(buffer), 0);
		//mandarle al ftp
		mensaje = (MPS_Package*)buffer;
		mensaje->PayloadDescriptor = '1';
		mensaje->PayloadLenght = strlen(mensaje->Payload);
		send(socket, (char *)mensaje, 21+mensaje->PayloadLenght+1, 0);
	}
	return(Tdd);
}




int buscar_socket(nodo_lista_sockets* lista_sockets, char* busqueda){
	while(lista_sockets!= NULL){
		if(!strcmp(lista_sockets->nombre, busqueda))
			return lista_sockets->socket;
		else
			lista_sockets = lista_sockets->puntero_siguiente;
	}
	return -1; //error, no hay busqueda
}

nodoTDD* generar_nodo(){
	nodoTDD *nodo_nuevo;
	nodo_nuevo = (nodoTDD*) malloc(sizeof(nodoTDD));
	return nodo_nuevo;
}

nodoTDD* agregar_nodo_TDD(nodoTDD* Tdd, nodoTDD* entradaTDD){
	nodoTDD *aux;
	aux = (nodoTDD*) malloc(sizeof(nodoTDD));
	aux=Tdd;
	
	if(Tdd == NULL){
		Tdd = entradaTDD;
	}else{
		while(aux->siguiente != NULL)
			aux=aux->siguiente;
		aux->siguiente = entradaTDD;
	}	
	return (Tdd);
}

unsigned int descriptor_nuevo(nodoTDD* Tdd){
	if(Tdd == NULL){
		return 0;
	}
	else{
		while(Tdd->siguiente!=NULL){
			Tdd=Tdd->siguiente;
		}
		return(Tdd->descriptor + 1);
	}
}

int buscar_tipo_apertura(nodoTDD* Tdd, unsigned int desc_tdd){
	while(Tdd->descriptor != desc_tdd){
		Tdd = Tdd->siguiente;
	}
	return Tdd->tipoApertura;
}

int rellenar_sectores(char* sectores){
	int i=0;
	
	while(sectores[i] != '\0'){
		i++;
	}
	for(i; i<1025; i++){
		sectores[i] = '\0';
	}
	return(0);
}

nodoTDD* eliminar_nodo_TDD(nodoTDD* Tdd, unsigned int desc_tdd){
	nodoTDD *aux, *anterior;
	aux = (nodoTDD*) malloc(sizeof(nodoTDD));
	anterior = (nodoTDD*) malloc(sizeof(nodoTDD));
	aux=Tdd;
	
	if(Tdd->descriptor == desc_tdd){
		Tdd = Tdd->siguiente;
	}
	else{
		anterior = Tdd;
		aux = aux->siguiente;
		while(aux->descriptor != desc_tdd){
			anterior = aux;
			aux = aux->siguiente;
		}
		anterior->siguiente = aux->siguiente;
	}
	free(aux);
	return(Tdd);
}
	
Sector* generar_insertar_sector(Sector* lista_tdd, unsigned int numero_sector){
	Sector *nodo_nuevo;
	Sector *aux;
	nodo_nuevo = (Sector*) malloc(sizeof(Sector));
	
	aux=lista_tdd;
	nodo_nuevo->sector = numero_sector;
	nodo_nuevo->punteroSector=NULL;
	if(lista_tdd == NULL){
		lista_tdd = nodo_nuevo;
	}else{
		while(aux->punteroSector != NULL)
			aux=aux->punteroSector;
		aux->punteroSector = nodo_nuevo;
	}	
	return lista_tdd;
}
	
	
	
	

nodoTDD* mount(nodoTDD* Tdd, nodo_lista_sockets* lista_sockets, char* argumento, int socket, char* desc){

	printf("entre a mount\n");
	printf("%s\n",lista_sockets->estado);

	while(lista_sockets!=NULL&&strcmp(lista_sockets->nombre,argumento)){
		lista_sockets=lista_sockets->puntero_siguiente;
		}
	if (lista_sockets==NULL){
		printf("no encontre la VDA \n");
		}
	else{ 
		printf("%s montada!!",argumento);
		strcpy(lista_sockets->estado,"montado");
		}

	return (Tdd);
}

nodoTDD* umount(nodoTDD* Tdd, nodo_lista_sockets* lista_sockets, char* argumento, int socket, char* desc){

        printf("entre a umount\n");
        printf("%s\n",lista_sockets->estado);

        while(lista_sockets!=NULL&&strcmp(lista_sockets->nombre,argumento)){
                lista_sockets=lista_sockets->puntero_siguiente;
                }
        if (lista_sockets==NULL){
                printf("no encontre la VDA \n");
                }
        else{
                printf("%s desmontada!!",argumento);
                strcpy(lista_sockets->estado,"nomontado");
                }

        return (Tdd);
}

nodoTDD* tdd_dump(nodoTDD* Tdd, nodo_lista_sockets* lista_sockets, char* argumento, int socket, char* desc){

	Sector* psector;
	nodoTDD* ptdd;
        printf("entre a tdd dump\n");
        printf("-----------------------------------------\n");

	ptdd = Tdd;
	while (ptdd!=NULL){
//		printf("\n");

		printf("descriptor: %u\n",ptdd->descriptor);
		printf("NombreVDA: %s\n",ptdd->nombreVDA);
		printf("nombreArchivo %s\n",ptdd->nombreArchivo);
		printf("tipoApertura: %d\n",ptdd->tipoApertura);
		printf("size: %u\n",ptdd->size);
		printf("buffer: %s\n",ptdd->buffer);
		printf("sectores: ");
		psector=ptdd->sector;
		while(psector!=NULL){
			printf("%d ",psector->sector);
			psector=psector->punteroSector;
		}
		printf("\n");
		printf("-----------------------------------------\n");
		ptdd=ptdd->siguiente;
	}

//        printf("-----------------------------------------\n");

        return (Tdd);
}

nodoTDD* ls(nodoTDD* Tdd, nodo_lista_sockets* lista_sockets, char* argumento, int socket, char* desc){

        printf("entre a ls\n");

        return (Tdd);
}

nodoTDD* format(nodoTDD* Tdd, nodo_lista_sockets* lista_sockets, char* argumento, int socket, char* desc){

        printf("entre a format\n");

        return (Tdd);
}

nodoTDD* md5sum(nodoTDD* Tdd, nodo_lista_sockets* lista_sockets, char* argumento, int socket, char* desc){

        printf("entre a md5sum\n");

        return (Tdd);
}

