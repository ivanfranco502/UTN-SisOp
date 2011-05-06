#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include <stdio.h>
#include <string.h>

#define SOCKET_MAX_BUFFER 100

int enviar_comando (int cliente,char *comando,char *buffer){

        int corrector;

        send(cliente,comando,strlen(comando),0);
        printf("enviado comando %s\n",comando);
        corrector=recv(cliente,buffer,SOCKET_MAX_BUFFER,0);
        buffer[corrector]='\0';
        printf("%s\n",buffer);
        Sleep(1);
        return 0;
}


int enviar_comando_no_recv (int cliente,char *comando,char *buffer){

        int corrector;

        send(cliente,comando,strlen(comando),0);
        printf("enviado comando %s\n",comando);

        Sleep(1);
        return 0;
}


int wait_for (int cliente,char *comando,char *buffer){

        int corrector;

        while(strcmp(buffer,comando)){
                send(cliente,"502 NO PERMITIDO\r\n",strlen("502 NO PERMITIDO\r\n"),0);
                printf("le mande el 500\n");
                corrector=recv(cliente,buffer,SOCKET_MAX_BUFFER,0);
                buffer[corrector]='\0';
                printf("%s\n",buffer);
                Sleep(2);
        }
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

char *obtenerParametrosParaPASV(char * IP, int puerto){
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
char *seleccionadorDeRespuesta (char *comando, char *dirActual, char *tipoIntercambio, char *IP, int puerto) {
	char *CMD,
		 *PRMS,
		 *Response;

	CMD = malloc(sizeof(char)*10);
	PRMS = malloc(sizeof(char)*50);
	Response = malloc(sizeof(char)*100);
	
	strcpy(CMD, obtenerComando(comando));
	strcpy(PRMS, obtenerParametro(comando));

	if(strcmpi(CMD, "USER") == 0){													//USER
		strcpy(Response, "230 User ");
		strcat(Response, PRMS);
		strcat(Response, " log in");
	}else if (strcmpi(CMD, "PWD") == 0){												//PWD
		strcpy(Response, "257 \"");
		strcat(Response, dirActual);
		strcat(Response, "\" is the current directory");
	}else if (strcmpi(CMD, "TYPE") == 0){											//TYPE
		strcpy(Response, "200 Type set to ");
		strcat(Response, PRMS);
		if (strcmpi(PRMS, "A") == 0){
			strcpy(tipoIntercambio, "ASCII");
		}else if (strcmpi(PRMS, "I") == 0){
			strcpy(tipoIntercambio, "BINARY");
		} 
	}else if (strcmpi(CMD, "PASV") == 0){											//PASV
		strcpy(Response, "227 Entering Passive Mode");
		strcat(Response, obtenerParametrosParaPASV(IP, puerto));
	}else if ((strcmpi(CMD, "LIST") == 0)|| (strcmpi(CMD, "LS") == 0)){				//LIST
		strcpy(Response, "150 Opening ");
		strcat(Response, tipoIntercambio);
		strcat(Response, " mode data connection for file list");
	}else if (strcmpi(CMD, "NOOP") == 0){											//NOOP
		strcpy(Response, "200 NOOP command successful");
	}else if ((strcmpi(CMD, "RETR") == 0) || (strcmpi(CMD, "GET") == 0)){				//RETR
		strcpy(Response, "150 Opening ");
		strcat(Response, tipoIntercambio);
		strcat(Response, " mode data connection for ");
		strcat(Response, PRMS);
		strcat(Response, "(63805 bytes)");
	}else if (strcmpi(CMD, "ENDT") == 0){											//ENDT
		strcpy(Response, "226 Transfer complete");
	}else if ((strcmpi(CMD, "STOR") == 0) || (strcmpi(CMD, "PUT") == 0)){				//STOR
		strcpy(Response, "150 Opening ");
		strcat(Response, tipoIntercambio);
		strcat(Response, "mode data connection for ");
		strcat(Response, PRMS);
	}else if ((strcmpi(CMD, "CWD") == 0) || (strcmpi(CMD, "CD") == 0)){				//CWD
		strcpy(Response, "250 CWD command successful");
		strcpy(dirActual, PRMS);
	}else if ((strcmpi(CMD, "DELE") == 0) || (strcmpi(CMD, "DELETE") == 0)){			//DELE
		strcpy(Response, "250 DELE command successful");
	}else if (strcmpi(CMD, "HELP") == 0 || (strcmpi(CMD, "?") == 0)){					//HELP
		strcpy(Response,"Commands are:\nUSER\nPWD\nTYPE\nPASV\nLIST ls\nNOOP\nRETR get\nSTOR put\nCWD cd\nDELE delete\nHELP ?");
	}else{
		strcpy(Response, "500 command not support");
	}
	strcat(Response, "\r\n");

	return(Response);
}
