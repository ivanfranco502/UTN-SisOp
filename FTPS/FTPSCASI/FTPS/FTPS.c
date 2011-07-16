#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include <stdio.h>
#include <process.h>   /* _beginthreadex, _endthreadex */
#include <string.h>
#include "funcionesMPS.h"
#include "funcionesftp.h"
#include "funcionesConfig.h"

#define CANTIDAD_CLIENTES 65535
#define SOCKET_MAX_BUFFER 5000

WSADATA wsaData;

typedef struct {
	HANDLE socketOcupado;
	SOCKET socketAux;
	configFTP *config;
	int descriptorSocketKernel;
}argumentosThreads;

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
		 argumento[50],
		 path[100],
		 mensajeLog[100],
		 auxLog[50];
	SOCKET aux = HeapAlloc(heap, HEAP_NO_SERIALIZE, sizeof(SOCKET));
	reg_cliente *datos_cliente  = HeapAlloc(heap, HEAP_NO_SERIALIZE, sizeof(reg_cliente));
	argumentosThreads *argumentosRecibidos = HeapAlloc(heap, HEAP_NO_SERIALIZE, sizeof(argumentosThreads));
	
	argumentosRecibidos = (argumentosThreads *)pArguments;
	aux = argumentosRecibidos->socketAux;
	datos_cliente->socket_comando = aux;
	strcpy(datos_cliente->ftp_path, "/");
	datos_cliente->threadID=threadID[datos_cliente->socket_comando];
	//HeapFree(heap, HEAP_NO_SERIALIZE, aux);
	//int (*puntero)(char*, reg_cliente*);
	datos_cliente->evento1 = CreateEvent(NULL, FALSE, FALSE, NULL);
	datos_cliente->evento2 = CreateEvent(NULL, FALSE, FALSE, NULL);

	strcpy(datos_cliente->IP, argumentosRecibidos->config->IPServer);
	datos_cliente->puerto_datos = argumentosRecibidos->config->puertoServer;
	datos_cliente->socketKSS = argumentosRecibidos->descriptorSocketKernel;

	datos_cliente->thDatosOK = 0;
	datos_cliente->socketOcupado = argumentosRecibidos->socketOcupado;
	
	/*---------------------------------------Log Conexion ThDatos----------------------------*/
	printLog("Thread Comandos","1",datos_cliente->threadID,"INFO","Conexion al Thread de Comandos");
	/*------------------------------------Fin Log Conexion ThDatos---------------------------*/

	//printf("%d", datos_cliente->socket_comando);

	send(datos_cliente->socket_comando,"220 POWER\r\n", strlen("220 POWER\r\n"),0);
    corrector=recv(datos_cliente->socket_comando,buffer,SOCKET_MAX_BUFFER,0);
    buffer[corrector]='\0';
	
	strcpy(comando, obtenerComando(buffer));
	strcpy(argumento, obtenerParametro(buffer));

	/*-----------------------------COMANDO Y ARGUMENTO----------------------------------*/
	strcpy(mensajeLog, comando);
	strcat(mensajeLog, " ");
	strcat(mensajeLog, argumento);
	printLog("Thread Comandos","1",datos_cliente->threadID,"DEBUG",mensajeLog);
	/*-----------------------------------FIN----------------------------*/
	
	while( corrector != SOCKET_ERROR ){  // espero que cierre sesion
		command_handler(vector_comandos, comando, argumento, datos_cliente);
		corrector=recv(datos_cliente->socket_comando,buffer,SOCKET_MAX_BUFFER,0);
		buffer[corrector]='\0';
		strcpy(comando, obtenerComando(buffer));
		strcpy(argumento, obtenerParametro(buffer));

/*		-----------------------------COMANDO Y ARGUMENTO----------------------------------*/
		strcpy(mensajeLog, comando);
		strcat(mensajeLog, " ");
		strcat(mensajeLog, argumento);
		printLog("Thread Comandos","1",datos_cliente->threadID,"DEBUG",mensajeLog);
/*		-----------------------------------FIN----------------------------*/
	}
	/*----------------------------------DESCONEXION Th Comandos-----------------------------*/
	printLog("Thread Comandos","1",datos_cliente->threadID,"INFO","Desconexion al Thread de Comandos");
	/*---------------------------------FIN------------------------------*/
	
	CloseHandle(datos_cliente->evento1);
	CloseHandle(datos_cliente->evento2);
	threadsFinalizados[datos_cliente->socket_comando] = 1;
    HeapDestroy(heap);
	_endthreadex( 0 );
    
} 

int main(){ 
	SOCKET descriptor;
	SOCKET socketAux;
	HANDLE hThread[CANTIDAD_CLIENTES];
	int a,
		addrlen = sizeof(struct sockaddr_in);
	unsigned i;
	char mensajeLog[100],
		 auxLog[50],
		 IPKSS[16];
	unsigned puertoKSS = 5000;
	struct sockaddr_in *local_address = HeapAlloc(GetProcessHeap(), 0, addrlen);
    struct sockaddr_in *remote_address = HeapAlloc(GetProcessHeap(), 0, addrlen);
	MPS_Package *paqueteKSS = HeapAlloc(GetProcessHeap(), 0, sizeof(MPS_Package));
	argumentosThreads *argumentos;
	configFTP *auxiliar;

	argumentos = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, sizeof(argumentosThreads));
	socketAux = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, sizeof(SOCKET));
	auxiliar = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, sizeof(configFTP));
	
	/*---------------------------Begin------------------------------------*/
	printLog("Main FTPS","0",0,"DEBUG","Comienza FTP");
	/*------------------------------end---------------------------------*/
	
	getConfigFTP(auxiliar);

	argumentos->config = auxiliar;

	argumentos->socketOcupado = CreateMutex(NULL, FALSE, NULL);

	/*-----------------------------------Log Config----------------------------------------*/
	strcpy(mensajeLog, "IPKernel:");
	strcat(mensajeLog, argumentos->config->IPKernel);
	strcat(mensajeLog," PortKernel:");
	sprintf(auxLog, "%d", argumentos->config->puertoKernel);
	strcat(mensajeLog, auxLog);
	printLog("Main FTP","1",0, "INFO",mensajeLog);
	/*--------------------------------------Fin Log Config----------------------------------*/

	argumentos->descriptorSocketKernel = conectarConKernel(argumentos->config->IPKernel, argumentos->config->puertoKernel, paqueteKSS);
	printf("%c", paqueteKSS->PayloadDescriptor);
	if(argumentos->descriptorSocketKernel != -1){
		paraElMain(vector_comandos);
		
		if ((a = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
			/*-----------------------------ERROR----------------------------------*/
			strcpy(mensajeLog, "WSAStartup() devolvio error nro ");
			sprintf(auxLog,"%d", a);
			strcat(mensajeLog,auxLog);
			printLog("Main FTP","0",0,"ERROR",mensajeLog);
			/*---------------------------------------------------------------*/
			return 255;
		}
			
		local_address->sin_family = AF_INET;
		local_address->sin_addr.s_addr=INADDR_ANY;
		local_address->sin_port = htons (21);
		
		descriptor= socket(AF_INET, SOCK_STREAM, 0);
		/*-----------------------------IMPRIME Descriptor Socket---------------------*/
		strcpy(mensajeLog, "Descriptor Socket: ");
		sprintf(auxLog,"%d",descriptor);
		strcat(mensajeLog, auxLog);
		printLog("Main FTP","0",0,"DEBUG",mensajeLog);
		/*-------------------------------FIN--------------------------------*/
		
		bind (descriptor,(struct sockaddr *) local_address, addrlen);
		listen(descriptor,100);
		
		/*-----------------------------LISTEN PORT----------------------------------*/
		printLog("Main FTP","0",0,"DEBUG","Escuchando puerto");
		/*---------------------------------FIN------------------------------*/
		
		inicializarVectorDeThreads();
		
		while(1){
			if((socketAux = accept (descriptor, (struct sockaddr *)remote_address, (void*)&addrlen))!= -1){
				argumentos->socketAux = socketAux;
				hThread[socketAux] = (HANDLE) _beginthreadex(NULL,0, &threadClienteNuevo, (void*) &(*argumentos), 0, &threadID[socketAux]);
				//printf("%d", *socketAux);
				/*----------------------------CONEXION NUEVO CLLIENTE-----------------------------------*/
				strcpy(mensajeLog, "Conexion Nuevo cliente al puerto ftp: ");
				sprintf(auxLog,"%d", socketAux);
				strcat(mensajeLog,auxLog);
				printLog("Main FTP","0",0,"DEBUG",mensajeLog);
				/*---------------------------------------------------------------*/
			}

			for (i=0; i<CANTIDAD_CLIENTES;i++){
				if (threadsFinalizados[i]){
					/*---------------------------------DESCONEXION CLIENTE------------------------------*/
					printLog("Disconnect client","0",threadID[i], "INFO", "Desconexion cliente al puerto ftp");
					/*-----------------------------FIN DESCONEXION CLIENTE----------------------------------*/
					closesocket(i);
					CloseHandle(hThread[i]);
					threadsFinalizados[i]=0;
				}

			}
		}
		CloseHandle(argumentos->socketOcupado);
		closesocket(argumentos->descriptorSocketKernel);
	}
	return 0;
}