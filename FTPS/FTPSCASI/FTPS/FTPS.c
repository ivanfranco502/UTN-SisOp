#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include <stdio.h>
#include <process.h>   /* _beginthreadex, _endthreadex */
#include <string.h>
#include "funcionesftp.h"


#define CANTIDAD_CLIENTES 65535
#define SOCKET_MAX_BUFFER 100

WSADATA wsaData;




int threadsFinalizados[CANTIDAD_CLIENTES]; /*Empiezan en cero y se ponen a uno cuando finaliza el thread */
unsigned threadID[CANTIDAD_CLIENTES];
t_command_handler vector_comandos[10];




void inicializarVectorDeThreads(){
	// Inicializa en cero threadsFinalizados[]
	int i=0;
	for (i; i<CANTIDAD_CLIENTES; i++)
		threadsFinalizados[i]=0;
}


unsigned __stdcall threadDeDatos( void* pArguments ){
    // Thread de datos

	HANDLE  heapDatos = HeapCreate(HEAP_NO_SERIALIZE, 1024*1024, 0);
	SOCKET descriptorD;
	int clienteDatos;
	int addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in *local_address = HeapAlloc(heapDatos, 0, addrlen);
    struct sockaddr_in *remote_address = HeapAlloc(heapDatos, 0, addrlen);
	char buffer[SOCKET_MAX_BUFFER];
	reg_cliente *datos_cliente  = HeapAlloc(heapDatos, HEAP_NO_SERIALIZE, sizeof(reg_cliente));
	datos_cliente = (reg_cliente*) pArguments;


    local_address->sin_family = AF_INET;
	local_address->sin_addr.s_addr=INADDR_ANY;
	local_address->sin_port = htons (5300);

	//printf("%d\n",descriptor= socket(AF_INET, SOCK_STREAM, 0));
	printf("%d\n",descriptorD= socket(AF_INET, SOCK_STREAM, 0));
	bind (descriptorD,(struct sockaddr *) local_address, addrlen);
	listen(descriptorD,100);


	// DECIRLE AL DE COMANDOS QUE MANDE EL 227

	
	printf("%d\n", clienteDatos = accept(descriptorD, (struct sockaddr *)remote_address, (void*)&addrlen));

	// ESPERAR QUE ME DIGA QUE HACER (si es list, retr, stor, dele)

	if(clienteDatos!=INVALID_SOCKET){
		// si es un list
		send(clienteDatos,"-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo1.txt\r\n",
				 strlen("-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo1.txt\r\n"),0);
		send(clienteDatos,"-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo2.txt\r\n",
				 strlen("-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo2.txt\r\n"),0);
		send(clienteDatos,"drwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 directorio1\r\n",
				 strlen("drwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 directorio1\r\n"),0);
	
	// DECIRLE AL DE COMANDOS QUE MANDE EL 226

		
		//si es un el retr
		send(clienteDatos, "Contenido del archivo descargado", 
				 strlen("Contenido del archivo descargado"),0);
			
	
		closesocket(clienteDatos);
		closesocket(descriptorD);
	}else
		printf("No se conectaron al thread de datos\n");
	HeapDestroy(heapDatos);
    _endthreadex( 0 );
} 

unsigned __stdcall threadClienteNuevo( void* pArguments ){
    // Thread de comandos
	
	HANDLE  heap = HeapCreate(HEAP_NO_SERIALIZE, 1024*1024, 0);
	HANDLE hThreadDatos;
	int corrector;
	char buffer[100],
		 comando[5],
		 argumento[50];
	reg_cliente *datos_cliente  = HeapAlloc(heap, HEAP_NO_SERIALIZE, sizeof(datos_cliente));
	datos_cliente = (reg_cliente*) pArguments;
	//int (*puntero)(char*, reg_cliente*);

	
	
	send(datos_cliente->socket_comando,"220 POWER\r\n", strlen("220 POWER\r\n"),0);
    corrector=recv(datos_cliente->socket_comando,buffer,SOCKET_MAX_BUFFER,0);
    buffer[corrector]='\0';
	
	strcpy(comando, obtenerComando(buffer));
	strcpy(argumento, obtenerParametro(buffer));
	while( (strcmp(comando, "bye")) && (strcmp(comando, "quit")) && (strcmp(comando, "exit")) ){
		while (strcmp(comando,"PASV")){
			command_handler(vector_comandos, comando, argumento, datos_cliente);
			corrector=recv(datos_cliente->socket_comando,buffer,SOCKET_MAX_BUFFER,0);
			buffer[corrector]='\0';
			strcpy(comando, obtenerComando(buffer));
			strcpy(argumento, obtenerParametro(buffer));
		}

		printLog("Thread Cmd","1",threadID[datos_cliente->socket_comando],"INFO","Conexion al Thread de Datos");
		hThreadDatos = (HANDLE) _beginthreadex(NULL,0, &threadDeDatos, (void*) datos_cliente, 0, NULL);
	
	    // ESPERAR QUE DATOS ME DIGA QUE ABRIO EL SOCKET
		
		while ( /*no termina el PASV*/ ){
			command_handler(vector_comandos, comando, argumento, datos_cliente);
			corrector=recv(datos_cliente->socket_comando,buffer,SOCKET_MAX_BUFFER,0);
			buffer[corrector]='\0';
			strcpy(comando, obtenerComando(buffer));
			strcpy(argumento, obtenerParametro(buffer));
		}



		CloseHandle(hThreadDatos);
		printLog("Thread Cmd", "1", threadID[datos_cliente->socket_comando], "INFO", "Desconexion al Thread de Datos");
		send(datos_cliente->socket_comando,"226 Transfer Complete\r\n", strlen("226 Transfer Complete\r\n"),0);
	}

	HeapDestroy(heap);
	threadsFinalizados[datos_cliente->socket_comando]=1;
    _endthreadex( 0 );
    
} 

int main(){ 
    SOCKET descriptor, socketAux;
	HANDLE hThread[CANTIDAD_CLIENTES];
	reg_cliente datos_cliente[CANTIDAD_CLIENTES];
	reg_cliente *reg_aux;
	int a, addrlen = sizeof(struct sockaddr_in);
	unsigned i;
	struct sockaddr_in *local_address = HeapAlloc(GetProcessHeap(), 0, addrlen);
    struct sockaddr_in *remote_address = HeapAlloc(GetProcessHeap(), 0, addrlen);
	
	printLog("Main FTPS","0","0","INFO","Archivo de Configuracion");
	paraElMain(vector_comandos);
	reg_aux = (reg_cliente *) HeapAlloc(GetProcessHeap(), 0, sizeof(reg_cliente));
	

	if ((a = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		printf("WSAStartup() devolvio error nro %d\n", a );
		return 255;
	}
	
	
    local_address->sin_family = AF_INET;
	local_address->sin_addr.s_addr=INADDR_ANY;
	local_address->sin_port = htons (21);

	printf("%d\n",descriptor= socket(AF_INET, SOCK_STREAM, 0));
	bind (descriptor,(struct sockaddr *) local_address, addrlen);
	listen(descriptor,100);

    printf("Escuchando puerto\n");
	
	inicializarVectorDeThreads();
	
	while(1){
		if((socketAux = accept (descriptor, (struct sockaddr *)remote_address, (void*)&addrlen))!= -1){
			datos_cliente[socketAux].socket_comando=socketAux;
			reg_aux->socket_comando=socketAux;
			hThread[socketAux] = (HANDLE) _beginthreadex(NULL,0, &threadClienteNuevo, (void*) reg_aux, 0, &threadID[socketAux]);
			printLog("New Client","0",threadID[socketAux],"INFO","Conexion Nuevo cliente al puerto ftp");

		}

		for (i=0; i<CANTIDAD_CLIENTES;i++){
			if (threadsFinalizados[i]){
				printLog("Disconnect client","0",threadID[i], "INFO", "Desconexion cliente al puerto ftp");
				closesocket(i);
				CloseHandle(hThread[i]);
				threadsFinalizados[i]=0;
			}

		}
	}
	return 0;
}