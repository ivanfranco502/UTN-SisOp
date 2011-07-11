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
	char no[20]="NO", ok[20]="OK",dir[40];

	sprintf(dir, "%s/%s",vda,nombreArchivo);

	archivo = fopen(dir,"r");
	if (archivo==NULL){
		printf("%s",no);
		return *no;
	}

	fclose (archivo);
	printf("%s",ok );

return *ok;

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


	char info[250];
	char dir[40], infoarchivo[500];

	sprintf(infoarchivo, "%s", infoArchivo (vda, nombreArchivo));

	printf("%s \n", info);

	sprintf(dir, "%s/%s",vda,nombreArchivo);

	liberarSectores (vda, infoarchivo);
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

char* actualizarTamanio (char *vda, char *nombreArchivo, long nuevoTamanio){

	FILE *archivo;
	int i=0, j=0, k=0;
	char dir[40],infoarchivo[500],nuevo[500], nuevaInfo[500];

	sprintf (dir, "%s/%s", vda,nombreArchivo);
	printf ("direccion: %s\n", dir);

	//Informacion del Archivo
	sprintf(infoarchivo, "%s", infoArchivo (vda, nombreArchivo));

	remove (dir);

	printf ("Info del Archivo antes:   %s\n",infoarchivo);

	snprintf (nuevo, sizeof(nuevo), "%d", nuevoTamanio);
	printf ("Tamanio Nuevo:    %s\n",nuevo);

	//longitud viejo tamanio
	while (infoarchivo[i] != '/'){
		i++;
	}
	printf("Longitud VIEJO:%d\n",i);

	//longitud nuevo tamanio
	while (nuevo[j] != '\0'){
		j++;
	}
	printf("Longitud NUEVO:%d\n",j);


	while (k<=j){
		nuevaInfo[k]=nuevo[k];
		k++;
	}

//TODO: a partir de aca hay algo q no funca!

	nuevaInfo[k] = '/';



	while (infoarchivo[i] != '\0'){
		nuevaInfo[k]=infoarchivo[i];
		i++;
		k++;
	}

	printf ("NUEVA INFO: %s\n",nuevaInfo);

	archivo = fopen( dir,"w");
	fputs (nuevaInfo, archivo);
	fclose (archivo);

return "OK";

}

char* crearTablaSectoresLibres (char *vda, int cantidadSectores){

	FILE *archivo;
	int i=0;
	char array[cantidadSectores-1], dir[40];



	while (i<cantidadSectores){
		array[i]='0';
		i++;
	}

	sprintf (dir, "%s/free_sectors",vda);
	archivo = fopen(dir,"w+");
	fputs (array, archivo);

	fclose (archivo);

return "OK";

}

char* dosSectoresLibres (char *vda){

	FILE *archivo;
	int i=0,j;
	char dir[40],infoarchivo[500], sectores[20],nombreArchivo[]="free_sectors";

	sprintf (dir, "%s/%s", vda,nombreArchivo);
	archivo = fopen (dir, "r");
	sprintf(infoarchivo, "%s", infoArchivo (vda, nombreArchivo));

	while (infoarchivo[i] != "/"){
		i++;
	}

	if (infoarchivo[i]=="/"){
		i++;
	}

	while (infoarchivo[i] != "0"){
		i++;
	}

	if (infoarchivo == '\0'){
		printf ("NO HAY SUFICIENTES SECTORES LIBRES\n");
	}

	j = i;
	while (infoarchivo[j] != "0"){
		j++;
	}

	if (infoarchivo == '\0'){
		printf ("NO HAY SUFICIENTES SECTORES LIBRES\n");
	}

	snprintf (sectores, sizeof(sectores), "%d,%d", i,j);

	printf ("sectores: %s", sectores);

return sectores;

}

char* asignarSectores (char *vda, char *nombreArchivo, char *sectores){

	FILE *archivo;
	char dir[40];

	sprintf (dir, "%s/%s", vda,nombreArchivo);
	archivo = fopen (dir, "a+");
	fseek(archivo, 0, SEEK_END);

	fputs (",",archivo);
	fputs (sectores,archivo);

	fclose (archivo);

return "OK";
}

char* liberarSectores (char *vda, char *listaSectores){

	int i=0, c=0;
	char sectoresVda[600];


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

//	while (i <= c ){

//	}

return "OK";
}

