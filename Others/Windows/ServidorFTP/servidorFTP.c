// ServidorFTP.c : Defines the entry point for the console application.
//

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
	char nomArchivo[20];
	char texto[100];
	int corrector, pasoComando, aux;

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

	send(asd,"220 POWER\r\n",strlen("220 POWER\r\n"),0);
	printf("le mande el 220 \n");
	corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
	buffer[corrector]='\0';
	printf("%s\n",buffer);

	send(asd,"230 POWER\r\n",strlen("230 POWER\r\n"),0);
    printf("le mande el 230 \n");
    corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
    buffer[corrector]='\0';
    printf("%s\n",buffer);

	while(strcmp(buffer,"PWD\r\n")){

		send(asd,"500 NO PERMITIDO\r\n",strlen("500 NO PERMITIDO\r\n"),0);
		printf("le mande el 500 \n");
		corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
		buffer[corrector]='\0';
		printf("%s\n",buffer);
	
		Sleep(2000);
	}

	send(asd,"257 \"/\" Is the current directory \r\n",strlen("257 \"/\" Is the current directory \r\n"),0);
    printf("le mande el 257 \n");
    corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
    buffer[corrector]='\0';
    printf("%s\n",buffer);

	while(strcmp(buffer,"TYPE A\r\n")){
  		send(asd,"500 NO PERMITIDO\r\n",strlen("500 NO PERMITIDO\r\n"),0);
    	printf("le mande el 500 \n");
    	corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
    	buffer[corrector]='\0';
    	printf("%s\n",buffer);

		Sleep(1000);
	}
    
	send(asd,"200 Type set to A\r\n",strlen("200 Type set to A\r\n"),0);
    printf("le mande el 200 \n");
    corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
    buffer[corrector]='\0';
    printf("%s\n",buffer);

	Sleep(5000);

	send(asd,"227 Entering Pasive Mode (192,168,1,108,20,180)\r\n",strlen("227 Entering Pasive Mode (192,168,1,108,20,180)\r\n"),0);
    printf("le mande el 227 \n");
//	data_client = accept (data_socket, (struct sockaddr *)remote_address, &addrlen);
    corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
    buffer[corrector]='\0';
    printf("%s\n",buffer);

	Sleep(2000);

	Sleep(10000);

	send(asd,"150 Openning ASCII mode\r\n",strlen("150 Openning ASCII mode\r\n"),0);
    printf("le mande el 150 \n");
//                      data_client = accept (data_socket, (struct sockaddr *)remote_address, &addrlen);
//                      corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
//                      buffer[corrector]='\0';
//                      printf("%s\n",buffer);

 	Sleep(10000);

	send(asd,"226 Transfer Complete\r\n",strlen("226 Transfer Complete\r\n"),0);
                        printf("le mande el 226 \n");
//                      data_client = accept (data_socket, (struct sockaddr *)remote_address, &addrlen);
	Sleep(10000);     

	corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
    buffer[corrector]='\0';
    printf("%s\n",buffer);

	send(asd,"200 NOOP command successful\r\n",strlen("200 NOOP command successful\r\n"),0);
    printf("le mande el 200 \n");
    corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
    buffer[corrector]='\0';
    printf("%s\n",buffer);

	send(asd,"250 CWD command successful\r\n",strlen("250 CWD command successful\r\n"),0);
	printf("le mande el 250 \n");
	corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
	buffer[corrector]='\0';
	printf("%s\n",buffer);

	Sleep(2000);

	send(asd,"200 Type set to I\r\n",strlen("200 Type set to I\r\n"),0);
	printf("le mande el 200 \n");
	corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
	buffer[corrector]='\0';
	printf("%s\n",buffer);

	Sleep(2000);

	send(asd,"227 Entering Passive Mode(192,168,1,108,20,181).\r\n",strlen("227 Entering Passive Mode(192,168,1,108,20,181).\r\n"),0);
    printf("le mande el 227 \n");
    corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
    buffer[corrector]='\0';
    printf("%s\n",buffer);
	
	Sleep(2000);

	send(asd,"500 no mandamos tamaño de archivo\r\n",strlen("500 no mandamos tamaño de archivo\r\n"),0);
    printf("le mande el 500 \n");
    corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
    buffer[corrector]='\0';
    printf("%s\n",buffer);

	Sleep(2000);
 
	n = 0;
	pasoComando = 0;
	aux = 0;
	for(aux =0; aux < 20; aux++){
		nomArchivo[aux] = '\0' ;
	}
	aux = 0;
	while (buffer[n] != '\0'){
		if (buffer[n] == ' '){
			pasoComando = 1;
		}
		if (pasoComando) {
			nomArchivo[aux]= buffer[n];
		}
		aux++;
		n++;
	}
	strcpy(texto,"150 Opening BINARY mode data connection for ");
	strcat(texto,nomArchivo);
	strcat(texto,"(63805.txt)\r\n");
	send(asd,texto,strlen(texto),0);
    printf("le mande el 150 \n");
    //corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
    //buffer[corrector]='\0';
    //printf("%s\n",buffer);
	
	Sleep(2000);

	send(asd,"226 Transfer Complete\r\n",strlen("226 Transfer Complete\r\n"),0);
    printf("le mande el 226 \n");
    
	Sleep(10000);

	corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
    buffer[corrector]='\0';
    printf("%s\n",buffer);

	send(asd,"200 NOOP command successful\r\n",strlen("200 NOOP command successful\r\n"),0);
    printf("le mande el 200 \n");
    corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
    buffer[corrector]='\0';
    printf("%s\n",buffer);

	send(asd,"250 CWD command successful\r\n",strlen("250 CWD command successful\r\n"),0);
	printf("le mande el 250 \n");
	corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
	buffer[corrector]='\0';
	printf("%s\n",buffer);

	while(strcmp(buffer,"TYPE I\r\n")){
		send(asd,"250 CWD command successful\r\n",strlen("250 CWD command successful\r\n"),0);
		printf("le mande el 250 \n");
		corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
		buffer[corrector]='\0';
		printf("%s\n",buffer);
	}

	Sleep(2000);

	send(asd,"200 Type set to I\r\n",strlen("200 Type set to I\r\n"),0);
	printf("le mande el 200 \n");
	corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
	buffer[corrector]='\0';
	printf("%s\n",buffer);

	Sleep(2000);

	send(asd,"227 Entering Passive Mode(192,168,1,108,20,182).\r\n",strlen("227 Entering Passive Mode(192,168,1,108,20,182).\r\n"),0);
    printf("le mande el 227 \n");
    corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
    buffer[corrector]='\0';
    printf("%s\n",buffer);
	
	Sleep(2000);

	n = 0;
	pasoComando = 0;
	aux = 0;
	for(aux =0; aux < 20; aux++){
		nomArchivo[aux] = '\0' ;
	}
	aux = 0;
	while (buffer[n] != '\0'){
		if (buffer[n] == ' '){
			pasoComando = 1;
		}
		if (pasoComando) {
			nomArchivo[aux]= buffer[n];
		}
		aux++;
		n++;
	}
	strcpy(texto,"150 Opening BINARY mode data connection for ");
	strcat(texto,nomArchivo);
	strcat(texto,"\r\n");
	send(asd,texto,strlen(texto),0);
    printf("le mande el 150 \n");

	Sleep(2000);

	send(asd,"226 Transfer Complete\r\n",strlen("226 Transfer Complete\r\n"),0);
    printf("le mande el 226 \n");

	Sleep(10000);

	corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
	buffer[corrector]='\0';
	printf("%s\n",buffer);

	send(asd,"200 Type set to I\r\n",strlen("200 Type set to I\r\n"),0);
	printf("le mande el 200 \n");
	corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
	buffer[corrector]='\0';
	printf("%s\n",buffer);

	n = 0;
	pasoComando = 0;
	aux = 0;
	for(aux =0; aux < 20; aux++){
		nomArchivo[aux] = '\0' ;
	}
	aux = 0;
	while (buffer[n] != '\0'){
		if (buffer[n] == ' '){
			pasoComando = 1;
		}
		if (pasoComando) {
			nomArchivo[aux]= buffer[n];
		}
		aux++;
		n++;
	}
	str
	strcpy(texto,"257 \"");
	strcat(texto,nomArchivo);
	strcat(texto,"\" is the current directory\r\n");
    //corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
    //buffer[corrector]='\0';
    //printf("%s\n",buffer);
	
	send(asd,"250 CWD command successful\r\n",strlen("250 CWD command successful\r\n"),0);
	printf("le mande el 250 \n");
	corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
	buffer[corrector]='\0';
	printf("%s\n",buffer);

	Sleep(2000);

	send(asd,"200 Type set to A\r\n",strlen("200 Type set to A\r\n"),0);
	printf("le mande el 200 \n");
	corrector=recv(asd,buffer,SOCKET_MAX_BUFFER,0);
	buffer[corrector]='\0';
	printf("%s\n",buffer);

	send(asd,texto,strlen(texto),0);
    printf("le mande el 257 \n");

	Sleep(5000);

	closesocket(asd);
	closesocket(sd);
	WSACleanup();
}