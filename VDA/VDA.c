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
#include "funcionesMPSvda.h"
#include "funcionesConfig.h"
#include "funcionesLog.h"

#define WIN32_LEAN_AND_MEAN
#define SOCKET_MAX_BUFFER 100
#define MAX_SEM_COUNT 10
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#define PUERTO 21

HANDLE hSemaphore; 
WSADATA wsaData;
typedef struct {
	HANDLE socketOcupado;
	SOCKET socketAux;
	configVDA *config;
	int descriptorSocketKernel;
}argumentosThreads;

struct infoGrabar* Estructura(char *);
unsigned __stdcall kss( void*);

unsigned __stdcall consola1( void* pArguments )
{
int opcion=0,i=0,c=0;
long dirLogica;
Nodo *nodo,*lista=NULL;
struct chs CHS;
char dir[10],data[512];
char x[2];
HANDLE heapCons = HeapCreate(HEAP_NO_SERIALIZE, 1024*1024, 0);
	while(1){
//		opcion=0;
//		while(opcion!=1 && opcion!=2){
		system ("cls");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),9);
		printf("1-Leer\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),14);
			printf("2-Posicion Cabezal\n");
			printf("3-Grabar\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),9);
		printf("4-Borrar\n");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),15);
	if(gets(x)){
		opcion=atoi(x);		
				switch(opcion){
					case 1:
						printf("Escriba los sectores que desea leer\n");
						while(i<5){
							printf("Escriba la Direccion Logica:");
							scanf("%d",&dirLogica);
							if(dirLogica>20000){
							printf("No existe ese Sector,escriba uno mas chico!\n");
							}else{
								nodo=generarNodo(dirLogica,heapCons);
								nodo->accion=2;
								lista=InsertarNodo(lista,nodo);
								i++;
							}
						}
						mostrarLista(lista,heapCons, 1);
						i=0;nodo=NULL;lista=NULL;
						getchar();
						getchar();
					break;
					case 2:
						system ("cls");
						posCabezal();
						getchar();
					break;
					case 3:
						system ("cls");
						printf("Escribir datos:");
						gets(data);
						printf("Escribir direccion:");
						scanf("%d\n",&dirLogica);
						grabar(dirLogica,data);
						grabarCabezal(dirLogica);
					break;
					case 4:
						system ("cls");
						printf("Escribir direccion:",dirLogica);
						scanf("%d\n",&dirLogica);
						borrar(dirLogica);
					break;
					
					default:
						printf("NO esta en las opciones!!\n");
				}
			}
		}	
	
	HeapDestroy(heapCons);
    _endthreadex( 0 );
    return 0;
}

int main()
{	
	int c=0,x=0;
	unsigned threadID,threadID2;
	HANDLE hThread,hThread2;
	char mensajeLog[100],auxLog[50],IPKSS[16];
	argumentosThreads *argumentos;
	SOCKET socketAux;
	configVDA *auxiliar;
	unsigned puertoKSS = 5000;

	argumentos = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, sizeof(argumentosThreads));
	socketAux = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, sizeof(SOCKET));
	auxiliar = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, sizeof(configVDA));

	getConfigVDA(auxiliar);

	argumentos->config = auxiliar;
	
	strcpy(vda,auxiliar->nombreVDA);

	crearBase();
	
	while(x<10){
		cache[x].dirLog=-1;
		cache[x].count=0;
		x++;
	}
	
	hThread = (HANDLE)_beginthreadex( NULL, 0, &consola1, NULL, 0, &threadID );
	hThread2 = (HANDLE)_beginthreadex( NULL, 0, &kss, NULL, 0, &threadID2 );
	
	WaitForSingleObject( hThread,INFINITE );
	WaitForSingleObject( hThread2,INFINITE );

	CloseHandle( hThread );
	CloseHandle( hThread2 );
	CloseHandle( hSemaphore );
}


unsigned __stdcall kss( void* pArguments )
{		
		int x=0,i=0,z=0,p=0,r=1;
		long dir1=0,dir2=0;
		char aux[1032],dato[1024];
		char Buffer[1200];
        struct infoGrabar *estructura;
		struct buffer datos;
		char c[10]="hola";
        SOCKET  conn_socket;
		char mensaje_aux[14];	
		char stru_aux[1032];
        char *server_name= "localhost";
        unsigned short port = 21;
        int retval, loopflag = 0;
        unsigned int addr;
        int socket_type = SOCK_STREAM;
        struct sockaddr_in server;
        struct hostent *hp;char mensaje[512];
        WSADATA wsaData;
		char mensajeLog[100],auxLog[50],IPKSS[16];
		struct chs CHS;			
		argumentosThreads *argumentos;
		SOCKET socketAux;
		configVDA *auxiliar;
		unsigned puertoKSS = 5000;
		int logActivada=0;
		HANDLE heapKSS = HeapCreate(HEAP_NO_SERIALIZE, 1024*1024, 0);

		MPS_Package *response = HeapAlloc(GetProcessHeap(),0,sizeof(MPS_Package));
		MPS_Package *paqueteMPS = HeapAlloc(GetProcessHeap(),0,sizeof(MPS_Package));
	
		argumentos = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, sizeof(argumentosThreads));
		socketAux = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, sizeof(SOCKET));
		auxiliar = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, sizeof(configVDA));
	
		getConfigVDA(auxiliar);

		argumentos->config = auxiliar;

		argumentos->socketOcupado = CreateMutex(NULL, FALSE, NULL);

		logActivada = auxiliar->logActivada;

		/*-----------------------------------Log Config----------------------------------------*/
		strcpy(mensajeLog, "IPKernel:");
		strcat(mensajeLog, argumentos->config->ipKSS,1);
		strcat(mensajeLog," PortKernel:");
		sprintf(auxLog, "%d", argumentos->config->puertoKss,1);
		strcat(mensajeLog, auxLog);
		printLog("Main VDA","1",0, "INFO",mensajeLog,logActivada);
		/*--------------------------------------Fin Log Config----------------------------------*/
	
		if ((retval = WSAStartup(0x202, &wsaData)) != 0){
            fprintf(stderr,"VDA: WSAStartup() failed with error %d\n", retval);
            WSACleanup();
        }
		
        // detectamos si se pueden usar gethostbyname() o gethostbyaddr()

        if (isalpha(server_name[0])){
			// la direccion del server es un nombre
            hp = gethostbyname(server_name);
        }else{ 
			// convieto la direccion nnn.nnn a una usable
            addr = inet_addr(server_name);
            hp = gethostbyaddr((char *)&addr, 4, AF_INET);
        }

        if (hp == NULL){
			fprintf(stderr,"VDA: no encuentra direccion \"%s\": Error %d\n", server_name, WSAGetLastError());
            WSACleanup();
            exit(1);
        }

        // copio la informacion obtenida en una esrtuctura sockaddr_in
        memset(&server, 0, sizeof(server));
		server.sin_addr.s_addr=inet_addr(auxiliar->ipKSS);
        server.sin_family = AF_INET;
		server.sin_port = htons(*auxiliar->puertoKss);
    

        conn_socket = socket(AF_INET, socket_type, 0); // abro a socket
		
		if (conn_socket <0 ){
            fprintf(stderr,"VDA: no pude abrir el socket: Error %d\n", WSAGetLastError());
            WSACleanup();
			exit(1);
		}
		
		if (connect(conn_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR){
            fprintf(stderr,"Connect failed: %d\n", WSAGetLastError());
			printLog("Main VDA","0",0,"ERROR","Connect failed",logActivada);
			closesocket(conn_socket);
           WSACleanup();
	      _endthreadex( 0 );
        }
		
		strcpy(paqueteMPS->DescriptorID,generar_DescriptorID(paqueteMPS->DescriptorID));
		paqueteMPS->PayloadDescriptor = '3';
		paqueteMPS->PayloadLenght = 0;
		strcpy(paqueteMPS->Payload,"\0");

		send(conn_socket, (char *)paqueteMPS,21+paqueteMPS->PayloadLenght+1,0);
	
		recv(conn_socket,(char *) Buffer, sizeof(Buffer),0);
		
		strcpy(paqueteMPS->Payload,auxiliar->nombreVDA);
		paqueteMPS->PayloadLenght = strlen(paqueteMPS->Payload);
		
		send(conn_socket, (char *)paqueteMPS,21+paqueteMPS->PayloadLenght+1,0);
		
		recv(conn_socket,(char *) Buffer, sizeof(Buffer),0);
		response = (MPS_Package *)Buffer;
		
		if(response->PayloadDescriptor == '0'){
			perror("Error Autenticacion Handshake");
		    printLog("Main VDA","0",0,"ERROR","Error Autenticacion Handshake", logActivada);
			closesocket(conn_socket);
           WSACleanup();
	      _endthreadex( 0 );
		}
		
		
		while(r>0){ 
			r=recv(conn_socket,(char *) Buffer, sizeof(Buffer),0);
			if(r==-1){
				printf("Dio -1, el error es: %ld\n", WSAGetLastError());
				printLog("Main VDA","0",0,"ERROR","Error en la coneccion con KSS", logActivada);
			}
			strncpy(mensaje_aux,((MPS_Package*)Buffer)->Payload,9);
			if (!strcmp(mensaje_aux,"getCHS()")){
				CHS=getCHS();
				sprintf(paqueteMPS->Payload,"%d,%d,%d",CHS.pista,CHS.cabezal,CHS.sector);
				paqueteMPS->PayloadLenght=strlen(paqueteMPS->Payload);
				send(conn_socket, (char *)paqueteMPS,21+paqueteMPS->PayloadLenght+1,0);
			}else{
				strncpy(mensaje_aux,((MPS_Package*)Buffer)->Payload,11);
				mensaje_aux[11]='\0';
			}
			if (!strcmp(mensaje_aux,"putSectores")){
				//es putsectores
				for(i=12;i<1045;i++){
					stru_aux[i-12]=((MPS_Package*)Buffer)->Payload[i];
				}
				estructura = (struct infoGrabar*)stru_aux;
				estructura->dir1=abs(estructura->dir1);
				estructura->dir2=abs(estructura->dir2);
				if(estructura->dir1>(CHS.pista*CHS.sector) || estructura->dir2>(CHS.pista*CHS.sector)){
					strcpy(paqueteMPS->Payload,"No existe ese Sector!!");
					paqueteMPS->PayloadLenght=strlen(paqueteMPS->Payload);
					send(conn_socket, (char *)paqueteMPS,21+paqueteMPS->PayloadLenght+1,0);
				}else{
					putSectores(estructura,heapKSS, logActivada);
					strcpy(paqueteMPS->Payload,"OK");
					paqueteMPS->PayloadLenght=strlen(paqueteMPS->Payload);
					send(conn_socket, (char *)paqueteMPS,21+paqueteMPS->PayloadLenght+1,0);
				}
			}else{
				if (!strcmp(mensaje_aux,"getSectores")){//es get sectores
					for(i=12;i<1045;i++){
						stru_aux[i-12]=((MPS_Package*)Buffer)->Payload[i];
					}
					while(stru_aux[x-1]!=')'){
						while(stru_aux[x]!=',' && stru_aux[x]!=')'){
							aux[p]=stru_aux[x];	
							p++;x++;
						}
						aux[p]='\0';
						if(z==0){
							dir1=atoi(aux);
							p=0;
						}
						if(z==1){
							dir2=atoi(aux);
							p=0;
						}
						z++;x++;aux[p]='\0';
					}
					z=0;x=0;
				//strcpy(datos,getSectores(dir1,dir2);
				getSectores(dir1,dir2,dato,heapKSS, logActivada);
				memcpy(paqueteMPS->Payload,dato,1024);
				paqueteMPS->PayloadLenght=1024;
				send(conn_socket, (char *)paqueteMPS,21+paqueteMPS->PayloadLenght+1,0);
				}
			}
			if (strcmp(mensaje_aux,"getSectores") && strcmp(mensaje_aux,"putSectores") && strcmp(mensaje_aux,"getCHS()")){
					strcpy(paqueteMPS->Payload,"No existe ese comando!!");
					paqueteMPS->PayloadLenght=strlen(paqueteMPS->Payload);
					send(conn_socket, (char *)paqueteMPS,21+paqueteMPS->PayloadLenght+1,0);
			}
		}
		HeapDestroy(heapKSS);
        closesocket(conn_socket);
        WSACleanup();
	_endthreadex( 0 );
}

struct infoGrabar* Estructura(char *stru){
	int x=0,i=0,z=0;struct infoGrabar* estructura;
	char aux[1032];
	while(stru[x-1]!=')'){
		while(stru[x]!=',' && stru[x]!=')'){
		aux[i]=stru[x];	
		i++;x++;
		}
		aux[i]='\0';
		if(z==0){
			estructura->dir1=atoi(aux);
			i=0;
		}
		if(z==1){
			strcpy(estructura->dato1,aux);
			i=0;
		}
		if(z==2){
			estructura->dir2=atoi(aux);
			i=0;
		}
		if(z==3){
			strcpy(estructura->dato2,aux);
			i=0;
		}
		z++;x++;
	}
return(estructura);
}