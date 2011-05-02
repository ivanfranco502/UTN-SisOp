// compile with: /MT
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include <stdio.h>
#include <process.h>   /* _beginthreadex, _endthreadex */

WSADATA wsaData;


/*unsigned*/ __stdcall threadClienteNuevo( void* pArguments ){
    int *remoteClient;
	remoteClient = (int*) pArguments;
	
	/* aca va el send y rcv de datos*/

	
	
	

    _endthreadex( 0 );
    return *remoteClient;
} 

int main(){ 
    HANDLE hThread[1000];
    unsigned threadID[1000];
	SOCKET descriptor;
	int *remoteClient;
	int numeroDeCliente =0;
	int a, addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in *local_address= malloc(addrlen);
    struct sockaddr_in *remote_address= malloc(addrlen);

	
	remoteClient = (int *) malloc(sizeof(int));

	if ((a = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		printf("WSAStartup() devolvio error nro %d\n", a );
		return 255;
	}
	
	

    local_address->sin_family = AF_INET;
	local_address->sin_addr.s_addr=INADDR_ANY;
	local_address->sin_port = htons (21);

	descriptor= socket(AF_INET, SOCK_STREAM, 0);
	bind (descriptor,(struct sockaddr *) local_address, addrlen);
	listen(descriptor,100);

    printf("estoy escuchando el puerto de mierda\n");

	while(1){
		if((*remoteClient = accept (descriptor, (struct sockaddr *)remote_address, (void*)&addrlen))!= -1){
			printf("%d", *remoteClient);
			hThread[*remoteClient] = (HANDLE) _beginthreadex(NULL,0, &threadClienteNuevo, (void*) remoteClient, 0, &threadID[numeroDeCliente]);
		
			//CloseHandle(hThread[*remoteClient]);
		
			numeroDeCliente++;
		}
	}

        

	
	
	

	

    
   

	
	
	
	getchar();
}