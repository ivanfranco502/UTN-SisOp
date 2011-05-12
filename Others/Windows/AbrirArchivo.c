#include <windows.h>
#include <stdio.h>
#include <stdlib.h>


char *dameIP (char *buff) {
	
	char IP[16];
	int i=0;
	

	while (buff[i] != ':'){
		IP[i]=buff[i];
		i++;
	}

	IP[i]='\0';

	Sleep (2000);
	
	return IP;
}


char *damePuerto(char *buff){

	char PORT[6];
	int i=0;
	int j=0;

	while (buff[i] != ':'){
		i++;
	}

	while (buff[i] != '\0'){
		if(buff[i] != ':'){
			PORT[j]=buff[i];
			j++;
		}
		i++;
	}

	PORT[j]='\0';

	return PORT;
}



int main () {
	
	//inicialización variables
	HANDLE *archivo;
	DWORD bytesLeidos;
	LPCSTR nombreArchivo = "ftp.config";
	char buffer[100];
	int lectura;
	char IP[15+1];
	char PORT[5+1];
	

	//archivo = (HANDLE*)malloc (sizeof("ftp.config"));

	//apertura archivo
	archivo = CreateFileA ( nombreArchivo, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	printf ("la apertura tiro:%d,%s\n",archivo);

	if (archivo == INVALID_HANDLE_VALUE){
		printf("No se pudo abrir el archivo de configuración");
		return;
	}

	printf ("buffer antes: %s\n",buffer);
	
	//lectura archivo
	lectura = ReadFile (archivo, buffer, sizeof(buffer), &bytesLeidos, NULL);

	printf ("la lectura dio: %d\n",lectura);
	printf ("leyo %d bytes\n", bytesLeidos);

	buffer[bytesLeidos] = '\0';
	printf ("buffer despues: %s\n",buffer);

	if (lectura == 0) {
		printf("Hubo error en la lectura del archivo");
		return;
	}

	CloseHandle(archivo);

	strcpy(IP, dameIP (buffer));
	strcpy(PORT, damePuerto (buffer));

	printf ("el IP es:%s y el puerto es: %s\n",IP,PORT);
	
	Sleep(5000);

	return 0;

}
