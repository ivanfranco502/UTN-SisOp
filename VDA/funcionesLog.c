#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include "funcionesLog.h"

int printLog (char *nombreProceso, char *pIDProceso, unsigned threadID, char *tipoLog, char *dato){
	int bytesTransferidos,
		n;
	char log[500],
		 fecha[13],
		 tID[6];
	SYSTEMTIME  st;
	HANDLE out = CreateFileA("ntvc.log", GENERIC_WRITE, 0, NULL, 4, FILE_ATTRIBUTE_NORMAL, NULL);
	GetLocalTime(&st);
	sprintf(fecha,"%d:%d:%d.%d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

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
	strcat(log, "]\r\n\0");
	
	n = 0;
	while (log[n] != '\0'){
		n++;
	}

	SetFilePointer(out,0,0,FILE_END);	
	WriteFile(out, log, n, &bytesTransferidos, NULL);

	CloseHandle(out);
	return 0;
}