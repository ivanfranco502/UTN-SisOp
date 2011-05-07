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


typedef struct  {

//registro del cliente TYPE, PUERTO DE DATOS, CURRENT PATH, QUE MAS?
                char type[8];
                int puerto_datos;
                char current_path[100];
				int socket_comando;

                } reg_cliente;

//estructura que tengo que vectorizar con las funciones para cada comando
typedef struct 	{
		
		int (*pfunc)(char *,reg_cliente *);
		char mensaje[5];
		} t_command_handler;

int threadsFinalizados[CANTIDAD_CLIENTES]; /*Empiezan en cero y se ponen a uno cuando finaliza el thread */
unsigned threadID[CANTIDAD_CLIENTES];

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
	remoteClient = (reg_cliente*) pArguments;


    local_address->sin_family = AF_INET;
	local_address->sin_addr.s_addr=INADDR_ANY;
	local_address->sin_port = htons (5300);

	//printf("%d\n",descriptor= socket(AF_INET, SOCK_STREAM, 0));
	printf("%d\n",descriptorD= socket(AF_INET, SOCK_STREAM, 0));
	bind (descriptorD,(struct sockaddr *) local_address, addrlen);
	listen(descriptorD,100);

	command_handler(vector_comandos, comando, argumento, datos_cliente);

	printf("%d\n", clienteDatos = accept(descriptorD, (struct sockaddr *)remote_address, (void*)&addrlen));

	if(clienteDatos!=INVALID_SOCKET){
		if (strcmp(datos_cliente->type,"ASCII") == 0){
			command_handler(vector_comandos, comando, argumento, datos_cliente);
			send(clienteDatos,"-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo1.txt\r\n",
				 strlen("-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo1.txt\r\n"),0);
			send(clienteDatos,"-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo2.txt\r\n",
				 strlen("-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo2.txt\r\n"),0);
			send(clienteDatos,"drwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 directorio1\r\n",
				 strlen("drwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 directorio1\r\n"),0);
		}else {
			if(strcmp(comando, "RETR") == 0){
				send(clienteDatos, "Contenido del archivo descargado", 
					 strlen("Contenido del archivo descargado"),0);
			}
		}
		closesocket(clienteDatos);
		closesocket(descriptorD);
	}else
		printf("No se conectaron al thread de datos\n");
	//HeapFree(heap, HEAP_NO_SERIALIZE, remoteClient);
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
	remoteClient = (int*) pArguments;
	int (*puntero)(char*, reg_cliente*);
	t_command_handler vector_comandos[10];

	vector_comandos[0].pfunc=&rta_PASV;
	strcpy(vector_comandos[0].mensaje,"PASV");
	vector_comandos[1].pfunc=&rta_NOOP;
	strcpy(vector_comandos[1].mensaje,"NOOP");
	vector_comandos[2].pfunc=&rta_DELE;
	strcpy(vector_comandos[2].mensaje,"DELE");
	vector_comandos[3].pfunc=&rta_TYPE;
	strcpy(vector_comandos[3].mensaje,"TYPE");
	vector_comandos[4].pfunc=&rta_LIST;
	strcpy(vector_comandos[4].mensaje,"LIST");
	vector_comandos[5].pfunc=&rta_CWD;
	strcpy(vector_comandos[5].mensaje,"CWD");
	vector_comandos[6].pfunc=&rta_PWD;
	strcpy(vector_comandos[6].mensaje,"PWD");
	vector_comandos[7].pfunc=&rta_HELP;
	strcpy(vector_comandos[7].mensaje,"HELP");
	vector_comandos[8].pfunc=&rta_RETR;
	strcpy(vector_comandos[8].mensaje,"RETR");
	vector_comandos[9].pfunc=&rta_STOR;
	strcpy(vector_comandos[9].mensaje,"STOR");
	
	send(*remoteClient,"220 POWER\r\n", strlen("220 POWER\r\n"),0);
    corrector=recv(*remoteClient,buffer,SOCKET_MAX_BUFFER,0);
    buffer[corrector]='\0';
	
	strcpy(comando, obtenerComando(buffer));
	strcpy(argumento, obtenerParametro(buffer));
	while( (strcmp(comando, "bye")) && (strcmp(comando, "quit")) && (strcmp(comando, "exit")) ){
		while (strcmp(comando,"PASV")){
			command_handler(vector_comandos, comando, argumento, datos_cliente);
			corrector=recv(*remoteClient,buffer,SOCKET_MAX_BUFFER,0);
			buffer[corrector]='\0';
		}
		printLog("Thread Cmd","1",threadID[*remoteClient],"INFO","Conexion al Thread de Datos");
		hThreadDatos = (HANDLE) _beginthreadex(NULL,0, &threadDeDatos, (void*) remoteClient, 0, NULL);
		WaitForSingleObject(hThreadDatos, INFINITE);
		CloseHandle(hThreadDatos);
		printLog("Thread Cmd", "1", threadID[*remoteClient], "INFO", "Desconexion al Thread de Datos");
		send(*remoteClient,"226 Transfer Complete\r\n", strlen("226 Transfer Complete\r\n"),0);
	}

	//HeapFree(heap, HEAP_NO_SERIALIZE, remoteClient);
	HeapDestroy(heap);
	threadsFinalizados[*remoteClient]=1;
    _endthreadex( 0 );
    
} 

int main(){ 
    HANDLE hThread[CANTIDAD_CLIENTES];
    SOCKET descriptor;
	reg_cliente *datos_cliente;
	int a, addrlen = sizeof(struct sockaddr_in);
	unsigned i;
	struct sockaddr_in *local_address = HeapAlloc(GetProcessHeap(), 0, addrlen);
    struct sockaddr_in *remote_address = HeapAlloc(GetProcessHeap(), 0, addrlen);
	
	printLog("Main FTPS","0","0","INFO","Archivo de Configuracion");
	datos_cliente = (reg_cliente *) HeapAlloc(GetProcessHeap(), 0, sizeof(reg_cliente));

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
		if((*remoteClient = accept (descriptor, (struct sockaddr *)remote_address, (void*)&addrlen))!= -1){
			printf("%d\n", *datos_cliente);
			hThread[*datos_cliente] = (HANDLE) _beginthreadex(NULL,0, &threadClienteNuevo, (void*) datos_cliente, 0, &threadID[*datos_cliente]);
			printLog("New Client","0",threadID[*remoteClient],"INFO","Conexion Nuevo cliente al puerto ftp");

		}

		for (i=0; i<CANTIDAD_CLIENTES;i++){
			if (threadsFinalizados[i]){
				printLog("Disconnect client","0",threadID[i], "INFO", "Desconexion cliente al puerto ftp");
				closesocket(i);
				CloseHandle(hThread[i]);
			}

		}
	}
	return 0;
}