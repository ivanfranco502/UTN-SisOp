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
#define MAX_SEM_COUNT 10

HANDLE hSemaphore; 

WSADATA wsaData;
unsigned __stdcall kss( void*);

unsigned __stdcall consola1( void* pArguments )
{
int opcion=0,i=0,c=0;
long dirLogica;
Nodo *nodo,*lista=NULL;
struct chs CHS;
struct info datos;
char dir[10];
char x[2];
	while(1){
//		opcion=0;
//		while(opcion!=1 && opcion!=2){
		system ("cls");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),9);
		printf("1-Grabar\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),14);
		//	printf("2-Leer\n");
		//	printf("3-Borrar\n");
	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),9);
		printf("2-Posicion Cabezal\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),15);
	if(gets(x)){
		opcion=atoi(x);		
				switch(opcion){
					case 1:
						printf("Escriba los sectores que desea leer\n");
						WaitForSingleObject(hSemaphore,INFINITE);
						while(i<3){
							printf("Escriba la Direccion Logica:");
							scanf("%d",&dirLogica);
							nodo=generarNodo(dirLogica);
							nodo->accion=1;
							lista=InsertarNodo(lista,nodo);
							i++;
						}
						mostrarLista(lista);
						i=0;nodo=NULL;lista=NULL;
						ReleaseSemaphore(hSemaphore,1,NULL);
					break;
					case 2:
						system ("cls");
						posCabezal();
					break;
					default:
						printf("NO esta en las opciones!!\n");
				}
			}
		}	
	

    _endthreadex( 0 );
    return 0;
}

int main()
{	
	int c=0;
	unsigned threadID,threadID2;
	HANDLE hThread,hThread2;
	hSemaphore = CreateSemaphore(NULL,MAX_SEM_COUNT,MAX_SEM_COUNT, NULL);
	while(1){
		hThread = (HANDLE)_beginthreadex( NULL, 0, &consola1, NULL, 0, &threadID );
		hThread2 = (HANDLE)_beginthreadex( NULL, 0, &kss, NULL, 0, &threadID2 );
		
		WaitForSingleObject( hThread,INFINITE );
		WaitForSingleObject( hThread2,INFINITE );
	}
	CloseHandle( hThread );
	CloseHandle( hThread2 );
	CloseHandle( hSemaphore );
}


unsigned __stdcall kss( void* pArguments )
{

    // Thread de comandos
	long escrito;
	struct infoGrabar buffer;
	struct buffer datos;
	HANDLE  heap = HeapCreate(HEAP_NO_SERIALIZE, 1024*1024, 0);
	int corrector;
	SOCKET descriptor;
	int *remoteClient;
	int r=-1,i, a, addrlen = sizeof(struct sockaddr_in);
	
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

	while((*remoteClient = accept (descriptor, (struct sockaddr *)remote_address, (void*)&addrlen))== -1){
		printf("%d",*remoteClient = accept (descriptor, (struct sockaddr *)remote_address, (void*)&addrlen));
	}
	/* aca va el send y rcv de datos */
	while(r!=0){
		r=recv(*remoteClient,(void *) &buffer,1032,0);
		if(r>0){
			if(buffer.dato1=="NULL" && buffer.dato2=="NULL"){
				datos=getSectores(buffer.dir1,buffer.dir2);
				send(*remoteClient,(void *)&datos,1024,0);
			}else{
				WaitForSingleObject(hSemaphore,INFINITE);
				escrito=putSectores(buffer);
				ReleaseSemaphore(hSemaphore, 1,NULL);
				send(*remoteClient,escrito,8,0);
			}
		}	
	}
	HeapDestroy(heap);
    _endthreadex( 0 );

}