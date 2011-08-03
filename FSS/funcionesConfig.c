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
	while((buffer[contadorBuffer]!= '\n') && (buffer[contadorBuffer]!='\0')){
		variableParametro[contadorParametro] = buffer[contadorBuffer];
		contadorParametro++;
		contadorBuffer++;
	}
	variableParametro[contadorParametro]= '\0';
	return(contadorBuffer);
}

void obtenerStructKSS(char *buffer, configKSS *configuracionInicial){
	char variableParametro[100];
	int contadorBuffer=0;

	//puertoFTPS
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	sscanf(variableParametro, "%d", &(configuracionInicial->puertoFTPS));
	//puertoVDA
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	sscanf(variableParametro, "%d", &(configuracionInicial->puertoVDA));
	//logActivada
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	sscanf(variableParametro, "%d", &(configuracionInicial->logActivado));
}

void getConfigKSS(configKSS *config){
	char *buffer;
	long size;
	FILE *archivo;

	//apertura archivo
	archivo =  fopen("kss.config", "r" );

	fseek (archivo , 0 , SEEK_END);
 	size = ftell (archivo);
  	rewind (archivo);

	//lectura archivo
	buffer = (char*)malloc(sizeof(char)*size);

	fread(buffer, 1, size, archivo);

	fclose(archivo);

	obtenerStructKSS(buffer, config);//funcion que se le pasa el buffer y el structFTP
	free(buffer);
}
