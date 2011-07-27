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
#include "funciones.h"

#define SOCK_PATH "echo_socket"
#define MAX_BLOQUES 50000


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
      char func[20], mensaje[400], tamanio[10];
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
}

char* eliminarArchivo (char *vda, char *nombreArchivo){

	int i=0,j=0;
	char dir[100],info[50000], listaSectores[500];

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

return "OK";

}

char* crearArchivo (char *vda, char *nombreArchivo){

	FILE *archivo;
	char dir[100], info[]="0\0";

	sprintf(dir, "./%s/%s",vda,nombreArchivo);


	archivo = fopen( dir,"w");
	fputs(info, archivo);

	fclose(archivo);

return "OK";
}

char* actualizarTamanio (char* vda, char* nombreArchivo, double actualiza){

	FILE *archivo;
	int i=0, j=0, k=0;
	char info[50000], nuevaInfo[50000], tamanio[40], dir[100];

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
	char dir[100],info[50000], nombreArchivo[]="free_sectors";

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

	snprintf (sectores, 10, "%d,%d", i,j);
	//printf ("Sectores: %s\n", sectores);

}

char* asignarSectores (char *vda, char *nombreArchivo, char *sectores){

	FILE *archivo;
	int i=0,j=0;
	char dir[100], info[50000];

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
	char info[50000];


	infoArchivo(vda, nombre, info);


	while (info[i] != ','){

		tamanio[i]=info[i];
		i++;

	}

	tamanio[i]='\0';

}
