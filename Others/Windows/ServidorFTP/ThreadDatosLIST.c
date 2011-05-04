#include <stdio.h>
#include <winsock2.h>
#pragma comment (lib, "Ws2_32.lib")
#include <windows.h>
#include <string.h>

#define SOCKET_MAX_BUFFER 100
WSADATA wsaData;

int main() {

	SOCKET sd, asd;
	struct sockaddr_in dirRemota;
	int n,a;
	char buffer[100];
	int corrector;

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
		sinInterface.sin_port = htons(5300);
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
    send(asd,"-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo1.txt\r\n",strlen("-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo1.txt\r\n"),0);
    send(asd,"-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo2.txt\r\n",strlen("-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo2.txt\r\n"),0);
    send(asd,"drwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 directorio1\r\n",strlen("drwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 directorio1\r\n"),0);

	Sleep(2000);

	closesocket(asd);
	closesocket(sd);

    WSACleanup();
}