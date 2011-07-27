#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "funcionesLog.h"

int printLog (char *nombreProceso, char *pIDProceso, unsigned threadID, char *tipoLog, char *dato){
	int bytesTransferidos,
		n;
	char log[500],
		 fecha[13],
		 tID[6];
	FILE *archivo;
	time_t  st;
	struct tm *tmPtr;

	archivo = fopen("ntvc.log","a");
	st = time(NULL);
	tmPtr = localtime(&st);
	
	strftime(fecha, 13, "%H:%M:%S", tmPtr);

	sprintf(tID, "%d", threadID);

	bytesTransferidos = 0;

	strcpy(log, "[");
	strcat(log, fecha);
	strcat(log, "][");
	strcat(log, nombreProceso);
	strcat(log, "][");
	strcat(log, pIDProceso);
	strcat(log, "][");
	strcat(log, tID);
	strcat(log, "][");
	strcat(log, tipoLog);
	strcat(log, "][");
	strcat(log, dato);
	strcat(log, "]\n\0");
	
	fwrite(log,1, sizeof(log), archivo);

	fclose(archivo);
	return 0;
}
