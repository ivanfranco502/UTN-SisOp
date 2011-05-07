#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include <stdio.h>
#include <string.h>

#define SOCKET_MAX_BUFFER 100

//funciones de respuesta!! hay que agregar el codigo de ivan
//prototipos, podrian ir en el .h dps vemoooos

int rta_PASV (char *, char *, reg_cliente *);
int rta_NOOP (char *, char *, reg_cliente *);
int rta_DELE (char *, char *, reg_cliente *);
int rta_TYPE (char *, char *, reg_cliente *);
int rta_LIST (char *, char *, reg_cliente *);
int rta_CWD  (char *, char *, reg_cliente *);
int rta_PWD  (char *, char *, reg_cliente *);
int rta_HELP (char *, char *, reg_cliente *);
int rta_RETR (char *, char *, reg_cliente *);
int rta_STOR (char *, char *, reg_cliente *);


int rta_PASV (char *Response, char *arg,reg_cliente *datos_cliente){
	strcpy(Response, "227 Entering Passive Mode");
	strcat(Response, obtenerParametrosParaPASV(IP, datos_cliente->puerto_datos));
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
}

int rta_NOOP (char *Response, char *arg,reg_cliente *datos_cliente){
	strcpy(Response, "200 NOOP command successful");
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
}

int rta_DELE (char *Response,char *arg,reg_cliente *datos_cliente){
	strcpy(Response, "250 DELE command successful");
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
}

int rta_TYPE (char *Response,char *arg,reg_cliente *datos_cliente){
	strcpy(Response, "200 Type set to ");
	strcat(Response, arg);
	if (strcmpi(arg, "A") == 0){
		strcpy(datos_cliente->type, "ASCII");
	}else if (strcmpi(arg, "I") == 0){
		strcpy(datos_cliente->type, "BINARY");
	}
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
}

int rta_LIST (char *Response,char *arg,reg_cliente *datos_cliente){
	strcpy(Response, "150 Opening ");
	strcat(Response, datos_cliente->type);
	strcat(Response, " mode data connection for file list");
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
}

int rta_CWD (char *Response,char *arg,reg_cliente *datos_cliente){
	strcpy(Response, "250 CWD command successful");
	strcpy(datos_cliente->current_path, arg);
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
}

int rta_PWD (char *Response,char *arg,reg_cliente *datos_cliente){
	strcpy(Response, "257 \"");
	strcat(Response, datos_cliente->current_path);
	strcat(Response, "\" is the current directory");
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
}

int rta_HELP (char *Response,char *arg,reg_cliente *datos_cliente){
	strcpy(Response,"Commands are:\nUSER\nPWD\nTYPE\nPASV\nLIST\nNOOP\nRETR\nSTOR\nCWD\nDELE\nHELP");
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
}

int rta_RETR (char *Response,char *arg,reg_cliente *datos_cliente){
	strcpy(Response, "150 Opening ");
	strcat(Response, datos_cliente->type);
	strcat(Response, " mode data connection for ");
	strcat(Response, arg);
	strcat(Response, "(63805 bytes)");
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
}

int rta_STOR (char *Response,char *arg	,reg_cliente *datos_cliente){
	strcpy(Response, "150 Opening ");
	strcat(Response, datos_cliente->type);
	strcat(Response, "mode data connection for ");
	strcat(Response, arg);
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
}

char *obtenerComando(char *comando){
	int n, 
		pasoComando,
		aux;
	char comandoObtenido[10];

	n = 0;
	pasoComando = 0;
	aux = 0;

	while (comando[n] != '\0' && !pasoComando){
		if (comando[n] == ' '){
			pasoComando = 1;
		}
		if (!pasoComando) {
			comandoObtenido[aux]= comando[n];
			aux++;
		}
		n++;
	}
	comandoObtenido[aux] = '\0';
	return (comandoObtenido);
}

char *obtenerParametro(char *comando){
	int n, 
		pasoComando,
		aux;
	char parametro[30];
	
	n = 0;
	pasoComando = 0;
	aux = 0;
	
	while (comando[n] != '\0'){
		if (comando[n] == ' '){
			pasoComando = 1;
		}
		if (pasoComando) {
			if(comando[n] != ' '){
				parametro[aux]= comando[n];
				aux++;
			}
		}
		n++;
	}
	parametro[aux] = '\0';

	return (parametro);
}

char *obtenerParametrosParaPASV(char *IP, int puerto){
	char parametrosPASV[26],
		 ipModificada[15],
		 puertoAux[4];
	int puertoDiv,
		puertoRes,
		indice;

	//transforma el numero de IP XXX.XXX.XXX.XXX en XXX,XXX,XXX,XXX
	strcpy(parametrosPASV,"(");
	indice = 0;
	while (IP[indice] != '\0'){
		if (IP[indice] == '.'){
			ipModificada[indice] = ',';
		}else{
			ipModificada[indice] = IP[indice];
		}
		indice++;
	}
	ipModificada[indice] = '\0';
	strcat(parametrosPASV, ipModificada);
	strcat(parametrosPASV, ",");

	//transforrma el puerto XXXXX en YYY*256+ZZZ= XXXXX
	puertoDiv = abs(puerto/256);
	puertoRes = puerto - (puertoDiv*256);

	sprintf(puertoAux, "%d", puertoDiv);
	strcat(parametrosPASV, puertoAux);
	strcat(parametrosPASV, ",");
	sprintf(puertoAux, "%d", puertoRes);
	strcat(parametrosPASV, puertoAux);
	strcat(parametrosPASV, ").");

	return (parametrosPASV);
}

//handler de comandos  recibe el vector de registros (funciones y comandos) el puntero al registro del cliente, el comando y el argumento

int command_handler(t_command_handler *vector_comandos,char *comando, char *argumento,reg_cliente* datos_cliente){

	int i=0;
	char Response[100];

       	while (strcmp(vector_comandos[i].mensaje,comando)&&(i<10)){
		i++;
	}

	if (i==10){
		strcpy(Response, "500 command not support\r\n");
	}
	else{
		(*(vector_comandos[i].pfunc)) (Response, argumento, datos_cliente);
	}

}
int printLog (char *nombreProceso, char *pIDProceso, unsigned threadID, char *tipoLog, char *dato){
	int bytesTransferidos,
		n;
	char log[100];
	char fecha[13];
	char tID[6];
	int i =0;
	SYSTEMTIME  st;
	HANDLE out = CreateFileA("ntvc.log", GENERIC_WRITE, 0, NULL, 4, FILE_ATTRIBUTE_NORMAL, NULL);
	GetLocalTime(&st);
	sprintf(fecha,"%d:%d:%d.%d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	
	do{
		tID[i++]= threadID % 10 + '0';
	}while((threadID/=10)>0);
	tID[i] = '\0';
	reverse(tID);

	bytesTransferidos = 0;

	strcpy(log, "[");
	strcat(log, fecha);
	strcat(log, "][");
	strcat(log, nombreProceso);
	strcat(log, "][");
	strcat(log, pIDProceso);
	strcat(log, "][");
	strcat(log, threadID);
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

