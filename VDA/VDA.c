#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include <stdio.h>
#include <process.h>   
#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include <db.h>
#pragma comment (lib, "libdb48.lib") 
#include "funcionesVDA.h"

#define SOCKET_MAX_BUFFER 100


WSADATA wsaData;
unsigned __stdcall kss( void*);

unsigned __stdcall consola1( void* pArguments )
{
int opcion=0,i=0;
long dirLogica;
Nodo *nodo,*lista=NULL;
struct chs CHS;
struct info datos;
char *str1,*str2;

	system ("cls");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),9);
		printf("1-Grabar\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),14);
		//	printf("2-Leer\n");
		//	printf("3-Borrar\n");
	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),9);
		printf("2-Posicion Cabezal\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),15);

	while(opcion!=1 && opcion!=2){
		scanf("%i\n",&opcion);		
		switch(opcion){
			case 1:
				printf("Escriba los sectores que desea leer\n");
				while(i<3){
					printf("Escriba la Direccion Logica:");
					scanf("%d\n",&dirLogica);
					nodo=generarNodo(dirLogica);
					nodo->accion=1;
					lista=InsertarNodo(lista,nodo);
					i++;
				}
				mostrarLista(lista);
				opcion=1;
			break;
			case 2:
				system ("cls");
				posCabezal();
			break;
			default:
				printf("NO esta en las opciones!!\n");
		}

	}

    _endthreadex( 0 );
    return 0;
}

int main()
{	
	int c=0;
	HANDLE hThread,hThread2;
    unsigned threadID;
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

	while(c!=27){
		hThread = (HANDLE)_beginthreadex( NULL, 0, &consola1, NULL, 0, &threadID );
		
	/*	if((*remoteClient = accept (descriptor, (struct sockaddr *)remote_address, (void*)&addrlen))!=-1){
			printf("Cliente:%d\n", *remoteClient);
			printf("%d\n", *remoteClient);
			hThread2 = (HANDLE) _beginthreadex(NULL,0, &kss, (void*) remoteClient, 0, &threadID);
			WaitForSingleObject( hThread2, 1000000 );
		}*/
	
		WaitForSingleObject( hThread,1000000 );
		CloseHandle( hThread );
		printf("Desea seguir? ENTER(sigue) ESC(termina)");
		c=getchar();
	}
	CloseHandle( hThread2 );
}


unsigned __stdcall kss( void* pArguments )
{

    // Thread de comandos
	long escrito;
	struct infoGrabar buffer;
	struct buffer datos;
	HANDLE  heap = HeapCreate(HEAP_NO_SERIALIZE, 1024*1024, 0);
	HANDLE hThreadDatos;
	int corrector;
	

	int *remoteClient  = HeapAlloc(heap, HEAP_NO_SERIALIZE, sizeof(int));
	remoteClient = (int*) pArguments;

	
	/* aca va el send y rcv de datos */
	
		recv(*remoteClient,(void *) &buffer,1032,0);
		if(buffer.dato1==NULL && buffer.dato2==NULL){
			datos=getSectores(buffer.dir1,buffer.dir2);
			send(*remoteClient,(void *)&datos,1024,0);
		}else{
			escrito=putSectores(buffer);
			send(*remoteClient,escrito,8,0);
		}
	
	HeapDestroy(heap);
    _endthreadex( 0 );

}