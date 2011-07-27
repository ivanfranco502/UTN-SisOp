#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "funcionesConfig.h"

int obtenerParametroParaStruct(char *buffer, char *variableParametro, int arrancaContadorBuffer){
	int contadorBuffer,
		contadorParametro=0;
	
	contadorBuffer = arrancaContadorBuffer;

	while(buffer[contadorBuffer]!= '='){
		contadorBuffer++;
	}
	contadorBuffer++;
	while((buffer[contadorBuffer]!= '\n') && (buffer[contadorBuffer]!='\0') && (buffer[contadorBuffer]!='\r')){
		variableParametro[contadorParametro] = buffer[contadorBuffer];
		contadorParametro++;
		contadorBuffer++;
	}
	variableParametro[contadorParametro]= '\0';
	return(contadorBuffer);
}

void obtenerStructFTP(char *buffer, configFTP *configuracionInicial){
	char variableParametro[100];
	int contadorBuffer=0;

	//ipServer
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	strcpy(configuracionInicial->IPServer, variableParametro);
	//puertoServer
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	sscanf(variableParametro, "%d", &configuracionInicial->puertoServer);
	//ipKernel
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	strcpy(configuracionInicial->IPKernel, variableParametro);
	//puertoKernel
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	sscanf(variableParametro, "%d", &configuracionInicial->puertoKernel);
	//puertoFTPAKernel
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	sscanf(variableParametro, "%d", &configuracionInicial->puertoFTPAKernel);
}

void getConfigFTP(configFTP *config){
	HANDLE *archivo;
	DWORD bytesLeidos;
	LPCSTR nombreArchivo = "ftp.config";
	char buffer[300];
	int lectura;
	char variableAuxiliarConfig[16]; //variable que me sirve para obtener los parametros

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

	obtenerStructFTP(buffer, config);//funcion que se le pasa el buffer y el structFTP
}