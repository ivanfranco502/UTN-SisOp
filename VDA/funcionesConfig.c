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

void obtenerStructVDA(char *buffer, configVDA *configuracionInicial){
	char *variableParametro[50];
	int contadorBuffer=0;

	//nombreVDA
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	strcpy(configuracionInicial->nombreVDA, variableParametro);
	//ipKSS
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	strcpy(configuracionInicial->ipKSS, variableParametro);
	//puertoKSS
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	sscanf(variableParametro, "%d", &configuracionInicial->puertoKss);
	//cilindros
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	sscanf(variableParametro, "%d", &configuracionInicial->cilindros);
	//headers
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	sscanf(variableParametro, "%d", &configuracionInicial->headers);
	//sectores
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	sscanf(variableParametro, "%d", &configuracionInicial->sectores);
	//retardoRotacional
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	sscanf(variableParametro, "%d", &configuracionInicial->retardoRotacional);
	//posicionCabezal
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	sscanf(variableParametro, "%d", &configuracionInicial->posicionCabezal);
	//cacheActivada
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	sscanf(variableParametro, "%d", &configuracionInicial->cacheActivada);
}

void getConfigVDA(configVDA *config){
	HANDLE *archivo;
	DWORD bytesLeidos;
	LPCSTR nombreArchivo = "vda.config";
	char buffer[300];
	int lectura;
	char variableAuxiliarConfig[100]; //variable que me sirve para obtener los parametros

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

	obtenerStructVDA(buffer, config);//funcion que se le pasa el buffer y el structVDA
}