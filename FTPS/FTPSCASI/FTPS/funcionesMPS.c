#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include "funcionesMPS.h"
#include "funcionesLog.h"



// genera un descriptor aleatorio y lo guarda en DescriptorID que debe ser char [16]
void generar_DescriptorID(char *DescriptorID){
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
	
}

int conectarConKernel(char *IPKSS, unsigned puertoKSS, MPS_Package *paqueteMPS){
	int a;
	int desc;

	char Buffer[1200];

	struct sockaddr_in *lcl_addr = HeapAlloc(GetProcessHeap(), 0, sizeof(struct sockaddr_in));
	struct sockaddr_in *rmt_addr = HeapAlloc(GetProcessHeap(), 0, sizeof(struct sockaddr_in));
	WSADATA wsaData;

	MPS_Package *response = HeapAlloc(GetProcessHeap(),0,sizeof(MPS_Package));

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
	rmt_addr->sin_addr.s_addr = inet_addr(IPKSS);
	rmt_addr->sin_port = htons(puertoKSS);

	a = connect(desc,(struct sockaddr*)rmt_addr, sizeof(struct sockaddr_in));

	generar_DescriptorID(paqueteMPS->DescriptorID);
	paqueteMPS->PayloadDescriptor = '1';
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
	return 0;*/
}