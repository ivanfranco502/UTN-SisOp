// compile with: /MT
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
	int *remoteClient  = HeapAlloc(heapDatos, HEAP_NO_SERIALIZE, sizeof(int));
	remoteClient = (int*) pArguments;


    local_address->sin_family = AF_INET;
	local_address->sin_addr.s_addr=INADDR_ANY;
	local_address->sin_port = htons (5300);

	//printf("%d\n",descriptor= socket(AF_INET, SOCK_STREAM, 0));
	printf("%d\n",descriptorD= socket(AF_INET, SOCK_STREAM, 0));
	bind (descriptorD,(struct sockaddr *) local_address, addrlen);
	listen(descriptorD,100);

	enviar_comando(*remoteClient,"227 Entering Pasive Mode (192,168,140,129,20,180)\r\n",buffer);

	printf("%d\n", clienteDatos = accept(descriptorD, (struct sockaddr *)remote_address, (void*)&addrlen));

	if(clienteDatos!=INVALID_SOCKET){
		Sleep(10);
		enviar_comando_no_recv(clienteDatos,"150 Openning ASCII mode\r\n",buffer);
		Sleep(10);
		send(clienteDatos,"-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo1.txt\r\n",strlen("-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo1.txt\r\n"),0);
		send(clienteDatos,"-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo2.txt\r\n",strlen("-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo2.txt\r\n"),0);
		send(clienteDatos,"drwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 directorio1\r\n",strlen("drwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 directorio1\r\n"),0);
		
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
	char buffer[100];
	int *remoteClient  = HeapAlloc(heap, HEAP_NO_SERIALIZE, sizeof(int));
	remoteClient = (int*) pArguments;
	
	
	/* aca va el send y rcv de datos */
	/*printf("Thread del cliente\n");
	recv(*remoteClient, buffer, SOCKET_MAX_BUFFER*sizeof(char),0);
	printf("%s\n",buffer);*/
	
	enviar_comando(*remoteClient,"220 POWER\r\n",buffer);
	enviar_comando(*remoteClient,"230 POWER\r\n",buffer);
	wait_for(*remoteClient,"PWD\r\n",buffer);
	enviar_comando(*remoteClient,"257 \"/\" Is the current directory \r\n",buffer);
	//wait_for(*remoteClient,"TYPE A\r\n",buffer);
	//enviar_comando(*remoteClient,"250 CWD command successful\r\n",buffer);
	if (!(strcmp(buffer, "noop")))
		enviar_comando(*remoteClient,"200 NOOP Command succesfull /\r\n",buffer);
	if (!(strcmp(buffer, "CWD /")))
		enviar_comando(*remoteClient,"200 ubicacion /\r\n",buffer);
	enviar_comando(*remoteClient,"200 Type set to A\r\n",buffer);
	Sleep(5);
	
	hThreadDatos = (HANDLE) _beginthreadex(NULL,0, &threadDeDatos, (void*) remoteClient, 0, NULL);
	WaitForSingleObject(hThreadDatos, INFINITE);
	CloseHandle(hThreadDatos);

	enviar_comando(*remoteClient,"226 Transfer Complete\r\n",buffer);

	
	//HeapFree(heap, HEAP_NO_SERIALIZE, remoteClient);
	HeapDestroy(heap);
	threadsFinalizados[*remoteClient]=1;
    _endthreadex( 0 );
    
} 



int main(){ 
    HANDLE hThread[CANTIDAD_CLIENTES];
    unsigned threadID[CANTIDAD_CLIENTES];
	SOCKET descriptor;
	int *remoteClient;
	int i, a, addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in *local_address = HeapAlloc(GetProcessHeap(), 0, addrlen);
    struct sockaddr_in *remote_address = HeapAlloc(GetProcessHeap(), 0, addrlen);
	
	remoteClient = (int *) HeapAlloc(GetProcessHeap(), 0, sizeof(int));

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

    printf("estoy escuchando el puerto de mierda\n");
	
	inicializarVectorDeThreads();
	
	while(1){
		if((*remoteClient = accept (descriptor, (struct sockaddr *)remote_address, (void*)&addrlen))!= -1){
			printf("%d\n", *remoteClient);
			hThread[*remoteClient] = (HANDLE) _beginthreadex(NULL,0, &threadClienteNuevo, (void*) remoteClient, 0, &threadID[*remoteClient]);
		}

		for (i=0; i<CANTIDAD_CLIENTES;i++){
			if (threadsFinalizados[i]){
				closesocket(i);
				CloseHandle(hThread[i]);
			}

		}
			
		
	}

        




}