#include <windows.h>
#include <string.h> 
#include <stdio.h>
#include <process.h>
#include "funcionesHandleFile.h"

char *obtenerMesEscrito(SYSTEMTIME creationTime){
	char mes[4],
		 aux[4];
	
	sprintf(aux,"%d", creationTime.wMonth);

	if(strcmp(aux, "1") == 0) {
		strcpy(mes, "Jan");
	}else if(strcmp(aux, "2") == 0) {
		strcpy(mes, "Feb");
	}else if(strcmp(aux, "3") == 0) {
		strcpy(mes, "Mar");
	}else if(strcmp(aux, "4") == 0) {
		strcpy(mes, "Apr");
	}else if(strcmp(aux, "5") == 0) {
		strcpy(mes, "May");
	}else if(strcmp(aux, "6") == 0) {
		strcpy(mes, "Jun");
	}else if(strcmp(aux, "7") == 0) {
		strcpy(mes, "Jul");
	}else if(strcmp(aux, "8") == 0) {
		strcpy(mes, "Aug");
	}else if(strcmp(aux, "9") == 0) {
		strcpy(mes, "Sep");
	}else if(strcmp(aux, "10") == 0) {
		strcpy(mes, "Oct");
	}else if(strcmp(aux, "11") == 0) {
		strcpy(mes, "Nov");
	}else if(strcmp(aux, "12") == 0) {
		strcpy(mes, "Dic");
	}

	return(mes);
}

char *obtenerFechaCreacion(WIN32_FIND_DATAA fileData){
	SYSTEMTIME pst;
	FILETIME ftime;
	char day[3],
		 month[4],
		 year[5],
		 response[15];
	
	FileTimeToLocalFileTime(&fileData.ftCreationTime.dwLowDateTime, &ftime);
	FileTimeToSystemTime(&ftime,&pst);

	sprintf(day, "%d", pst.wDay);
	strcpy(month, obtenerMesEscrito(pst));
	sprintf(year, "%d", pst.wYear);

	strcpy(response, month);
	strcat(response, " ");
	strcat(response, day);
	strcat(response, " ");
	strcat(response, year);

	return (response);
}

char *pasarArchivoAMensajeFTP(WIN32_FIND_DATAA fileData){
	char mensaje      [5000],
		 fileSize     [ 10];

	if((strcmp(fileData.cFileName,".") != 0) && (strcmp(fileData.cFileName,"..") != 0)){
		if(fileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY){
			strcpy(mensaje,"d");
		}else{
			strcpy(mensaje,"-");
		}
		strcat(mensaje, "rwxrwxrwx 1 ftp ftp ");

		sprintf(fileSize, "%d", fileData.nFileSizeLow);
		strcat(mensaje, fileSize);
		
		strcat(mensaje, " ");

		strcat(mensaje, obtenerFechaCreacion(fileData));
		
		strcat(mensaje, " ");
		strcat(mensaje, fileData.cFileName);
		strcat(mensaje, "\n");

		return(mensaje);
	}
}

void borrarArchivo(char *currentPath, char *nombreArchivoABorrar){
	HANDLE fileHandle;
	WIN32_FIND_DATAA fileData;
	int encontroArchivo = 0; 

	strcat(currentPath, nombreArchivoABorrar);

	DeleteFileA(currentPath);
}

char *getDirectorioNuevo(char *ftpPath){
	char aux[50], 
		 path[50];
	int indice = 0,
		indAux = 0;

	strcpy(aux, ftpPath);

	while((aux[indice] != '\0') && (aux[indice] != '\r')){
		if(indice > 0 || aux[indice] != '/'){
			if(aux[indice] != '/'){
				path[indAux] = aux[indice];
				indAux++;
			}else{
				path[indAux] = '\\';
				indAux++;
			}
		}
		indice ++;
	}
	path[indAux] = '\0';
	return (path);
}

char *realizarCambioDeDirectorio(char *originalPath, char *ftpPath){
	char auxPath[100],
		 ftp[50];

	strcpy(auxPath, originalPath);
	strcpy(ftp, getDirectorioNuevo(ftpPath));
	strcat(auxPath, ftp);

	return(auxPath);
}

void leerArchivosDeCarpeta(char *path, char *mensaje){
	HANDLE fileHandle;
	WIN32_FIND_DATAA fileData;
	char pathAux[100];
	
	strcpy(pathAux, path);
	strcat(pathAux, "*.*");
	strcpy(mensaje, "");

	fileHandle = FindFirstFileA(pathAux, &fileData);

	do {
		if((strcmp(fileData.cFileName,".") != 0) && (strcmp(fileData.cFileName,"..") != 0)){
			strcat(mensaje, pasarArchivoAMensajeFTP(fileData));
		}
	} while (FindNextFileA(fileHandle, &fileData) != 0);
	strcat(mensaje, "\r\n");
	printf("%s", mensaje);
}

void leerArchivoParaObtenerData(char *path, char *bufferDef){
	HANDLE *archivo;
	DWORD bytesLeidos;
	char buffer[100];
	int lectura;

	//apertura archivo
	archivo = CreateFileA (path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (archivo == INVALID_HANDLE_VALUE){
		return;
	}

	//lectura archivo
	lectura = ReadFile (archivo, buffer, sizeof(buffer), &bytesLeidos, NULL);

	if (lectura == 0) {
		return;
	}

	CloseHandle(archivo);

	memcpy(bufferDef, buffer, bytesLeidos);
}

void *getDataFromFile(char *originalPath, char *archivo, char *buffer){
	char pathFinal[100];

	strcpy(pathFinal, realizarCambioDeDirectorio(originalPath, archivo));
	
	leerArchivoParaObtenerData(pathFinal, buffer);

}

char *getSizeOfFile (char *originalPath, char *archivo){
	char pathFinal[100];
	char size[10];
	HANDLE fileHandle;
	WIN32_FIND_DATAA fileData;
	int encontreElArchivo = 0;
	char pathAux[100];
	
	strcpy(pathAux, originalPath);
	strcat(pathAux, "*.*");

	fileHandle = FindFirstFileA(pathAux, &fileData);


	do {
		if(strcmp(fileData.cFileName, archivo) == 0){
			sprintf(size, "%d", fileData.nFileSizeLow);
			encontreElArchivo = 1; 
		}
	} while ((FindNextFileA(fileHandle, &fileData) != 0) && (!encontreElArchivo));
	
	return(size);
}

