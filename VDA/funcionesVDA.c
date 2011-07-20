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

int cache[10]={0,0,0,0,0,0,0,0,0,0};


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
int x=0,num=0,i=0;
	while(x!=10){
		if(cache[x]==ptr->dirLogica){
			num=1;
			x=9;
		}
		x++;
	}
	if(num!=1){
		while(cache[i]!=0){
			i++;
		}
		if(cache[i]==0){
			cache[i]=ptr->dirLogica;
			}else{
				x=9;
				while(cache[x]!=0){
					cache[x]=cache[x-1];
					x--;
				}
				cache[0]=ptr->dirLogica;
			}
	}
return(num);
}
int buscarSector(long cabezal, long dirLogica){
	int	num=0;
	if(cabezal==dirLogica){
			printf("%d ",dirLogica);
			num=0;
	}else{
		if(cabezal>dirLogica){
			while(cabezal!=dirLogica){
					printf("%d ",cabezal);
					cabezal--;
					num++;
			}
			printf("%d ",cabezal);
		}else{
			while(cabezal!=dirLogica){
				printf("%d ",cabezal);
				cabezal++;
				num++;
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

Nodo* mostrarLista(Nodo *lista) 
{
Nodo *ptr,*ptr2,*cabezal;
int sumaTiempos=0,dir,opcion,esta,num;
struct chs CHS;char mensajeLog[100],auxLog[50];
char datos[512];
//HANDLE heap = HeapCreate(HEAP_NO_SERIALIZE, 1024*1024, 0);
ptr=lista;
//cabezal=(Nodo *)HeapAlloc(heap,HEAP_NO_SERIALIZE, sizeof(Nodo));
cabezal=(Nodo *)malloc(sizeof(Nodo));
dir=leerCabezal();
cabezal->dirLogica=dir;
CHS=takeCHS(dir);
cabezal->pista=CHS.pista;
cabezal->proximo=NULL; 
cabezal->tiempo=0;

printf("\nPosicion:%d\n",dir);

while (lista!=NULL){
		ptr=lista;
		ptr=algoritmo(ptr,cabezal);
		esta=buscarCache(ptr);

		strcpy(mensajeLog, "Posicion Cabezal: ");
		sprintf(auxLog,"%d",dir);
		strcat(mensajeLog, auxLog);
		printLog("Thread VDA","0",0,"DEBUG",mensajeLog);

		if (esta==1){
					CHS=takeCHS(ptr->dirLogica);
					printf("\nSectores Leidos:%d - cache",CHS.sector);
					
					strcpy(mensajeLog, "Sectores Leidos: ");
					sprintf(auxLog,"%d",CHS.sector);
					strcat(mensajeLog, auxLog);
					printLog("Thread VDA","0",0,"DEBUG",mensajeLog);
					
					printf("\nTiempo consumido:%d ns",1);

					strcpy(mensajeLog, "Tiempo consumido: ");
					strcat(mensajeLog,"1 ns");
					printLog("Thread VDA","0",0,"DEBUG",mensajeLog);	

					strcpy(datos,leer(ptr->dirLogica));
					strcat(datos,"\0");
					printf("\n%s\n",datos);
					printLog("Thread VDA","0",0,"DEBUG","Cache:1 ns");
		}else{
			cabezal->dirLogica=ptr->dirLogica;
			cabezal->pista=ptr->pista;
			dir=leerCabezal();
			CHS=takeCHS(dir);
			num=sectLeidos(ptr->dirLogica);
			sumaTiempos=sumaTiempos+abs(ptr->pista-CHS.pista)+num;
			CHS=takeCHS(ptr->dirLogica);
			
			
			printf("\nTiempo consumido:%d ms",sumaTiempos);
			printf("\nSector %d (%d,%d,%d): %s\n",ptr->dirLogica,CHS.cabezal,CHS.pista,CHS.sector,leer(ptr->dirLogica));
			grabarCabezal(ptr->dirLogica+1);
	
			strcpy(mensajeLog, "Sectores Leidos: ");
			sprintf(auxLog,"%d",ptr->dirLogica);
			strcat(mensajeLog, auxLog);
			printLog("Thread VDA","0",0,"DEBUG",mensajeLog);

			strcpy(mensajeLog, "Tiempo Cosumido: ");
			sprintf(auxLog,"%d",sumaTiempos);
			strcat(mensajeLog, auxLog);
			printLog("Thread VDA","0",0,"DEBUG",mensajeLog);
		
		}
			ptr2=lista;
			while ((ptr2->proximo!=ptr) && (ptr!=lista)){
				ptr2=ptr2->proximo;
			}
			ptr2->proximo=ptr->proximo;
			if(ptr==lista){
				lista=lista->proximo;
				//HeapFree( heap,0,ptr);
				free(ptr);
				ptr=lista;
			}else{
				//HeapFree( heap,0,ptr);
				free(ptr);
				ptr=ptr2;
			}
}
	//HeapFree( heap,0, cabezal );
	free(cabezal);
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

void grabar(long clave,void *dato){
DB *dbp; 
int ret;           
DBT key, data;  
memset(&key, 0, sizeof(DBT)); 
memset(&data, 0, sizeof(DBT)); 
	
	crearBase();
	ret = db_create(&dbp, NULL, 0);    
                       
	ret = dbp->open(dbp, NULL, vda , NULL, DB_BTREE, 0, 0);		
	
	key.data = &clave;
	key.size = sizeof(long);
	data.data = dato;
	data.size = 512; 

	ret = dbp->put(dbp, NULL, &key, &data, 0);
	
	if (ret == DB_KEYEXIST) {
			dbp->err(dbp, ret, "La clave %d ya existe!\n", clave);
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

crearBase();
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
	
	crearBase();
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
	
	return(dirLogica);
	
	dbp->close(dbp, 0); 

}	

//Leo Datos

char* leer(long clave){	
DB *dbp;int ret;           
DBT key, data; char datos[512]; 	
memset(&key, 0, sizeof(DBT)); 
memset(&data, 0, sizeof(DBT)); 
	crearBase();
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
	return(datos);
	dbp->close(dbp, 0); 
}

//Borrar

void borrar(long clave){
DB *dbp; 
int ret;           
DBT key, data; 	
memset(&key, 0, sizeof(DBT)); 
memset(&data, 0, sizeof(DBT)); 
	crearBase();
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
	printf("CHS:(1,%d,%d)\n",CHS.pista,CHS.sector);
}
Nodo* generarNodo(dir1){
Nodo *nodo1;struct chs posDato;
//HANDLE heap = HeapCreate( 0, 0, 0 );
	posDato=takeCHS(dir1);
	//nodo1=(Nodo *)HeapAlloc(GetProcessHeap(),HEAP_NO_SERIALIZE, sizeof(Nodo));
	nodo1=(Nodo *)malloc(sizeof(Nodo));
	nodo1->dirLogica=dir1;
	nodo1->pista=posDato.pista;
	nodo1->tiempo=0; 
	nodo1->proximo=NULL;
return(nodo1);
}
char* getSectores(long dir1,long dir2){

Nodo *nodo,*nodo1=NULL,*nodo2=NULL, *lista=NULL;
struct buffer info;char	mensaje[1024];			
	
	nodo1=generarNodo(dir1);
	nodo1->accion=2;
	nodo2=generarNodo(dir2);
	nodo2->accion=2;
	lista=InsertarNodo(lista,nodo1);
	lista=InsertarNodo(lista,nodo2);
	nodo=nodo1;
	memcpy(info.dato1,leer(dir1),512);
	memcpy(info.dato2,leer(dir2),512);
	while(nodo!=NULL){
		nodo=mostrarLista(lista);	
	}
	memcpy(mensaje,info.dato1,512);
	memcpy(mensaje+512,info.dato2,512);
	return(mensaje);
}

int putSectores(struct infoGrabar *datos){

Nodo *nodo1=NULL,*nodo2=NULL,*lista=NULL,*nodo;

int num=0;
	nodo1=generarNodo(datos->dir1);
	nodo1->dato=datos->dato1;
	nodo2=generarNodo(datos->dir2);
	nodo2->dato=datos->dato2;
	lista=InsertarNodo(lista,nodo1);
	lista=InsertarNodo(lista,nodo2);
	grabar(datos->dir1,datos->dato1);
	grabar(datos->dir2,datos->dato2);
	mostrarLista(lista);	
//	free(nodo1);free(nodo2);
return(num);
}