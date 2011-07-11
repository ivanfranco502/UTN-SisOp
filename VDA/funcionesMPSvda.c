#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include "funcionesMPSvda.h"
//#include "funcionesLog.h"



// genera un descriptor aleatorio y lo guarda en DescriptorID que debe ser char [16]
char *generar_DescriptorID(char *DescriptorID){
	time_t tiempo;
	char tpo[11], num[6];
	long aleatorio;
	
	strcpy(DescriptorID, "");
	srand(time(NULL));
	time(&tiempo);
	aleatorio = rand() % 90000;
	aleatorio += 10000;
	sprintf(tpo,"%d",tiempo);;
	sprintf(num,"%ld",aleatorio);
	//itoa(tiempo, tpo, 10);
	//ltoa(aleatorio, num, 10);
	strcat(DescriptorID, tpo);
	strcat(DescriptorID, num);
	return(DescriptorID);
}
/*
int conectarConKernel(void){
	int a;
	int desc;

	char Buffer[1200];

	struct sockaddr_in *lcl_addr = HeapAlloc(GetProcessHeap(), 0, sizeof(struct sockaddr_in));
	struct sockaddr_in *rmt_addr = HeapAlloc(GetProcessHeap(), 0, sizeof(struct sockaddr_in));
	WSADATA wsaData;

	MPS_Package *response = HeapAlloc(GetProcessHeap(),0,sizeof(MPS_Package));
	MPS_Package *paqueteMPS = HeapAlloc(GetProcessHeap(),0,sizeof(MPS_Package));

	if ((a = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		perror("Error Wsa");
		return 255;
	}
	
	lcl_addr->sin_family = AF_INET;
	lcl_addr->sin_addr.s_addr = INADDR_ANY;
	lcl_addr->sin_port = htons(7500);

	desc = socket(AF_INET, SOCK_STREAM, 0);

	bind(desc,(struct sockaddr*)lcl_addr,sizeof(struct sockaddr_in));
	
	rmt_addr->sin_family = AF_INET;
	rmt_addr->sin_addr.s_addr = INADDR_ANY;
	rmt_addr->sin_port = htons(5300);

	while(!(connect(desc,(struct sockaddr*)rmt_addr, sizeof(struct sockaddr_in))));

	generar_DescriptorID(paqueteMPS->DescriptorID);
	paqueteMPS->PayloadDescriptor = '3';
	paqueteMPS->PayloadLenght = 0;
	strcpy(paqueteMPS->Payload,"\0");

	send(desc, (char *)paqueteMPS,sizeof(MPS_Package)+1,0);
	
	//listen(desc,100);
	recv(desc, Buffer, sizeof(Buffer),0);

	response = (MPS_Package *)Buffer;
//	if(response->PayloadDescriptor == '1'){
		return desc;		
/*	}else{
		perror("Error Autenticacion Handshake");
		return -1;
	}

//	WSACleanup();
	return 0;
}*/
SOCKET conectarConKernel(void){
	
		char Buffer[128];
        char *server_name= "localhost";
        unsigned short port = 21;
        int retval, loopflag = 0;
        unsigned int addr;
        int socket_type = SOCK_STREAM;
        struct sockaddr_in server;
        struct hostent *hp;
		char c[10]="hola";
        WSADATA wsaData;
        SOCKET  conn_socket;
		int x;

		MPS_Package *response = HeapAlloc(GetProcessHeap(),0,sizeof(MPS_Package));
		MPS_Package *paqueteMPS = HeapAlloc(GetProcessHeap(),0,sizeof(MPS_Package));
	
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
        memcpy(&(server.sin_addr), hp->h_addr, hp->h_length);
        server.sin_family = hp->h_addrtype;
        server.sin_port = htons(port);
    

        conn_socket = socket(AF_INET, socket_type, 0); // abro a socket
       
		if (conn_socket <0 ){
            fprintf(stderr,"VDA: no pude abrir el socket: Error %d\n", WSAGetLastError());
            WSACleanup();
			exit(1);
		}

		if (connect(conn_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR){
            fprintf(stderr,"Connect failed: %d\n", WSAGetLastError());           
			WSACleanup();
			exit(1);
        }
		
		strcpy(paqueteMPS->DescriptorID,generar_DescriptorID(paqueteMPS->DescriptorID));
		paqueteMPS->PayloadDescriptor = '3';
		paqueteMPS->PayloadLenght = 0;
		strcpy(paqueteMPS->Payload,"\0");

		send(conn_socket, (char *)paqueteMPS,sizeof(MPS_Package)+1,0);
	
		strcpy(paqueteMPS->Payload,"VDA1");
		paqueteMPS->PayloadLenght = 4;

		send(conn_socket, (char *)paqueteMPS,sizeof(MPS_Package)+1,0);
		
		recv(conn_socket,(char *) Buffer, sizeof(Buffer),0);
		response = (MPS_Package *)Buffer;
		
		if(response->PayloadDescriptor != '0'){
			return conn_socket;		
		}else{
			perror("Error Autenticacion Handshake");
			exit(1);
		}
		
}