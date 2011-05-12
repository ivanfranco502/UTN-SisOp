#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include <stdio.h>
#include <process.h>   /* _beginthreadex, _endthreadex */
#include <string.h>
#include "funcionesftp.h"


#define CANTIDAD_CLIENTES 65535
#define SOCKET_MAX_BUFFER 5000

WSADATA wsaData;




int threadsFinalizados[CANTIDAD_CLIENTES]; /*Empiezan en cero y se ponen a uno cuando finaliza el thread */
unsigned threadID[CANTIDAD_CLIENTES];
t_command_handler vector_comandos[11];




void inicializarVectorDeThreads(){
	// Inicializa en cero threadsFinalizados[]
	unsigned i=0;
	for (i; i<CANTIDAD_CLIENTES; i++)
		threadsFinalizados[i]=0;
}




unsigned __stdcall threadClienteNuevo( void* pArguments ){
    // Thread de comandos
	
	HANDLE  heap = HeapCreate(HEAP_NO_SERIALIZE, 1024*1024, 0);
	int corrector;
	char buffer[100],
		 comando[5],
		 argumento[50];
	SOCKET *aux = HeapAlloc(heap, HEAP_NO_SERIALIZE, sizeof(SOCKET));
	reg_cliente *datos_cliente  = HeapAlloc(heap, HEAP_NO_SERIALIZE, sizeof(reg_cliente));
	aux = (SOCKET*) pArguments;
	datos_cliente->socket_comando = *aux;
	strcpy(datos_cliente->current_path, "/");
	datos_cliente->threadID=threadID[datos_cliente->socket_comando];
	//HeapFree(heap, HEAP_NO_SERIALIZE, aux);
	//int (*puntero)(char*, reg_cliente*);
	datos_cliente->evento1 = CreateEvent(NULL, FALSE, FALSE, NULL);
	datos_cliente->evento2 = CreateEvent(NULL, FALSE, FALSE, NULL);
	

	printLog("Thread Comandos","1",datos_cliente->threadID,"INFO","Conexion al Thread de Comandos");


	//printf("%d", datos_cliente->socket_comando);

	send(datos_cliente->socket_comando,"220 POWER\r\n", strlen("220 POWER\r\n"),0);
    corrector=recv(datos_cliente->socket_comando,buffer,SOCKET_MAX_BUFFER,0);
    buffer[corrector]='\0';
	
	

	strcpy(comando, obtenerComando(buffer));
	strcpy(argumento, obtenerParametro(buffer));
	printf("%s ", comando);
	printf("%s", argumento);
	
	while( corrector != SOCKET_ERROR ){  // espero que cierre sesion
		command_handler(vector_comandos, comando, argumento, datos_cliente);
		corrector=recv(datos_cliente->socket_comando,buffer,SOCKET_MAX_BUFFER,0);
		buffer[corrector]='\0';
		strcpy(comando, obtenerComando(buffer));
		strcpy(argumento, obtenerParametro(buffer));
		printf("%s ", comando);
		printf("%s\n", argumento);
	}
	
	printLog("Thread Comandos","1",datos_cliente->threadID,"INFO","Desconexion al Thread de Comandos");

	CloseHandle(datos_cliente->evento1);
	CloseHandle(datos_cliente->evento2);
	threadsFinalizados[datos_cliente->socket_comando] = 1;
    HeapDestroy(heap);
	_endthreadex( 0 );
    
} 

int main(){ 
    
	
	SOCKET descriptor;
	SOCKET *socketAux;
	HANDLE hThread[CANTIDAD_CLIENTES];
	int a, addrlen = sizeof(struct sockaddr_in);
	unsigned i;
	struct sockaddr_in *local_address = HeapAlloc(GetProcessHeap(), 0, addrlen);
    struct sockaddr_in *remote_address = HeapAlloc(GetProcessHeap(), 0, addrlen);
	socketAux = HeapAlloc(GetProcessHeap(),HEAP_NO_SERIALIZE, sizeof(SOCKET));
	

	printLog("Main FTPS","0",0,"INFO","Archivo de Configuracion");
	
	

	paraElMain(vector_comandos);
	
	

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
		if((*socketAux = accept (descriptor, (struct sockaddr *)remote_address, (void*)&addrlen))!= -1){
			hThread[*socketAux] = (HANDLE) _beginthreadex(NULL,0, &threadClienteNuevo, (void*) socketAux, 0, &threadID[*socketAux]);
			//printf("%d", *socketAux);
			printLog("New Client","0",threadID[*socketAux],"INFO","Conexion Nuevo cliente al puerto ftp");

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