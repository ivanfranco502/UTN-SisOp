#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"

#define MAX_BLOQUES 400


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
