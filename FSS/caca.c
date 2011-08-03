#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "funcionesConfig.h"
#include "funcionesLog.h"

#define MAX_BLOQUES 100000
#define SOCK_PATH "echo_socket"


typedef struct{
	char DescriptorID[16];   //Unico, si usamos la hora + un random es suficiente.
	char PayloadDescriptor;  //tipo de operacion (handshake, vda, fss, ftp, list, cd, etc)
	int PayloadLenght;  //longitud del payload, 0 si no enviamos nada.
	char Payload[1050];  //mensaje.
}__attribute__ ((__packed__)) MPS_Package;



int print_pkg (MPS_Package *);
char* generar_DescriptorID(char *);
char* existeArchivo (char *, char *);
void obtenerFuncion(char *, char * );
void infoArchivo (char *, char *, char *);
char* eliminarArchivo (char *, char *);
char* crearArchivo (char *, char *);
char* actualizarTamanio(char *, char *,double);
char* crearTablaSectoresLibres (char *, int );
void dosSectoresLibres (char *, char *);
char* asignarSectores (char *, char *, char *);
char* liberarSectores (char *, char *);
char* formatear (char* , int);
char* tieneFormato (char *);
void listarDirectorio(char *, char *);
void tamanioArchivo (char *, char *, char *);



int main(){

	char func[50],f[50],vda[5],nomArch[31],rta[100000],sectores[100000], infoLog[200];
	int t=0,x=0,z=0,i=0,tam=0,val2=0;
	long val=0;
	char stru[1032];
	int s, len;
	char aux[1032];
	char Buffer[1032];
	int cont=0;
	int cont2;
	struct sockaddr_in remote;

	MPS_Package *mensaje;
	configFSS *configuracion;

	char mensaje_aux[20];
	char stru_aux[1032];

	char comando[10];
	char argumento[100];
	
	mensaje = (MPS_Package*) malloc(sizeof(MPS_Package));
	configuracion = (configFSS*) malloc(sizeof(configFSS));
	
	generar_DescriptorID(mensaje->DescriptorID);
	mensaje->PayloadDescriptor='2';
	mensaje->PayloadLenght=0;
	strcpy(mensaje->Payload, "\0" );
	
	getConfigFSS(configuracion);
	
	sprintf(infoLog, "ipKSS: %s, puertoKSS: %u", configuracion->ipKSS, configuracion->puertoKSS);
	printLog("MAIN","0",0,"INFO",infoLog,configuracion->logActivado);
	

// lo inicializo como para arrancar directo con el Handshake
// NOTA:  para el handshake es CLAVE agregar el \0 para qe pise el mensaje anterior porque sino queda en buffer

    	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        	perror("socket");
    		exit(1);
	}
	
	printf("Trying to connect...\n");

	remote.sin_family = AF_INET;
	remote.sin_addr.s_addr=inet_addr(configuracion->ipKSS);
	remote.sin_port = htons (configuracion->puertoKSS);

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
		exit(1);
	}
	else {
      		printf("Handshake OK\n");
		/*while(strcmp(mensaje->Payload,"halt()")){
		    recv(s,Buffer,sizeof(Buffer),0);
                    print_pkg((MPS_Package*)Buffer);
		    strcpy(mensaje->Payload,"FSS");
			generar_DescriptorID(mensaje->DescriptorID);
			mensaje->PayloadLenght=strlen(mensaje->Payload);
			//send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0);
		}*/
        }

	while(1){
		recv(s,Buffer,sizeof(Buffer),0);
		print_pkg((MPS_Package*)Buffer);

		obtenerFuncion(((MPS_Package*)Buffer)->Payload, func);

		tam=strlen(func)+1;
		for(i=tam;i<1045;i++){
			stru[i-tam]=((MPS_Package*)Buffer)->Payload[i];;
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
	if( !strcmp(func,"infoArchivo")){
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
		infoArchivo(vda,nomArch,rta);
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
				sscanf (aux, "%d", &val);
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
				sscanf (aux, "%d", &val2);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		strcpy(rta,crearTablaSectoresLibres(vda,val2));
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
		dosSectoresLibres(vda,rta);
	}
	if( !strcmp(func,"asignarSectores")){
		while(stru[x-1]!=')'){
			while(stru[x]!=',' && stru[x]!=')' && z<2){
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
		    	while(stru[x]!=')'){
			 	     aux[i]=stru[x];
	                 i++;x++;
			     }
                 aux[i]='\0';
                 sprintf(sectores,"%s",aux);
			}
		aux[i]='\0';
		z++;x++;
		}
		z=0;x=0;t=0;
		strcpy(rta,asignarSectores(vda,nomArch,sectores));
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
			if(z!=0){
				if(t==0){
					strcpy(sectores,aux);
					t=1;
				}else{
					strcat(sectores,",");
					strcat(sectores,aux);
				}
				i=0;
			}
		aux[i]='\0';
		z++;x++;
		}
		z=0;x=0;t=0;
		strcpy(rta,liberarSectores(vda,sectores));
	}
	if( !strcmp(func,"listarDirectorio")){
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
        listarDirectorio(vda,rta);
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
				sscanf (aux, "%d", &val2);
				i=0;
			}
		z++;x++;aux[i]='\0';
		}
		z=0;x=0;
		strcpy(rta,formatear(vda,val2));
	}
	    if( !strcmp(func,"tieneFormato")){
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
                strcpy(rta,tieneFormato(vda));
        }
			printf("%ld\n",strlen(rta));
			
			if(!strcmp(func,"infoArchivo")){
				do{
					cont2=0;
					while(rta[cont]!=',' && rta[cont] != '\0'){
						mensaje->Payload[cont2] = rta[cont];
						cont2++;
						cont++;
					}
					cont++;
					mensaje->Payload[cont2] = '\0';
					mensaje->PayloadLenght = strlen(mensaje->Payload);
					mensaje->PayloadDescriptor='2';
					send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0);
					recv(s, Buffer, sizeof(Buffer), 0);
				}while(((MPS_Package*)Buffer)->PayloadDescriptor == '1');
				printf("Termino el infoArchivo\n");
				cont = 0;
			}else{
				strcpy(mensaje->Payload,rta);
				mensaje->PayloadDescriptor='2';
				mensaje->PayloadLenght=strlen(mensaje->Payload);
				//printf("lo que respondo es: %s \n", mensaje->Payload);
				printf("%ld\n",send(s, (char*)mensaje,21+mensaje->PayloadLenght+1, 0));
			}
			strcpy(rta,"\0");

	}

return 0;
}

int print_pkg(MPS_Package* mensaje){

        printf("Server: DescriptorID = %s\n", mensaje->DescriptorID);
        printf("Server: \"Pay Desc = %c\"\n", mensaje->PayloadDescriptor);
        printf("Server: \"Lenght = %d\"\n",mensaje->PayloadLenght);
        printf("Server: \"Payload = %s\"\n",mensaje->Payload);
	return 0;
}

char* generar_DescriptorID(char *DescriptorID){
	time_t tiempo;
	char tpo[11], num[6];
	long aleatorio;

	strcpy(DescriptorID, "");
	srand(time(NULL));
	time(&tiempo);
	aleatorio = rand() % 90000;
	aleatorio += 10000;
	sprintf(tpo,"%d",tiempo);;
	sprintf(num,"%ld",aleatorio);
	strcat(DescriptorID, tpo);
	strcat(DescriptorID, num);
	return DescriptorID;
}

void obtenerFuncion(char *f, char *func){
	int x=0;


	while(f[x]!= '('){
		func[x]=f[x];
		x++;
	}
	func[x]='\0';
}

void listarDirectorio (char *vda, char *rta){
      DIR *dirp;
      struct dirent *direntp;
      char func[20], mensaje[400], tamanio[20];
      int t=0;
         /* Abrimos el directorio */


		sprintf(func, "./%s", vda);
        dirp = opendir(func);

         if (dirp == NULL){
                  sprintf (rta, "%s", "NO");
                 return;
          }else{

                        /* Leemos las entradas del directorio */

                         printf("i-nodo\toffset\t\tlong\tnombre\n");

                         while ((direntp = readdir(dirp)) != NULL){
 				               printf("%d\t%d\t%d\t%s\n", direntp->d_ino, direntp->d_off, direntp->d_reclen, direntp->d_name);
                               if (strcmp(direntp->d_name,"free_sectors") && strcmp(direntp->d_name,".") && strcmp(direntp->d_name,"..")){

                                     tamanioArchivo(vda, direntp->d_name, tamanio);
					                 sprintf(mensaje,"%s,%s",direntp->d_name, tamanio);



                                      if(t==0){
                                   				strcpy(rta,mensaje);
	                     						t=1;
                                      }else{
		                            			if(strlen(rta)){
	                                    				strcat(rta,",");
	                                      				}
												strcat(rta,mensaje);

                                     }

                               }
						}

				}
         /* Cerramos el directorio */
	printf("Dir:  %s\n",rta);
    closedir(dirp);
	//chdir("..");

}

char* existeArchivo (char *vda, char *nombreArchivo){

	FILE *archivo;
	char dir[100];

	sprintf(dir, "./%s/%s",vda,nombreArchivo);

	archivo = fopen(dir,"r");
	if (archivo==NULL){
		return "NO";
	}

	fclose (archivo);


return "OK";

}

void infoArchivo (char *vda, char *nombreArchivo, char *info){


	FILE *archivo;
	int size;
	char dir[100];


	sprintf(dir, "./%s/%s",vda,nombreArchivo);

	archivo = fopen(dir, "r");

	fseek(archivo, 0, SEEK_END);
	size = ftell(archivo)+1;
	rewind (archivo);

	fgets(info, size, archivo);


	info[size+1] = '\0';

	//printf("Desde infoArchivo: %s\n", info);


	fclose (archivo);
	//chdir("..");
}

char* eliminarArchivo (char *vda, char *nombreArchivo){

	int i=0,j=0;
	char dir[100],info[100000], listaSectores[100000];

	infoArchivo (vda, nombreArchivo, info);

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
	sprintf(dir, "./%s/%s",vda,nombreArchivo);
	remove(dir);
	//chdir("..");

return "OK";

}

char* crearArchivo (char *vda, char *nombreArchivo){

	FILE *archivo;
	char dir[100], info[]="0\0";

	sprintf(dir, "./%s/%s",vda,nombreArchivo);


	archivo = fopen( dir,"w");
	fputs(info, archivo);

	fclose(archivo);
	//chdir("..");

return "OK";
}

char* actualizarTamanio (char* vda, char* nombreArchivo, double actualiza){

	FILE *archivo;
	int i=0, j=0, k=0;
	char info[100000], nuevaInfo[100000], tamanio[40], dir[100];

	infoArchivo ( vda, nombreArchivo, info);
//	printf ("Info Antes: %s\n", info);



	sprintf (tamanio, "%.0f", actualiza);
//	printf ("Tamanio Nuevo: %s\n", tamanio);

	//Longitud Anterior
	while (info[i] != ','){
		i++;
	}
//	printf ("Longitud Tamanio anterior: %d\n", i);

	//Longitud Nueva
	while (tamanio[j] != '\0'){
		j++;
	}
//	printf ("Longitud Tamanio nueva: %d\n", j);

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
	sprintf(dir, "./%s/%s",vda,nombreArchivo);
	remove (dir);
	archivo = fopen( dir,"w");
	fputs (nuevaInfo, archivo);
	fclose (archivo);
	//chdir("..");

return "OK";

}

char* crearTablaSectoresLibres (char *vda, int cantidadSectores){

	FILE *archivo;
	int i=0;
	char array[cantidadSectores], dir[100];



	while (i<cantidadSectores){
		array[i]='0';
		i++;
	}
	array[i]='\0';

	sprintf (dir, "./%s/free_sectors",vda);
	archivo = fopen(dir,"w");
	fputs (array, archivo);

	fclose (archivo);
	

return "OK";

}

void dosSectoresLibres (char *vda, char* sectores){

	FILE *archivo;
	int i=0,j;
	char dir[100],info[100000], nombreArchivo[]="free_sectors";

	infoArchivo (vda, nombreArchivo, info);

	while ( (info[i] == '1') && (info[i] != '\0') ){
		i++;
	}

	if (info[i] == '\0'){
		printf ("NO HAY SUFICIENTES SECTORES LIBRES\n");
		sprintf (sectores, "%s", "NO");
		return;
	}
	printf("Primer Sector Libre: %d\n",i);

	j = i + 1;

	while ( (info[j] == '1') && (info[j] != '\0') ){
		j++;
	}

	if (info[j] == '\0'){
		printf ("NO HAY SUFICIENTES SECTORES LIBRES\n");
		sprintf (sectores, "%s", "NO");
		return;
	}

	printf("Segundo Sector Libre: %d\n",j);

	//Actualizo free_sectors
	info[i]='1';
	info[j]='1';
	sprintf(dir, "./%s/%s", vda, nombreArchivo);
	remove (dir);
	archivo = fopen( dir,"w");
	fputs (info, archivo);
	fclose (archivo);

	sprintf (sectores, "%d,%d", i,j);
	//printf ("Sectores: %s\n", sectores);
	
	//chdir("..");

}

char* asignarSectores (char *vda, char *nombreArchivo, char *sectores){

	FILE *archivo;
//	int i=0,j=0;
	char dir[100], info[100000];

	infoArchivo(vda, nombreArchivo, info);
	printf ("La informacion antes de asignar sectores : %s\n", info);



    strcat(info,",");
    strcat(info,sectores);

    /*
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
	*/
    printf("Los sectores que me mandaron son: %s\n", sectores);

//	printf("info con sectores asignados: %s\n",info);

	sprintf (dir, "./%s/%s", vda,nombreArchivo);
	remove (dir);
	archivo = fopen (dir, "w");
	fputs (info, archivo);
	fclose (archivo);
	
	//chdir("..");

return "OK";
}

char* liberarSectores (char *vda, char *listaSectores){

	FILE *archivo;
	int i=0, c=0,j=0, k, sec;
	char sectoresVda[MAX_BLOQUES], sector[6], dir[100];


	infoArchivo (vda,"free_sectors", sectoresVda);
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

//	printf("la lista es: %s\ncantidad sectores a liberar: %d\n", listaSectores ,c);

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

	sprintf(dir, "./%s/%s",vda,"free_sectors");
	remove (dir);
	archivo = fopen( dir,"w");
	fputs (sectoresVda, archivo);
	fclose (archivo);
	
	//chdir("..");

return "OK";

}

char* tieneFormato (char *vda){

	FILE *archivo;
	char dir[100];

	sprintf(dir, "./%s/free_sectors", vda);

	archivo = fopen (dir, "r");

	if (archivo == NULL){
		return "NO";
	}

	fclose (archivo);
	


return "OK";

}

char* formatear (char* vda, int cantidadSectores){

	char command1[40], command2[40], resp[4];

	sprintf (resp, "%s", tieneFormato(vda) );


	if ((strcmp(resp, "OK") == 0)){

		sprintf(command1, "rm -r %s", vda);
		system (command1);

		sprintf (command2, "mkdir %s", vda);
		system(command2);

		crearTablaSectoresLibres(vda, cantidadSectores);

		printf("Ya estaba formateado, se vuelve a formatear\n");

		
		return "OK";

	}

	sprintf (command2, "mkdir %s", vda);
	system(command2);

	crearTablaSectoresLibres(vda, cantidadSectores);
	
	

return "OK";

}

void tamanioArchivo (char *vda, char *nombre, char *tamanio){

	int i=0;
	char info[100000];


	infoArchivo(vda, nombre, info);


	while (info[i] != ','){

		tamanio[i]=info[i];
		i++;

	}

	tamanio[i]='\0';
	
	//chdir("..");

}
