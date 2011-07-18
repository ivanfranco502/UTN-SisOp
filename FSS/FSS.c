#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SOCK_PATH "echo_socket"
#define MAX_BLOQUES 400




typedef struct{
	char DescriptorID[16];   //único, si usamos la hora + un random es suficiente.
	char PayloadDescriptor;  //tipo de operacion (handshake, vda, fss, ftp, list, cd, etc)
	int PayloadLenght;  //longitud del payload, 0 si no enviamos nada.
	char Payload[1050];  //mensaje.
}__attribute__ ((__packed__)) MPS_Package;

int parseo_parentesis (char*,char*,char *);
void generar_DescriptorID(char *);
int print_pkg (MPS_Package*);
char* obtenerFuncion(char);
char* existeArchivo (char *vda, char *nombreArchivo);
char* infoArchivo (char *vda, char *nombreArchivo);
char* eliminarArchivo (char *vda, char *nombreArchivo);
char* crearArchivo (char *vda, char *nombreArchivo);
char* actualizarTamanio (char *vda, char *nombreArchivo, long nuevoTamanio);
char* crearTablaSectoresLibres (char *vda, int cantidadSectores);
char* dosSectoresLibres (char *vda);
char* asignarSectores (char *vda, char *nombreArchivo, char *sectores);
char* liberarSectores (char *vda, char *listaSectores);

void main(){

	char func[50],f[50],vda[4],nomArch[20],rta[50];
	int x=0,z=0,i=0,tam=0,sec1,sec2;
	long val=0;
	char aux[20];
	char stru[1032];
	int s, t, len;
	struct sockaddr_in remote;
	char Buffer[1200];
	MPS_Package *mensaje;
	char comando[10];
	char argumento[100];
	
	mensaje = (MPS_Package*) malloc(sizeof(MPS_Package));
	generar_DescriptorID(mensaje->DescriptorID);
	mensaje->PayloadDescriptor='2';
	mensaje->PayloadLenght=0;
	strcpy(mensaje->Payload, "\0" );

// lo inicializo como para arrancar directo con el Handshake 
// NOTA:  para el handshake es CLAVE agregar el \0 para qe pise el mensaje anterior porque sino queda en buffer

    	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        	perror("socket");
    		exit(1);
	}

	printf("Trying to connect...\n");

	remote.sin_family = AF_INET;
	remote.sin_addr.s_addr=INADDR_ANY;
	remote.sin_port = htons (5300);

	len = sizeof(struct sockaddr);
	if (connect(s, (struct sockaddr *)&remote, len) == -1) {
        	perror("connect");
        	exit(1);
	}

	printf("Connected.\n");


	printf("Iniciando Handshake..\n");

     send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0);
	recv(s,Buffer,sizeof(Buffer),0);
	print_pkg((MPS_Package*)Buffer);


	if (((MPS_Package*)Buffer)->PayloadDescriptor=='0'){
	printf("Handshake Failed \n");
	}
	else {
		printf("Handshake OK\n");
		while(strcmp(mensaje->Payload,"halt()")){
		    recv(s,Buffer,sizeof(Buffer),0);
			print_pkg((MPS_Package*)Buffer);
			strcpy(mensaje->Payload,"FSS");
			generar_DescriptorID(mensaje->DescriptorID);
			mensaje->PayloadLenght=strlen(mensaje->Payload);
			send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0);
		}
	while(1){
	recv(s,Buffer,sizeof(Buffer),0);
	strcpy(f,Buffer);
	strcpy(func,obtenerFuncion(f));
	
	tam=strlen(func)+1;
	for(i=tam;i<1045;i++){
		stru[i-tam]=f[i];
	}
	i=0;
	if( !strcmp(func,"existeArchivo")){
		
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
			if(z==1){
				strcpy(nomArch,aux);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		strcpy(rta,existeArchivo(vda,nomArch));

	}
	if( strcmp(func,"infoArchivo")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
			if(z==1){
				strcpy(nomArch,aux);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		strcpy(rta,infoArchivo(vda,nomArch));
	}
	if( !strcmp(func,"eliminarArchivo")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
			if(z==1){
				strcpy(nomArch,aux);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		strcpy(rta,eliminarArchivo(vda,nomArch));
	}	
	if( !strcmp(func,"crearArchivo")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
			if(z==1){
				strcpy(nomArch,aux);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		strcpy(rta,crearArchivo(vda,nomArch));
	}		
	if( !strcmp(func,"actualizarTamanio")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
			if(z==1){
				strcpy(nomArch,aux);
				i=0;
			}
			if(z==2){
				val=atoi(aux);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		strcpy(rta,actualizarTamanio(vda,nomArch,val));
	}
	if( !strcmp(func,"crearTablaSectoresLibres")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
			if(z==1){
				val=atoi(aux);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		strcpy(rta,crearTablaSectoresLibres(vda,val));
	}
	if( !strcmp(func,"dosSectoresLibres")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		strcpy(rta,dosSectoresLibres(vda));
	}
	if( !strcmp(func,"asignarSectores")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
			if(z==1){
				sec1=atoi(aux);
				i=0;
			}	
			if(z==2){
				sec2=atoi(aux);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;	
		strcpy(rta,asignarSectores(vda,nomArch,sec1,sec2));
	}
	if( !strcmp(func,"liberarSectores")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
			if(z==1){
				/*
				Agregar lista de sectores
				*/
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		//strcpy(rta,liberarSectores(vda,lsSectores));
	}
	if( !strcmp(func,"liberarDirectorio")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		strcpy(rta,liberarDirectorio(vda));
	}
	if( !strcmp(func,"formatear")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')'){
				aux[i]=stru[x];	
				i++;x++;
			}
			aux[i]='\0';
			if(z==0){
				strcpy(vda,aux);
				i=0;
			}
			if(z==1){
				val=atoi(aux);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		strcpy(rta,liberarDirectorio(vda,val));
	}
		strcpy(mensaje->Payload,rta);
		mensaje->PayloadLenght=strlen(mensaje->Payload);
		send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0);
}
	
}

char* obtenerFuncion(char f[50]){
	int x=0;
	char func[20];
	

	while(f[x]!= '('){
		func[x]=f[x];
		x++;
	}
	func[x]='\0';
return func;
}

char* existeArchivo (char *vda, char *nombreArchivo){

	FILE *archivo;
	char dir[60];

	sprintf(dir, "%s/%s",vda,nombreArchivo);

	archivo = fopen(dir,"r");
	if (archivo==NULL){
		return "NO";
	}

	fclose (archivo);

return "OK";

}

char* infoArchivo (char *vda, char *nombreArchivo){


	FILE *archivo;
	int size;
	char infoArchivo[500],dir[40];


	sprintf(dir, "%s/%s",vda,nombreArchivo);

	archivo = fopen(dir, "r");

	fseek(archivo, 0, SEEK_END);
	size = ftell(archivo);
	rewind (archivo);

	fgets(infoArchivo, size, archivo);


	infoArchivo[size+1] = '\0';

	printf("%s", infoArchivo);


	fclose (archivo);


return infoArchivo;

}

char* eliminarArchivo (char *vda, char *nombreArchivo){

	int i=0,j=0;
	char dir[40],info[500], listaSectores[500];

	sprintf(info, "%s", infoArchivo (vda, nombreArchivo));

	while (info[i] != ','){
		i++;
	}

	i++;

	while (info[i] != '\0'){
		listaSectores[j]=info[i];
		i++;
		j++;
	}

	listaSectores[j]='\0';

	liberarSectores (vda, listaSectores);
	sprintf(dir, "%s/%s",vda,nombreArchivo);
	remove(dir);

return "OK";

}

char* crearArchivo (char *vda, char *nombreArchivo){

	FILE *archivo;
	char dir[40];

	sprintf(dir, "%s/%s",vda,nombreArchivo);

	archivo = fopen( dir,"w");

	fclose(archivo);

return "OK";
}

char* actualizarTamanio (char* vda, char* nombreArchivo, long actualiza){

	FILE *archivo;
	int i=0, j=0, k=0;
	char info[500], nuevaInfo[500], tamanio[40], dir[100];

	sprintf (info, "%s", infoArchivo ( vda, nombreArchivo ) );
	printf ("Info Antes: %s\n", info);

	snprintf (tamanio, 20, "%d", actualiza);
	printf ("Tamanio Nuevo: %s\n", tamanio);

	//Longitud Anterior
	while (info[i] != ','){
		i++;
	}
	printf ("Longitud Tamanio anterior: %d\n", i);

	//Longitud Nueva
	while (tamanio[j] != '\0'){
		j++;
	}
	printf ("Longitud Tamanio nueva: %d\n", j);

	//Lleno Nueva Info
	while (k<j){
		nuevaInfo[k]=tamanio[k];
		k++;
	}

	while (info[i] != '\0'){
		nuevaInfo[k]=info[i];
		i++;
		k++;
	}
	nuevaInfo[k] = '\0';

	printf("Nueva Info: %s\n",nuevaInfo);

	//Agrego Nueva Info Al Archivo
	sprintf(dir, "%s/%s",vda,nombreArchivo);
	remove (dir);
	archivo = fopen( dir,"w");
	fputs (nuevaInfo, archivo);
	fclose (archivo);

return "OK";

}

char* crearTablaSectoresLibres (char *vda, int cantidadSectores){

	FILE *archivo;
	int i=0;
	char array[cantidadSectores], dir[40];



	while (i<cantidadSectores){
		array[i]='0';
		i++;
	}
	array[i]='\0';

	sprintf (dir, "%s/free_sectors",vda);
	archivo = fopen(dir,"w");
	fputs (array, archivo);

	fclose (archivo);

return "OK";

}

char* dosSectoresLibres (char *vda){

	FILE *archivo;
	int i=0,j;
	char dir[40],info[500], sectores[10],nombreArchivo[]="free_sectors";

	sprintf(info, "%s", infoArchivo (vda, nombreArchivo));

	while ( (info[i] == '1') && (info[i] != '\0') ){
		i++;
	}

	if (info[i] == '\0'){
		printf ("NO HAY SUFICIENTES SECTORES LIBRES\n");
		return "NO";
	}
	printf("Primer Sector Libre: %d\n",i);

	j = i + 1;

	while ( (info[j] == '1') && (info[j] != '\0') ){
		j++;
	}

	if (info[j] == '\0'){
		printf ("NO HAY SUFICIENTES SECTORES LIBRES\n");
		return "NO";
	}

	printf("Segundo Sector Libre: %d\n",j);

	//Actualizo free_sectors
	info[i]='1';
	info[j]='1';
	sprintf(dir, "%s/%s", vda, nombreArchivo);
	remove (dir);
	archivo = fopen( dir,"w");
	fputs (info, archivo);
	fclose (archivo);

	snprintf (sectores, 10, "%d,%d", i,j);
	printf ("Sectores: %s\n", sectores);

return sectores;
}

char* asignarSectores (char *vda, char *nombreArchivo, char *sectores){

	FILE *archivo;
	int i=0,j=0;
	char dir[40], info[500];

	sprintf(info, "%s",infoArchivo(vda, nombreArchivo));

	while (info[i] != '\0'){
		i++;
	}

	info[i] = ',';
	i++;



	while (sectores[j] != '\0'){
		info[i]=sectores[j];
		i++;
		j++;
	}
	info[i]='\0';

//	printf("info con sectores asignados: %s\n",info);


	sprintf (dir, "%s/%s", vda,nombreArchivo);
	remove (dir);
	archivo = fopen (dir, "w");
	fputs (info, archivo);
	fclose (archivo);

return "OK";
}

char* liberarSectores (char *vda, char *listaSectores){

	FILE *archivo;
	int i=0, c=0,j=0, k, sec;
	char sectoresVda[MAX_BLOQUES], sector[6], dir[50];


	sprintf(sectoresVda, infoArchivo (vda,"free_sectors"));
	printf("sectores del vda: %s\n", sectoresVda);


	//cantidad sectores a liberar
	while (listaSectores[i] != '\0'){
		if ( listaSectores[i] == ',' ) {
			c++;
		}
		i++;
	}
	c = c + 1;
	i = 0;

	printf("la lista es: %s\ncantidad sectores a liberar: %d\n", listaSectores ,c);

	while (j < c ){

		k=0;

		while ((listaSectores[i] != ',') && (listaSectores[i] != '\0') ){
			sector[k] = listaSectores[i];
			i++;
			k++;
		}

		sscanf (sector, "%d", &sec);
		printf("sector: %d\n",sec);
		sectoresVda[sec] = '0';
		i++;
		j++;

	}

	printf("Sectores Cambiados: %s\n", sectoresVda);

	sprintf(dir, "%s/%s",vda,"free_sectors");
	remove (dir);
	archivo = fopen( dir,"w");
	fputs (sectoresVda, archivo);
	fclose (archivo);

return "OK";

}

char* formatear (char* vda, int cantidadSectores){

	char command1[40], command2[40];

	sprintf(command1, "rm -r %s", vda);
	system (command1);
	sprintf (command2, "mkdir %s", vda);
	system(command2);

	crearTablaSectoresLibres(vda, cantidadSectores);

return "OK";

}
