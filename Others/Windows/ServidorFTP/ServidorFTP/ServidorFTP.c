// ServidorFTP.c : Defines the entry point for the console application.
//

#include "stdafx.h"


#include <stdio.h>
#include <winsock2.h>
#pragma comment (lib, "Ws2_32.lib")
#include <windows.h>
#include <string.h>


WSADATA wsaData;

int main() {

	SOCKET sd, asd;
	struct sockaddr_in dirRemota;
	int n,a;
	char Buffer[1025];

	if ((a = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		printf("WSAStartup() devolvio error nro %d\n", a );
		return 255;
	}

	printf("Inicializando Listener\n");

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd != INVALID_SOCKET) {
		SOCKADDR_IN sinInterface;
		sinInterface.sin_family = AF_INET;
		sinInterface.sin_addr.s_addr = inet_addr("0.0.0.0");
		sinInterface.sin_port = htons(21);
		if (bind(sd, (struct sockaddr*)&sinInterface, sizeof(SOCKADDR_IN)) != SOCKET_ERROR) {
			listen(sd, 1);
		} else {
			printf("Error bindeando");
			return 1;
		}
	} else {
		printf("Error creando Socket");
		return 2;
	}

	// Wait for a connection, and accepting it when one arrives.
	printf("Esperando conexion de cliente FTP\n");


	a = sizeof(dirRemota);
	asd = accept(sd, (struct sockaddr *)&dirRemota, &a);

	if (asd != INVALID_SOCKET) {
		printf("Conexion aceptada de: %s:%d\n", 
		inet_ntoa(dirRemota.sin_addr), 
		ntohs(dirRemota.sin_port) );
	}
	else {
		printf("Error aceptando conexion\n");
		return 3;
	}
	send(asd, "220 FTP Prendido\n",strlen("220 SFTP ON")+1, 0);
	recv(asd, Buffer, 1025, 0); 
	while (strcmp(Buffer, "\0")==0){
		recv(asd, Buffer, 1025, 0);
	}
	printf("El socket %d recibio: %s\n", asd, Buffer);
	if (strcmp(Buffer, "USER anonimo")==0){
		send(asd, "230 USER anonimo logueado", strlen("230 USER anonimo logueado")+1, 0);
		printf("Se mando todo");
	}
	WSACleanup();
}