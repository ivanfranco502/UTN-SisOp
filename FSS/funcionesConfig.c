#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void obtenerStructFSS(char *buffer, configFSS *configuracionInicial){
	char variableParametro[100];
	int contadorBuffer=0;

	//IPKSS
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	strcpy(configuracionInicial->ipKSS, variableParametro);
	//puertoKSS
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	sscanf(variableParametro, "%d", &(configuracionInicial->puertoKSS));
	//logActivada
	contadorBuffer = obtenerParametroParaStruct(buffer, variableParametro, contadorBuffer);
	sscanf(variableParametro, "%d", &(configuracionInicial->logActivado));
}

void getConfigFSS(configFSS *config){
	char *buffer;
	long size;
	FILE *archivo;

	//apertura archivo
	archivo =  fopen("fss.config", "r" );

	fseek (archivo , 0 , SEEK_END);
 	size = ftell (archivo);
  	rewind (archivo);

	//lectura archivo
	buffer = (char*)malloc(sizeof(char)*size);

	fread(buffer, 1, size, archivo);

	fclose(archivo);

	obtenerStructFSS(buffer, config);//funcion que se le pasa el buffer y el structFTP
	free(buffer);
}
