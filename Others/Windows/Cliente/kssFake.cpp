/* Ejemplo de programa cliente */

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 

#pragma comment(lib, "Ws2_32.lib")


#define PUERTO 21

#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )

PACK(typedef struct{
        char DescriptorID[16];   //unico, si usamos la hora + un random es suficiente.
        char PayloadDescriptor;  //tipo de operacion (handshake, vda, fss, ftp, list, cd, etc)
        int PayloadLenght;  //longitud del payload, 0 si no enviamos nada.
        char Payload[1050];  //mensaje.
}MPS_Package);

struct buffer
{
char *dato1;
char *dato2;
	
};
struct infoGrabar
{
long dir1;
char dato1[512];
long dir2;
char dato2[512];
int accion;
};



int main(){
	char Buffer[1200];
	long escrito;
	struct infoGrabar *buffer;
	struct buffer *datos;
	HANDLE  heap = HeapCreate(HEAP_NO_SERIALIZE, 1024*1024, 0);
	int corrector;
	SOCKET descriptor;
	int *remoteClient;
	int r=-1,i, a, addrlen = sizeof(struct sockaddr_in);
	 WSADATA wsaData;

	MPS_Package *response = HeapAlloc(GetProcessHeap(),0,sizeof(MPS_Package));
	MPS_Package *paqueteMPS = HeapAlloc(GetProcessHeap(),0,sizeof(MPS_Package));

	 struct sockaddr_in *local_address = HeapAlloc(GetProcessHeap(), 0, addrlen);
    struct sockaddr_in *remote_address = HeapAlloc(GetProcessHeap(), 0, addrlen);
	
	remoteClient = (int *) HeapAlloc(GetProcessHeap(), 0, sizeof(int));

	if ((a = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		printf("WSAStartup() devolvio error nro %d\n", a );
		return 255;
	}
	
	
    local_address->sin_family = AF_INET;
	local_address->sin_addr.s_addr=INADDR_ANY;
	local_address->sin_port = htons (5300);

	descriptor= socket(AF_INET, SOCK_STREAM, 0);
	bind (descriptor,(struct sockaddr *) local_address, addrlen);
	listen(descriptor,100);
	printf("Esperando que se conecte\n");
	while((*remoteClient = accept (descriptor, (struct sockaddr *)remote_address, (void*)&addrlen))== -1){
		//printf("%d",*remoteClient = accept (descriptor, (struct sockaddr *)remote_address, (void*)&addrlen));
	}
	/* aca va el send y rcv de datos */
	printf("Se conecto el vda\n");
	
	recv(*remoteClient,(char *)Buffer,sizeof(Buffer),0);
	response = (MPS_Package *)Buffer;

	strcpy(paqueteMPS->Payload,"OK");
	paqueteMPS->PayloadLenght=3;

	send(*remoteClient, (char *)paqueteMPS,21+paqueteMPS->PayloadLenght+1,0);
	
	/*recv(*remoteClient,(char *)Buffer,sizeof(Buffer),0);
	
	response = (MPS_Package *)Buffer;
	printf("%s",response->Payload);
	strcpy(paqueteMPS->DescriptorID,response->DescriptorID);
	paqueteMPS->PayloadDescriptor = '1';
	paqueteMPS->PayloadLenght = 0;
	strcpy(paqueteMPS->Payload,"\0");
	
	send(*remoteClient, (char *)paqueteMPS,21+paqueteMPS->PayloadLenght+1,0);
	*/
	while(r!=0){
		printf("\nMandar mensaje piola\n");
		gets(paqueteMPS->Payload);
		paqueteMPS->PayloadLenght=strlen(paqueteMPS->Payload);
		send(*remoteClient,(char *) paqueteMPS,21+paqueteMPS->PayloadLenght+1,0);
		
		r=recv(*remoteClient,(char *)Buffer,sizeof(Buffer),0);
		response = (MPS_Package *)Buffer;
		datos=(struct buffer *)response->Payload;	
		printf("Datos:%s\n",response->Payload);
	}
	HeapDestroy(heap);

}