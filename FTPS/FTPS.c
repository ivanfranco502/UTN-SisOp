// compile with: /MT
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include <stdio.h>
#include <process.h>   /* _beginthreadex, _endthreadex */

#define CANTIDAD_CLIENTES 65535

WSADATA wsaData;
int threadsFinalizados[CANTIDAD_CLIENTES]; /*Empiezan en cero y se ponen a uno cuando finaliza el thread */


void inicializarVectorDeThreads(){
	// Inicializa en cero threadsFinalizados[]
	int i=0;
	for (i; i<CANTIDAD_CLIENTES; i++)
		threadsFinalizados[i]=0;
}


unsigned __stdcall threadClienteNuevo( void* pArguments ){
    // Thread de datos
	
	
	HANDLE  heap = HeapCreate(HEAP_NO_SERIALIZE, 1024*1024, 0);

	
	int *remoteClient  = HeapAlloc(heap, HEAP_NO_SERIALIZE, sizeof(int));
	remoteClient = (int*) pArguments;
	
	/* aca va el send y rcv de datos */
	
	//printf("Thread del cliente");
	
	
	
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
			printf("%d", *remoteClient);
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