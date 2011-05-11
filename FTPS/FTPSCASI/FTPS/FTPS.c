#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include <stdio.h>
#include <process.h>   /* _beginthreadex, _endthreadex */
#include <string.h>
#include "funcionesftp.h"


#define CANTIDAD_CLIENTES 65535
#define SOCKET_MAX_BUFFER 1000

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


unsigned __stdcall threadDeDatos( void* pArguments ){
    // Thread de datos

	HANDLE  heapDatos = HeapCreate(HEAP_NO_SERIALIZE, 1024*1024, 0);
	SOCKET descriptorD;
	int clienteDatos;
	int addrlen = sizeof(struct sockaddr_in);
	int cantidad_de_bytes;
	unsigned puerto = 5300;
	struct sockaddr_in *local_address = HeapAlloc(heapDatos, 0, addrlen);
    struct sockaddr_in *remote_address = HeapAlloc(heapDatos, 0, addrlen);
	char buffer[SOCKET_MAX_BUFFER];
	reg_cliente *datos_cliente  = HeapAlloc(heapDatos, HEAP_NO_SERIALIZE, sizeof(reg_cliente));
	datos_cliente = (reg_cliente*) pArguments;
	
	printLog("Thread Datos","2",threadID[datos_cliente->socket_comando],"INFO","Conexion al Thread de Datos");
    local_address->sin_family = AF_INET;
	local_address->sin_addr.s_addr=INADDR_ANY;
	
	do{
		local_address->sin_port = htons (puerto);
		descriptorD= socket(AF_INET, SOCK_STREAM, 0);
		puerto++;
	} while((bind (descriptorD,(struct sockaddr *) local_address, addrlen)) == SOCKET_ERROR); // intento bindear algun puerto
	
	printf("bindeo exitoso!!\n");

	datos_cliente->puerto_datos= puerto-1;

	listen(descriptorD,100);

	SetEvent(datos_cliente->evento1);
	printf("me pongo a aceptar el 5300");	
	printf("%d\n", clienteDatos = accept(descriptorD, (struct sockaddr *)remote_address, (void*)&addrlen));
	printf("acepte!!\n");
	
	WaitForSingleObject(datos_cliente->evento2, INFINITE); // espero que el thread de comandos me diga que ya puedo mandar o recibir
		
	printf("soy el Thread de datos y voy a analizar..\n");
	switch(datos_cliente->envio_o_recibo){
		case 'E':
			printf("voy a mandar..\n");
			send (clienteDatos, datos_cliente->buffer, strlen(datos_cliente->buffer), 0);	
			printf("mande..\n");
			break;
		case 'R':
			recv (clienteDatos, datos_cliente->buffer, SOCKET_MAX_BUFFER, 0);
			break;
		default:
			;//hacer nada
	}
	printLog("Thread Datos", "2", threadID[datos_cliente->socket_comando], "INFO", "Desconexion al Thread de Datos");		
	// DECIRLE AL DE COMANDOS QUE MANDE EL 226

	closesocket(clienteDatos);
	closesocket(descriptorD);
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
	SOCKET *aux = HeapAlloc(heap, HEAP_NO_SERIALIZE, sizeof(SOCKET));
	reg_cliente *datos_cliente  = HeapAlloc(heap, HEAP_NO_SERIALIZE, sizeof(reg_cliente));
	aux = (SOCKET*) pArguments;
	datos_cliente->socket_comando = *aux;
	strcpy(datos_cliente->current_path, "/");
	
	//HeapFree(heap, HEAP_NO_SERIALIZE, aux);
	
	//int (*puntero)(char*, reg_cliente*);

	datos_cliente->evento1 = CreateEvent(NULL, FALSE, FALSE, NULL);
	datos_cliente->evento2 = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	//printf("%d", datos_cliente->socket_comando);

	send(datos_cliente->socket_comando,"220 POWER\r\n", strlen("220 POWER\r\n"),0);
    corrector=recv(datos_cliente->socket_comando,buffer,SOCKET_MAX_BUFFER,0);
    buffer[corrector]='\0';
	
	

	strcpy(comando, obtenerComando(buffer));
	strcpy(argumento, obtenerParametro(buffer));
	printf("%s ", comando);
	printf("%s", argumento);
	
	while( corrector != SOCKET_ERROR ){  // espero que cierre sesion
		while (strcmp(comando,"PASV\r\n")&&(corrector!= SOCKET_ERROR)){  // espero por pasv
			command_handler(vector_comandos, comando, argumento, datos_cliente);
			corrector=recv(datos_cliente->socket_comando,buffer,SOCKET_MAX_BUFFER,0);
			buffer[corrector]='\0';
			strcpy(comando, obtenerComando(buffer));
			strcpy(argumento, obtenerParametro(buffer));
			printf("%s ", comando);
			printf("%s\n", argumento);
		}
		printf("llego aca");
		hThreadDatos = (HANDLE) _beginthreadex(NULL,0, &threadDeDatos, (void*) datos_cliente, 0, NULL);
	    WaitForSingleObject(datos_cliente->evento1,INFINITE);
		command_handler(vector_comandos, comando, argumento, datos_cliente);  // ejecuto PASV : mando puerto y demas,,
		corrector=recv(datos_cliente->socket_comando,buffer,SOCKET_MAX_BUFFER,0);
		buffer[corrector]='\0';
		strcpy(comando, obtenerComando(buffer));
		strcpy(argumento, obtenerParametro(buffer));

		while ( strcmp(comando,"STOR") && strcmp(comando, "RETR") && strcmp(comando, "LIST\r\n") && strcmp(comando, "DELE") &&(corrector!= SOCKET_ERROR)){ //espero uno de estos comandos
			printf(" no es retr, stor, list ni dele \n" );
			command_handler(vector_comandos, comando, argumento, datos_cliente);
			corrector=recv(datos_cliente->socket_comando,buffer,SOCKET_MAX_BUFFER,0);
			buffer[corrector]='\0';
			strcpy(comando, obtenerComando(buffer));
			strcpy(argumento, obtenerParametro(buffer));
			printf("el comando en el while es %s y su largo es %d\n",comando,strlen(comando));
		}
		
		printf("voy a responder a LIST RETR STOR DELE\n" );
		command_handler(vector_comandos, comando, argumento, datos_cliente); // mando el list o retr o stor o dele

		SetEvent(datos_cliente->evento2);     // le aviso al thread de datos que tiene que mandar o recibir
		WaitForSingleObject(hThreadDatos, INFINITE); // espero que termine el thread de datos
		CloseHandle(hThreadDatos);
		send(datos_cliente->socket_comando,"226 Transfer Complete\r\n", strlen("226 Transfer Complete\r\n"),0);
		corrector=recv(datos_cliente->socket_comando,buffer,SOCKET_MAX_BUFFER,0);
		buffer[corrector]='\0';
		strcpy(comando, obtenerComando(buffer));
		strcpy(argumento, obtenerParametro(buffer));
	}
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