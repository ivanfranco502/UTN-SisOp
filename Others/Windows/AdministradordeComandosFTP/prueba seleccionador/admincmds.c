#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "funcionesftp.h"

int main(void){
	int fin;
	char comando[100];
	char dirActual[50] = "/";
	char tipoIntercambio[8] = "";
	char IP[] = "192.168.1.108";
	int  puerto = 5000;

	fin = 0;
	while (!fin){
		gets(comando);
		printf("%s",seleccionadorDeRespuesta(comando, dirActual, tipoIntercambio, IP, puerto));
		if (strcmp(seleccionadorDeRespuesta(comando, dirActual, tipoIntercambio, IP, puerto),"226 Transfer complete\r\n") == 0){
			fin = 1;
		}
	}

	return 0;
}
