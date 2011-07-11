#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"



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

