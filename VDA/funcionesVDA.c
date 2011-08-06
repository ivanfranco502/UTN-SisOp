#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include <stdio.h>
#include <process.h>   
#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include <db.h>
#pragma comment (lib, "libdb48.lib") 
#include "funcionesVDA.h"
#include "funcionesMPSvda.h"
#include "funcionesConfig.h"
#include "funcionesLog.h"



Nodo* InsertarNodo(Nodo *lista,Nodo *nodito)
{
    Nodo *ult=NULL;
	ult=lista;
	if(lista==NULL){ 
		lista=nodito;
		ult=nodito;
		nodito->proximo=NULL;
	}else{
		while(ult->proximo!=NULL){
			ult=ult->proximo;
		}
		ult->proximo=nodito;
		nodito->proximo=NULL;
		ult=nodito;
	}
	return(lista);
}
int buscarCache (Nodo *ptr){
int num=0;
unsigned long i=0,x=0,max=0;
	while(x!=10){
		if(cache[x].dirLog==ptr->dirLogica){
			num=1;
			x=9;
			cache[x].count=0;
		}
		if(cache[x].dirLog!=-1){
			cache[x].count++;
		}
		x++;
	}
	x=0;
	if(num!=1){
		if(cache[9].dirLog==-1){
			while(cache[i].dirLog!=-1){
				i++;
			}
			cache[i].dirLog=ptr->dirLogica;
		}else{
			max=cache[0].count;
			while(x<10){
				if(cache[x].count>max){
					max=cache[x].count;
					i=x;
				}
				x++;
			}
			cache[i].dirLog=ptr->dirLogica;
			cache[i].count=0;
		}
	}
return(num);
}
int buscarSector(long cabezal, long dirLogica){
	int	num=0;
	struct chs dir,cab;
	cab=takeCHS(cabezal);
	dir=takeCHS(dirLogica);
	if(cabezal==dirLogica){
			printf("%d ",dirLogica);
			num=0;
	}else{
			printf("%d ",cabezal);
			while(cabezal!=dirLogica){
					cabezal++;
					num++;
					if(cabezal>((dir.pista+1)*4)-1){
						cabezal=dir.pista*4;
					}
					printf("%d ",cabezal);
			}
		
	}
	return num;
}
int sectLeidos(long dirLogica){
	
	long cabezal;
	struct chs cabCHS,dirCHS; 
	int num=0;

	dirLogica=abs(dirLogica);
	cabezal=leerCabezal();
	cabCHS=takeCHS(cabezal);
	dirCHS=takeCHS(dirLogica);
	printf("\nSectores Leidos:");
	if(cabCHS.pista==dirCHS.pista){
		num=buscarSector(cabezal,dirLogica);
	}else{
		if(cabCHS.pista<dirCHS.pista){
			while(cabCHS.pista!=dirCHS.pista){
					printf("%d ",cabezal);
					cabezal=cabezal+4;
					cabCHS.pista++;
					//cabCHS=takeCHS(cabezal);	
			}
			num=buscarSector(cabezal,dirLogica);
		}else{
			while(cabCHS.pista!=dirCHS.pista){
					printf("%d ",cabezal);
					cabezal=cabezal-4;
					cabCHS.pista--;
					//cabCHS=takeCHS(cabezal);	
			}
			num=buscarSector(cabezal,dirLogica);
		}
	}
	return num;
}

Nodo* mostrarLista(Nodo *lista,HANDLE heap, int logActivada) 
{
Nodo *ptr,*ptr2,*cabezal;
int sumaTiempos=0,dir,opcion,esta,num,contador=0;
struct chs CHS;char mensajeLog[100],auxLog[50];
char datos[512];
configVDA *auxiliar;
//HANDLE heap = HeapCreate(HEAP_NO_SERIALIZE, 1024*1024, 0);
ptr=lista;
cabezal=(Nodo *)HeapAlloc(heap,HEAP_NO_SERIALIZE, sizeof(Nodo));
//cabezal=(Nodo *)malloc(sizeof(Nodo));
dir=leerCabezal();
cabezal->dirLogica=dir;
CHS=takeCHS(dir);
cabezal->pista=CHS.pista;
cabezal->proximo=NULL; 
cabezal->tiempo=0;

auxiliar = HeapAlloc(heap, HEAP_NO_SERIALIZE, sizeof(configVDA));
getConfigVDA(auxiliar);

printf("\nPosicion:%d\n",dir);

while (lista!=NULL){
		ptr=lista;
		ptr=algoritmo(ptr,cabezal);

		strcpy(mensajeLog, "Posicion Cabezal: ");
		sprintf(auxLog,"%d",dir);
		strcat(mensajeLog, auxLog);
		printLog("Thread VDA","0",0,"DEBUG",mensajeLog,logActivada);
		switch (ptr->accion){
		case 1:
			cabezal->dirLogica=ptr->dirLogica;
			cabezal->pista=ptr->pista;
			dir=leerCabezal();
			CHS=takeCHS(dir);
			num=sectLeidos(ptr->dirLogica);
			sumaTiempos=sumaTiempos+abs(ptr->pista-CHS.pista)+num;
			CHS=takeCHS(ptr->dirLogica);
			
			
			printf("\nTiempo consumido:%d ms",sumaTiempos);
			leer(ptr->dirLogica,datos);
			if((cabezal->dirLogica+1)>((ptr->pista+1)*4)-1){
					cabezal->dirLogica=ptr->pista*4;
					grabarCabezal(cabezal->dirLogica);
			}else{
					grabarCabezal(ptr->dirLogica+1);
			}
			printf("\nPosicion Cabezal:%d\n",cabezal->dirLogica);
			strcpy(mensajeLog, "Sector Escrito: ");
			sprintf(auxLog,"%d",ptr->dirLogica);
			strcat(mensajeLog, auxLog);
			printLog("Thread VDA","0",0,"DEBUG",mensajeLog,logActivada);

			strcpy(mensajeLog, "Tiempo Cosumido: ");
			sprintf(auxLog,"%d",sumaTiempos);
			strcat(mensajeLog, auxLog);
			printLog("Thread VDA","0",0,"DEBUG",mensajeLog,logActivada);
			contador++;
		break;
		case 2:
		esta=buscarCache(ptr);
		if (esta==1){
					CHS=takeCHS(ptr->dirLogica);
					printf("\nSectores Leidos:%d - cache",ptr->dirLogica);
					
					strcpy(mensajeLog, "Sectores Leidos: ");
					sprintf(auxLog,"%d",CHS.sector);
					strcat(mensajeLog, auxLog);
					printLog("Thread VDA","0",0,"DEBUG",mensajeLog,logActivada);
					
					printf("\nTiempo consumido:%d ns",1);

					strcpy(mensajeLog, "Tiempo consumido: ");
					strcat(mensajeLog,"1 ns");
					printLog("Thread VDA","0",0,"DEBUG",mensajeLog,logActivada);	

					leer(ptr->dirLogica,datos);
					strcat(datos,"\0");
					printf("\nDatos:%s\n",datos);
					printLog("Thread VDA","0",0,"DEBUG","Cache:1 ns",logActivada);
		}else{
			cabezal->dirLogica=ptr->dirLogica;
			cabezal->pista=ptr->pista;
			dir=leerCabezal();
			CHS=takeCHS(dir);
			num=sectLeidos(ptr->dirLogica);
			sumaTiempos=sumaTiempos+abs(ptr->pista-CHS.pista)+num;
			CHS=takeCHS(ptr->dirLogica);
			
			
			printf("\nTiempo consumido:%d ms",sumaTiempos);
			leer(ptr->dirLogica,datos);
			printf("\nSector %d (%d,%d,%d): %s\n",ptr->dirLogica,CHS.cabezal,CHS.pista,CHS.sector,datos);
			if((cabezal->dirLogica+1)>((ptr->pista+1)*4)-1){
					cabezal->dirLogica=ptr->pista*4;
					grabarCabezal(cabezal->dirLogica);
			}else{
					grabarCabezal(ptr->dirLogica+1);
			}
			strcpy(mensajeLog, "Sectores Leidos: ");
			sprintf(auxLog,"%d",ptr->dirLogica);
			strcat(mensajeLog, auxLog);
			printLog("Thread VDA","0",0,"DEBUG",mensajeLog,logActivada);

			strcpy(mensajeLog, "Tiempo Cosumido: ");
			sprintf(auxLog,"%d",sumaTiempos);
			strcat(mensajeLog, auxLog);
			printLog("Thread VDA","0",0,"DEBUG",mensajeLog,logActivada);
		
			contador++;
		}
		break;
		}
			ptr2=lista;
			while ((ptr2->proximo!=ptr) && (ptr!=lista)){
				ptr2=ptr2->proximo;
			}
			ptr2->proximo=ptr->proximo;
			if(ptr==lista){
				lista=lista->proximo;
				HeapFree( heap,0,ptr);
				//free(ptr);
				ptr=lista;
			}else{
				HeapFree( heap,0,ptr);
				//free(ptr);
				ptr=ptr2;
			}
			
}
	HeapFree( heap,0, cabezal );
	printf("Tiempo medio:%.2f \n",((float)sumaTiempos/(float)contador));
	printf("Peor tiempo:%d \n",sumaTiempos);
	//free(cabezal);
}


Nodo* algoritmo(Nodo *ptr,Nodo* cabezal)
{
	Nodo *buscador1=NULL,*buscador2=NULL;
	long dir;
	buscador1=ptr;
	buscador2=ptr->proximo;
	while(buscador2!=NULL){
		buscador1=ptr;
		if(cabezal->dirLogica==ptr->dirLogica){
			ptr->tiempo=0;	
		}else{
				if((abs(buscador1->pista-cabezal->pista))==(abs(buscador2->pista-cabezal->pista))){
					ptr=buscador1;
					ptr->tiempo=abs(buscador1->pista-cabezal->pista);		
				}else{
					if((abs(buscador1->pista-cabezal->pista))<(abs(buscador2->pista-cabezal->pista))){
							ptr=buscador1;						
							ptr->tiempo=abs(buscador1->pista-cabezal->pista);
						}else{
							if((cabezal!=buscador1) && (cabezal!=buscador2)){
								ptr->tiempo=abs(buscador2->pista-cabezal->pista);
								ptr=buscador2;
							}
						}
				}
		}
	buscador2=buscador2->proximo;
	}
return(ptr);
}
void crearBase(void){
DB *dbp; 
u_int32_t flags; int ret;           
DBT key, data;  
if (!(ret = db_create(&dbp, NULL, 0))){

		flags = DB_CREATE;    
                       
		if (!(ret = dbp->open(dbp, NULL, vda, NULL, DB_BTREE, flags, 0))) {
			if (dbp != NULL){ 
				dbp->close(dbp, 0); 
			}	
		}

	}
}

//Grabar la base

void grabar(long clave,char *dato){
DB *dbp; 
int ret;char datos[512];           
DBT key, data;  
memset(&key, 0, sizeof(DBT)); 
memset(&data, 0, sizeof(DBT)); 
	

	ret = db_create(&dbp, NULL, 0);    
                       
	ret = dbp->open(dbp, NULL, vda , NULL, DB_BTREE, 0, 0);		
	memcpy(datos,dato,512);
	key.data = &clave;
	key.size = sizeof(long);
	data.data = datos;
	data.size = 512; 
	
		ret = dbp->put(dbp, NULL, &key, &data, 0);
		
	if (ret == DB_KEYEXIST) {
			dbp->err(dbp, ret, "La clave %d ya existe!\n", clave);
			strcpy(dato,"\0");
		}
dbp->close(dbp, 0); 
}
//Grabar Cabezal
void grabarCabezal(long dir){
DB *dbp; 
u_int32_t flags; int ret;           
long numero,clave;
DBT key, data; 
memset(&key, 0, sizeof(DBT)); 
memset(&data, 0, sizeof(DBT));


ret = db_create(&dbp, NULL, 0);

ret=dbp->open(dbp, NULL, vda, NULL, DB_BTREE, 0, 0);
clave=-1;
key.data = &clave;
key.size = sizeof(long);
data.data = &dir;
data.size = sizeof(long); 

ret = dbp->put(dbp, NULL, &key, &data, 0);


dbp->close(dbp, 0);
}

long leerCabezal(void){
DB *dbp; 
int ret,clave; long dirLogica;           
DBT key, data; 	struct chs CHS;
memset(&key, 0, sizeof(DBT)); 
memset(&data, 0, sizeof(DBT)); 
	
	
	ret = db_create(&dbp, NULL, 0);    
                       
	ret = dbp->open(dbp, NULL, vda, NULL, DB_BTREE, 0, 0);		
	
	clave=-1;
	key.data = &clave;
	key.size = sizeof(int);
    
	data.data = &dirLogica;
	data.ulen = sizeof(long);
	data.flags = DB_DBT_USERMEM;


	dbp->get(dbp, NULL, &key, &data, 0);
	if(dirLogica<0){
		dirLogica=0;
	}
	
	dbp->close(dbp, 0); 
	return(dirLogica);
}	

//Leo Datos

void leer(long clave, char *datos){	
DB *dbp;int ret;           
DBT key, data; 	
memset(&key, 0, sizeof(DBT)); 
memset(&data, 0, sizeof(DBT)); 
	
	ret = db_create(&dbp, NULL, 0);    
                       
	ret = dbp->open(dbp, NULL, vda, NULL, DB_BTREE, 0, 0);

	key.data = &clave;
	key.size = sizeof(long);
	
	
	dbp->get(dbp, NULL, &key, &data, 0);
	
	if (ret == DB_NOTFOUND) {
		dbp->err(dbp, ret, "El dato no esta en la Base");
	}
	
	if(data.data==NULL){
		strcpy(datos,"\0");
	}else{
		memcpy(datos,data.data,512);
	}
	dbp->close(dbp, 0); 
}

//Borrar

void borrar(long clave){
DB *dbp; 
int ret;           
DBT key, data; 	
memset(&key, 0, sizeof(DBT)); 
memset(&data, 0, sizeof(DBT)); 
	
	ret = db_create(&dbp, NULL, 0);    
                       
	ret = dbp->open(dbp, NULL, vda, NULL, DB_BTREE, 0, 0);
	
	key.data = &clave;
	key.size = sizeof(long);
 
	dbp->del(dbp, NULL, &key, 0); 
	dbp->close(dbp, 0);
}

struct chs	takeCHS(long dirLogica){
	struct chs	CHS;
	CHS.pista=(dirLogica/4);	//4 sectores, 1 cabezal , 100 cilindros
	CHS.sector=(dirLogica%4)%4; 
	CHS.cabezal=1;
	return(CHS);
}

struct chs getCHS(void){
	struct chs CHS;	

	configVDA *auxiliar;

	auxiliar = HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, sizeof(configVDA));
	

	getConfigVDA(auxiliar);

	CHS.pista=auxiliar->cilindros;
	CHS.cabezal=auxiliar->headers;
	CHS.sector=auxiliar->sectores;
	return CHS;
	
}
void posCabezal(void){
	long dir; struct chs CHS;
	dir=leerCabezal();
	CHS=takeCHS(dir);
	printf("CHS:(%d,1,%d)\n",CHS.pista,CHS.sector);
}
Nodo* generarNodo(long dir1,HANDLE heap ){
Nodo *nodo1;struct chs posDato;
//HANDLE heap = HeapCreate( 0, 0, 0 );
	//heap = HeapCreate(HEAP_NO_SERIALIZE, 1024*1024, 0);
	posDato=takeCHS(dir1);
	nodo1=(Nodo *)HeapAlloc(heap,HEAP_NO_SERIALIZE, sizeof(Nodo));
	//nodo1=(Nodo *)malloc(sizeof(Nodo));
	nodo1->dirLogica=dir1;
	nodo1->pista=posDato.pista;
	nodo1->tiempo=0; 
	nodo1->proximo=NULL;
return(nodo1);
}
void getSectores(long dir1,long dir2,char *mensaje,HANDLE heap, int logActivada){

Nodo *nodo,*nodo1=NULL,*nodo2=NULL, *lista=NULL;
struct buffer info;			
	
	nodo1=generarNodo(dir1,heap );
	nodo1->accion=2;
	nodo2=generarNodo(dir2,heap );
	nodo2->accion=2;
	lista=InsertarNodo(lista,nodo1);
	lista=InsertarNodo(lista,nodo2);
	nodo=nodo1;
	leer(dir1,info.dato1);
	leer(dir2,info.dato2);
	
	mostrarLista(lista,heap, logActivada);	
	
	memcpy(mensaje,info.dato1,512);
	memcpy(mensaje+512,info.dato2,512);
}

int putSectores(struct infoGrabar *datos,HANDLE heap, int logActivada){

Nodo *nodo1=NULL,*nodo2=NULL,*lista=NULL,*nodo;

int num=0;
	nodo1=generarNodo(datos->dir1,heap );
	nodo1->dato=datos->dato1;
	nodo1->accion=1;
	nodo2=generarNodo(datos->dir2, heap );
	nodo2->dato=datos->dato2;
	nodo2->accion=1;
	num=sizeof(datos->dato1)+sizeof(datos->dato2);
	lista=InsertarNodo(lista,nodo1);
	lista=InsertarNodo(lista,nodo2);
	grabar(datos->dir1,datos->dato1);
	grabar(datos->dir2,datos->dato2);
	mostrarLista(lista,heap, logActivada);	
	if(!strcmp(datos->dato1,"\0") || !strcmp(datos->dato2,"\0")){
		num=0;
	}
return(num);
}