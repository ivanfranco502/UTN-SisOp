#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "funcionesConfig.h"

char *dameIP (char *buff) {
	char IP[16];
	int i=0;
	
	while (buff[i] != '>'){
		IP[i]=buff[i];
		i++;
	}
	IP[i]='\0';
	return IP;
}

char *getPath(char * buff){
	char path[100];
	int indice = 0,
		aux = 0,
		pasoCentinelaPath = 0;

	while(buff[indice] != '\0'){
		if(buff[indice] == '>'){
			pasoCentinelaPath++;
		}
		if((pasoCentinelaPath == 2) && (buff[indice] != '\0') && (buff[indice] != '>')){
			path[aux] = buff[indice];
			aux++;
		}
		indice++;
	}
	path[aux] = '\0';
	
	return(path);
}

char *damePuerto(char *buff){
	char PORT[6];
	int i=0;
	int j=0,
		pasoCentinelas = 0;

	while (buff[i] != '>'){
		i++;
	}
	while ((buff[i] != '\0') || (pasoCentinelas < 2)){
		if((buff[i] != '>') && (pasoCentinelas < 2)){
			PORT[j]=buff[i];
			j++;
		}
		if(buff[i] == '>'){
			pasoCentinelas++;
		}
		i++;
	}
	PORT[j]='\0';
	return PORT;
}

void getConfigPath (char *PATH){
	HANDLE *archivo;
	DWORD bytesLeidos;
	LPCSTR nombreArchivo = "ftp.config";
	char buffer[100];
	int lectura;

	//apertura archivo
	archivo = CreateFileA (nombreArchivo, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (archivo == INVALID_HANDLE_VALUE){
		return;
	}

	//lectura archivo
	lectura = ReadFile (archivo, buffer, sizeof(buffer), &bytesLeidos, NULL);

	buffer[bytesLeidos] = '\0';

	if (lectura == 0) {
		return;
	}

	CloseHandle(archivo);

	strcpy(PATH, getPath (buffer));
}

void getConfigFTP (char *IP, char *PORT){
	HANDLE *archivo;
	DWORD bytesLeidos;
	LPCSTR nombreArchivo = "ftp.config";
	char buffer[100];
	int lectura;

	//apertura archivo
	archivo = CreateFileA (nombreArchivo, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (archivo == INVALID_HANDLE_VALUE){
		return;
	}

	//lectura archivo
	lectura = ReadFile (archivo, buffer, sizeof(buffer), &bytesLeidos, NULL);

	buffer[bytesLeidos] = '\0';

	if (lectura == 0) {
		return;
	}

	CloseHandle(archivo);

	strcpy(IP, dameIP (buffer));
	strcpy(PORT, damePuerto (buffer));
}