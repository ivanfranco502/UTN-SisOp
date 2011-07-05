#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include <string.h> 
#include <stdio.h>
#include <process.h>
#include "funcionesHandleFile.h"

int enviarSyscallOpen(char *arg, int socketKSS, char *modoApertura){
	char payload[50];
	char IDpaquete[16];
	char buffer[100];
	int fileDescriptor;
	MPS_Package *paqueteSyscall = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, sizeof(MPS_Package));
	
	generar_DescriptorID(paqueteSyscall->DescriptorID);
	strcpy(IDpaquete, paqueteSyscall->DescriptorID);
	paqueteSyscall->PayloadDescriptor = '1';
	strcpy(payload, "sys_open(");
	strcat(payload, modoApertura);
	strcat(payload,",");
	strcat(payload, arg);
	strcat(payload, ")");
	strcpy(paqueteSyscall->Payload,payload);
	paqueteSyscall->PayloadLenght = strlen(payload);
	
	send(socketKSS,(char *)paqueteSyscall, sizeof(MPS_Package)+1,0);
	//listen(socketKSS,100);
	recv(socketKSS, buffer, sizeof(buffer), 0);
	
	paqueteSyscall = (MPS_Package *)buffer;
	if(strcmp(paqueteSyscall->DescriptorID, IDpaquete) == 0){
		if(paqueteSyscall->PayloadDescriptor == '1'){
			sscanf(paqueteSyscall->Payload,"%d", &fileDescriptor);
			return fileDescriptor;
		}else{
			return -1;
		}
	}else{
		return -1;
	}
}

int enviarSyscallClose(int fileDescriptor, int socketKSS){
	char payload[50],
		 IDpaquete[16],
		 buffer[100],
		 FDescriptor[5];
	MPS_Package *paqueteSyscall = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, sizeof(MPS_Package));
	
	generar_DescriptorID(paqueteSyscall->DescriptorID);
	strcpy(IDpaquete, paqueteSyscall->DescriptorID);
	paqueteSyscall->PayloadDescriptor = '1';
	strcpy(payload, "sys_close(");
	sprintf(FDescriptor,"%d", fileDescriptor);
	strcat(payload, FDescriptor);
	strcat(payload, ")");
	strcpy(paqueteSyscall->Payload,payload);
	paqueteSyscall->PayloadLenght = strlen(payload);
	
	send(socketKSS,(char *)paqueteSyscall, sizeof(MPS_Package)+1,0);
	////listen(socketKSS,100);
	recv(socketKSS, buffer, sizeof(buffer), 0);
	
	paqueteSyscall = (MPS_Package *)buffer;
	if(strcmp(paqueteSyscall->DescriptorID, IDpaquete) == 0){
		if(paqueteSyscall->PayloadDescriptor == '1'){
			return 1;
		}else{
			return 0;
		}
	}else{
		return 0;
	}
}

char *enviarSyscallList(char *pathFTP, int socketKSS){
	char payload[50];
	char IDpaquete[16];
	char buffer[1024];
	char mensaje[1024];
	MPS_Package *paqueteSyscall = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, sizeof(MPS_Package));
	
	generar_DescriptorID(paqueteSyscall->DescriptorID);
	strcpy(IDpaquete, paqueteSyscall->DescriptorID);
	paqueteSyscall->PayloadDescriptor = '1';
	strcpy(payload, "sys_list(");
	strcat(payload, pathFTP);
	strcat(payload, ")");
	strcpy(paqueteSyscall->Payload,payload);
	paqueteSyscall->PayloadLenght = strlen(payload);
	
	send(socketKSS,(char *)paqueteSyscall, sizeof(MPS_Package)+1,0);
	//listen(socketKSS,100);
	recv(socketKSS, buffer, sizeof(buffer), 0);
	
	paqueteSyscall = (MPS_Package *)buffer;
	if(paqueteSyscall->PayloadDescriptor == '1'){
		strcpy(mensaje, pasarListaArchivosARespuestaFTP(paqueteSyscall->Payload));
		return mensaje;
	}else{
		strcpy(mensaje, "");
		return mensaje;
	}
}

char *pasarListaArchivosARespuestaFTP(char *buffer){
	int argumentosPasados = 1,
		contadorBuffer = 0,
		contadorArgumento = 0,
		yaCargueArchivo = 0,
		AuxiliarSize = 0;
	char mensaje[1024],
		 argumento[50],
		 nombreArchivo[50],
		 lineaArchivo[100];

	strcpy(mensaje,"");

	while(buffer[contadorBuffer] != '\0'){
		if(buffer[contadorBuffer] != ','){
			argumento[contadorArgumento] = buffer[contadorBuffer];
			contadorArgumento++;
		}else{
			argumentosPasados++;
		}
		contadorBuffer++;
		if(buffer[contadorBuffer] == '\0'){ //va a salir pero antes necesito cargar la linea
			argumentosPasados++;
		}
		if(argumentosPasados == 3){
			argumentosPasados = 1;
			yaCargueArchivo = 0;
			argumento[contadorArgumento] = '\0';
			contadorArgumento = 0;
			sscanf(argumento,"%d", &AuxiliarSize);
			if(AuxiliarSize){						//es archivo
				strcpy(lineaArchivo, "-rwxrwxrwx 1 ftp ftp ");
			}else{									//es directorio
				strcpy(lineaArchivo, "drwxrwxrwx 1 ftp ftp ");
			}
				strcat(lineaArchivo, argumento);
				strcat(lineaArchivo, " Sep 02 2009 ");
				strcat(lineaArchivo, nombreArchivo);
				strcat(lineaArchivo, "\n");
				
				strcat(mensaje, lineaArchivo);
		}else if ((argumentosPasados == 2) && (!yaCargueArchivo)){
			argumento[contadorArgumento] = '\0';
			contadorArgumento = 0;
			strcpy(nombreArchivo, argumento);
			yaCargueArchivo = 1;
		}
	}
	strcat(mensaje, "\r\n");
	return mensaje;
}
char *enviarSyscallRead(int fileDescriptor, int socketKSS){
	char payload[50],
		 IDpaquete[16],
		 buffer[1024],
		 mensaje[1025],
		 FDescriptor[5];
	MPS_Package *paqueteSyscall = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, sizeof(MPS_Package));
	
	generar_DescriptorID(paqueteSyscall->DescriptorID);
	strcpy(IDpaquete, paqueteSyscall->DescriptorID);
	paqueteSyscall->PayloadDescriptor = '1';
	strcpy(payload, "sys_read(");
	sprintf(FDescriptor,"%d", fileDescriptor);
	strcat(payload, FDescriptor);
	strcat(payload, ")");
	strcpy(paqueteSyscall->Payload,payload);
	paqueteSyscall->PayloadLenght = strlen(payload);
	
	send(socketKSS,(char *)paqueteSyscall, sizeof(MPS_Package)+1,0);
	//listen(socketKSS,100);
	recv(socketKSS, buffer, sizeof(buffer), 0);
	
	paqueteSyscall = (MPS_Package *)buffer;
//	if(strcmp(paqueteSyscall->DescriptorID, IDpaquete) == 0){
		if(paqueteSyscall->PayloadDescriptor != '0'){
			strcpy(mensaje, paqueteSyscall->Payload);
			return mensaje;
		}else{
			strcpy(mensaje, "");
			return mensaje;
		}
/*	}else{
		strcpy(mensaje, "");
		return mensaje;
	}*/
}

int enviarSyscallWrite(int fileDescriptor, int socketKSS, char *bufferParaMandar){
	char payload[50];
	char IDpaquete[16];
	char buffer[100];
	MPS_Package *paqueteSyscall = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, sizeof(MPS_Package));
	
	generar_DescriptorID(paqueteSyscall->DescriptorID);
	strcpy(IDpaquete, paqueteSyscall->DescriptorID);
	paqueteSyscall->PayloadDescriptor = '1';
	strcpy(payload, "sys_write(");
	strcat(payload, fileDescriptor);
	strcat(payload, ",");
	strcat(payload, bufferParaMandar);
	strcat(payload, ")");
	strcpy(paqueteSyscall->Payload,payload);
	paqueteSyscall->PayloadLenght = strlen(payload);
	
	send(socketKSS,(char *)paqueteSyscall, sizeof(MPS_Package)+1,0);
	//listen(socketKSS,100);
	recv(socketKSS, buffer, sizeof(buffer), 0);
	
	paqueteSyscall = (MPS_Package *)buffer;
	if(strcmp(paqueteSyscall->DescriptorID, IDpaquete) == 0){
		if(paqueteSyscall->PayloadDescriptor == '1'){
			return 1;
		}else{
			return 0;
		}
	}else{
		return 0;
	}
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*char *pasarArchivoAMensajeFTP(WIN32_FIND_DATAA fileData){
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
}*/

