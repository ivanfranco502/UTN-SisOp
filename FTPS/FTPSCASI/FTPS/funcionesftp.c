#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include "funcionesftp.h"

#define SOCKET_MAX_BUFFER 5000



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
int rta_USER (char *, char *, reg_cliente *);


unsigned __stdcall threadDeDatos( void* pArguments ){
    // Thread de datos
	
	HANDLE  heapDatos = HeapCreate(HEAP_NO_SERIALIZE, 1024*1024, 0);
	SOCKET descriptorD;
	SOCKET clienteDatos;
	int addrlen = sizeof(struct sockaddr_in);
	int cantidadBytes;
	unsigned puerto = 5300;
	struct sockaddr_in *local_address = HeapAlloc(heapDatos, 0, addrlen);
    struct sockaddr_in *remote_address = HeapAlloc(heapDatos, 0, addrlen);
	reg_cliente *datos_cliente  = HeapAlloc(heapDatos, HEAP_NO_SERIALIZE, sizeof(reg_cliente));
	datos_cliente = (reg_cliente*) pArguments;
	
	printLog("Thread Datos","2",datos_cliente->threadID,"INFO","Conexion al Thread de Datos");
    local_address->sin_family = AF_INET;
	local_address->sin_addr.s_addr=INADDR_ANY;
	
	do{
		local_address->sin_port = htons (puerto);
		descriptorD= socket(AF_INET, SOCK_STREAM, 0);
		puerto++;
	} while((bind (descriptorD,(struct sockaddr *) local_address, addrlen)) == SOCKET_ERROR); // intento bindear algun puerto
	
	printf("bindeo exitoso!!\n");

	datos_cliente->puerto_datos= puerto-1;

	listen(descriptorD,100);

	SetEvent(datos_cliente->evento1);
	printf("me pongo a aceptar el 5300\n");	
	printf("nuevo cliente descriptor: %d\n", clienteDatos = accept(descriptorD, (struct sockaddr *)remote_address, (void*)&addrlen));
	printf("acepte!!\n");
	
	WaitForSingleObject(datos_cliente->evento2, INFINITE); // espero que el thread de comandos me diga que ya puedo mandar o recibir
		
	printf("soy el Thread de datos y voy a analizar..\n");
	switch(datos_cliente->envio_o_recibo){
		case 'E':
			printf("voy a mandar..\n");
			send (clienteDatos, datos_cliente->buffer, strlen(datos_cliente->buffer), 0);	
			printf("mande..\n");
			break;
		case 'R':
			printf("voy a recibir de: %d\n",clienteDatos );
	
//			send(datos_cliente->socket_comando, "150 Opening BINARY mode" , strlen("150 Opening BINARY mode"),0);
			cantidadBytes=recv (clienteDatos, datos_cliente->buffer, SOCKET_MAX_BUFFER, 0);
			datos_cliente->buffer[cantidadBytes]='\0' ;
			printf("llego: %s \n",datos_cliente->buffer);
			//while(cantidadBytes==SOCKET_MAX_BUFFER)
			//	cantidadBytes=recv(clienteDatos, datos_cliente->buffer, SOCKET_MAX_BUFFER, 0);*/
			printf("paso el recv");
			break;
		default:
			;//hacer nada
	}
	printLog("Thread Datos", "2", datos_cliente->threadID, "INFO", "Desconexion al Thread de Datos");		
	// DECIRLE AL DE COMANDOS QUE MANDE EL 226

	closesocket(clienteDatos);
	closesocket(descriptorD);
    _endthreadex( 0 );
} 


int rta_PASV (char *Response, char *arg,reg_cliente *datos_cliente){
	datos_cliente->hThreadDatos = (HANDLE) _beginthreadex(NULL,0, &threadDeDatos, (void*) datos_cliente, 0, NULL);
	WaitForSingleObject(datos_cliente->evento1,INFINITE);
	strcpy(Response, "227 Entering Passive Mode");
	strcat(Response, obtenerParametrosParaPASV("192.168.153.128", datos_cliente->puerto_datos));
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
	printf(" al cliente le mande : %s \n", Response);
	return 0;
}

int rta_NOOP (char *Response, char *arg,reg_cliente *datos_cliente){
	strcpy(Response, "200 NOOP command successful");
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
	return 0;
}

int rta_DELE (char *Response,char *arg,reg_cliente *datos_cliente){
	strcpy(Response, "250 DELE command successful");
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
	return 0;
}

int rta_TYPE (char *Response,char *arg,reg_cliente *datos_cliente){
	strcpy(Response, "200 Type set to ");
	strcat(Response, arg);
	printf("%d\n",strlen(arg));
	if((strncmp(arg,"A",1))==0){
		strcpy(datos_cliente->type,"ASCII");
	}else if((strncmp(arg,"I",1))==0){
		strcpy(datos_cliente->type,"BINARY");
	}
//	strcat(Response, "\r\n"); esta linea hija de puta cagaba todo!! ya venia el arg con \r\n

	printf(" voy a enviar \n" );
	send(datos_cliente->socket_comando, Response , strlen("200 Type set to A\r\n"),0);
	printf(" mande: %s",Response );
	return 0;
}

int rta_LIST (char *Response,char *arg,reg_cliente *datos_cliente){
	datos_cliente->envio_o_recibo='E';
	strcpy(datos_cliente->buffer, "-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo1.txt\r\n-rwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 archivo2.txt\r\ndrwxrwxrwx 1 ftp ftp 4096 Sep 02 2009 directorio1\r\n");
				 
	strcpy(Response, "150 Opening ");
	strcat(Response, datos_cliente->type);
	strcat(Response, " mode data connection for file list");
	strcat(Response, "\r\n");
	printf(" la respuesta que mando es : %s \n" ,Response);
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
	SetEvent(datos_cliente->evento2);     // le aviso al thread de datos que tiene que mandar o recibir
	WaitForSingleObject(datos_cliente->hThreadDatos, INFINITE); // espero que termine el thread de datos
	CloseHandle(datos_cliente->hThreadDatos);
	send(datos_cliente->socket_comando,"226 Transfer Complete\r\n", strlen("226 Transfer Complete\r\n"),0);
	return 0;
}

int rta_CWD (char *Response,char *arg,reg_cliente *datos_cliente){
	strcpy(Response, "250 CWD command successful");
	strcpy(datos_cliente->current_path, arg);
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
	return 0;
}

int rta_PWD (char *Response,char *arg,reg_cliente *datos_cliente){
	strcpy(Response, "257 \"");
	strcat(Response, datos_cliente->current_path); 
	strcat(Response, "\" is the current directory");
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
	return 0;
}

int rta_HELP (char *Response,char *arg,reg_cliente *datos_cliente){
	strcpy(Response,"Commands are:\nUSER\nPWD\nTYPE\nPASV\nLIST\nNOOP\nRETR\nSTOR\nCWD\nDELE\nHELP");
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
	return 0;
}

int rta_RETR (char *Response,char *arg,reg_cliente *datos_cliente){
	datos_cliente->envio_o_recibo='E';
	strcpy(datos_cliente->buffer,"caca caca");
	strcpy(Response, "150 Opening ");
	strcat(Response, datos_cliente->type);
	strcat(Response, " mode data connection for ");
	strcat(Response, arg);
//	strcat(Response, "(63805 bytes)");  comente por filezilla,, no se que onda agregara un \n al arg?
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
	printf(Response);
	SetEvent(datos_cliente->evento2);     // le aviso al thread de datos que tiene que mandar o recibir
	WaitForSingleObject(datos_cliente->hThreadDatos, INFINITE); // espero que termine el thread de datos
	CloseHandle(datos_cliente->hThreadDatos);
	send(datos_cliente->socket_comando,"226 Transfer Complete\r\n", strlen("226 Transfer Complete\r\n"),0);
	return 0;
}

int rta_STOR (char *Response,char *arg	,reg_cliente *datos_cliente){
	datos_cliente->envio_o_recibo='R';
//	SetEvent(datos_cliente->evento2);     // le aviso al thread de datos que tiene que mandar o recibir
	strcpy(Response, "150 Opening ");
	strcat(Response, datos_cliente->type);
	strcat(Response, "mode data connection for ");
	strcat(Response, arg);
//	strcat(Response, "\r\n");	otra linea de mierda que caga todo?!?!?! SII era esta tambien
//	send(datos_cliente->socket_comando, Response, strlen(Response),0); 
	SetEvent(datos_cliente->evento2);     // le aviso al thread de datos que tiene que mandar o recibir
	 
	printf(" mandame cliente de mierda!! \n" );
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
	WaitForSingleObject(datos_cliente->hThreadDatos, INFINITE); // espero que termine el thread de datos
	printf("volvi del thread datos stor");
	CloseHandle(datos_cliente->hThreadDatos);
	send(datos_cliente->socket_comando,"226 Transfer Complete\r\n", strlen("226 Transfer Complete\r\n"),0);
	return 0;
}

int rta_USER (char *Response, char *arg,reg_cliente *datos_cliente){
	strcpy(Response, "230 Usuario anonimo logueado");
	strcat(Response, "\r\n");
	send(datos_cliente->socket_comando, Response, strlen(Response),0);
	return 0;
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
		if ((!pasoComando) && (comando[n] != '\r') && (comando[n] != '\n')) {
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
  if ((comando[n] == ' ') || (comando[n] == '\r')){
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

char *obtenerParametrosParaPASV(char *IP, unsigned puerto){
	char parametrosPASV[26],
		 ipModificada[18],
		 puertoAux[4];
	unsigned puertoDiv,
			 puertoRes;
	int indice;

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
	puertoDiv = puerto/256;
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
	char Response[100]="";

	while (strcmp(vector_comandos[i].mensaje,comando)&&(i<11)){
		printf("%d\n",i);
		i++;
	}

	if (i==11){
		strcpy(Response, "500 command not support\r\n");
		send(datos_cliente->socket_comando, Response, strlen(Response),0);
	}else
		(*(vector_comandos[i].pfunc)) (Response, argumento, datos_cliente);
	

}
int printLog (char *nombreProceso, char *pIDProceso, unsigned threadID, char *tipoLog, char *dato){
	int bytesTransferidos,
		n;
	char log[100],
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

void paraElMain(t_command_handler * vector_comandos){
	vector_comandos[0].pfunc=&rta_PASV;
	strcpy(vector_comandos[0].mensaje,"PASV");
	vector_comandos[1].pfunc=&rta_NOOP;
	strcpy(vector_comandos[1].mensaje,"noop");
	vector_comandos[2].pfunc=&rta_DELE;
	strcpy(vector_comandos[2].mensaje,"DELE");
	vector_comandos[3].pfunc=&rta_TYPE;
	strcpy(vector_comandos[3].mensaje,"TYPE");
	vector_comandos[4].pfunc=&rta_LIST;
	strcpy(vector_comandos[4].mensaje,"LIST");
	vector_comandos[5].pfunc=&rta_CWD;
	strcpy(vector_comandos[5].mensaje,"CWD");
	vector_comandos[6].pfunc=&rta_PWD;
	strcpy(vector_comandos[6].mensaje,"PWD");
	vector_comandos[7].pfunc=&rta_HELP;
	strcpy(vector_comandos[7].mensaje,"HELP");
	vector_comandos[8].pfunc=&rta_RETR;
	strcpy(vector_comandos[8].mensaje,"RETR");
	vector_comandos[9].pfunc=&rta_STOR;
	strcpy(vector_comandos[9].mensaje,"STOR");
	vector_comandos[10].pfunc=&rta_USER;
	strcpy(vector_comandos[10].mensaje,"USER");
}