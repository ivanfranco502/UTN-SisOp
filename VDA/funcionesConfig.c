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

char *getIPKSS(char *buff){
	char IP[16];
	int i=0;
	int j=0,
		pasoCentinelas = 0;

	while (pasoCentinelas < 3){
		if(buff[i] == '>'){
			pasoCentinelas++;
		}
		i++;
	}
	while (pasoCentinelas < 4){
		if(buff[i] != '>'){
			IP[j] = buff[i];
			j++;
		}else{
			pasoCentinelas++;
		}
		i++;
	}
	IP[j]='\0';
	return IP;
}

char *getPortKSS(char *buff){
	char PORT[6];
	int i=0;
	int j=0,
		pasoCentinelas = 0;

	while (pasoCentinelas < 4){
		if(buff[i] == '>'){
			pasoCentinelas++;
		}
		i++;
	}
	while (buff[i]!='\0'){
		if(buff[i] != '>'){
			PORT[j] = buff[i];
			j++;
		}else{
			pasoCentinelas++;
		}
		i++;
	}
	PORT[j]='\0';
	return PORT;
}
void getConfigKSS (char *IP, unsigned puerto){
	HANDLE *archivo;
	DWORD bytesLeidos;
	LPCSTR nombreArchivo = "ftp.config";
	char buffer[100];
	char port[6];
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


	strcpy(IP, getIPKSS (buffer));
	strcpy(port,getPortKSS(buffer));
	sscanf(port, "%d", &puerto);
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