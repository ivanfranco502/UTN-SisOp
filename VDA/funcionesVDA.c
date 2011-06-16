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

int cache[10]={0,0,0,0,0,0,0,0,0,0};
struct buffer
{
char *dato1;
char *dato2;
	
};
struct info
{
long dirLogica;
void *dato;
	
};
struct infoGrabar
{
long dir1;
char dato1[512];
long dir2;
char dato2[512];	
};
struct chs
{
int pista;
int sector;
int cabezal;
	
};

typedef struct nodo
{	
	int accion;
	long dirLogica;
	int tiempo;
	int pista;
	void *dato;
	struct nodo *proximo;
}Nodo;

void buscarSector(long, long );
int putSectores(struct infoGrabar);
struct buffer getSectores(long,long);
void crearBase(void);
void grabarCabezal(long);
long leerCabezal(void);
void posCabezal(void);
void consola(void);
struct chs getCHS(long);
Nodo* atenderSector(int);
void borrar(long);
char* leer(long);
void grabar(long ,void *);
Nodo* InsertarNodo(Nodo *,Nodo *);
Nodo* mostrarLista(Nodo *);
Nodo* algoritmo(Nodo *,Nodo *);
void sectLeidos(long);

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
void buscarSector(long cabezal, long dirLogica){
	
	if(cabezal==dirLogica){
			printf("%d ",dirLogica);
	}else{
		if(cabezal>dirLogica){
			while(cabezal!=dirLogica){
					printf("%d ",cabezal);
					cabezal--;
			}
			printf("%d ",cabezal);
		}else{
			while(cabezal!=dirLogica){
				printf("%d ",cabezal);
				cabezal++;
			}
			printf("%d ",cabezal);
		}
	}
}
void sectLeidos(long dirLogica){
	
	long cabezal;
	struct chs cabCHS,dirCHS; 
	
	cabezal=leerCabezal();
	cabCHS=getCHS(cabezal);
	dirCHS=getCHS(dirLogica);
	printf("\nSectores Leidos:");
	if(cabCHS.pista==dirCHS.pista){
		buscarSector(cabezal,dirLogica);
	}else{
		if(cabCHS.pista<dirCHS.pista){
			while(cabCHS.pista!=dirCHS.pista){
					printf("%d ",cabezal);
					cabezal=cabezal+4;
					cabCHS.pista++;
					//cabCHS=getCHS(cabezal);	
			}
			buscarSector(cabezal,dirLogica);
		}else{
			while(cabCHS.pista!=dirCHS.pista){
					printf("%d ",cabezal);
					cabezal=cabezal-4;
					cabCHS.pista--;
					//cabCHS=getCHS(cabezal);	
			}
			buscarSector(cabezal,dirLogica);
		}
	}
}

Nodo* mostrarLista(Nodo *lista) 
{
Nodo *ptr,*ptr2,*cabezal;
int sumaTiempos=0,dir,opcion,esta;
struct chs CHS;
void *datos;
//HANDLE heap = HeapCreate(HEAP_NO_SERIALIZE, 1024*1024, 0);
ptr=lista;
//cabezal=(Nodo *)HeapAlloc(heap,HEAP_NO_SERIALIZE, sizeof(Nodo));
cabezal=(Nodo *)malloc(sizeof(Nodo));
dir=leerCabezal();
cabezal->dirLogica=dir;
CHS=getCHS(dir);
cabezal->pista=CHS.pista;
cabezal->proximo=NULL; 
cabezal->tiempo=0;
printf("\nPosicion:%d\n",dir);
while (lista!=NULL){
		ptr=lista;
		ptr=algoritmo(ptr,cabezal);
		esta=buscarCache(ptr);
		if (esta==1){
					CHS=getCHS(ptr->dirLogica);
					printf("\nSectores Leidos:%d - cache",CHS.sector);
					printf("\nTiempo consumido:%d ns",1);
					datos=leer(ptr->dirLogica);
					printf("\n%s\n",datos);
		}else{
			cabezal->dirLogica=ptr->dirLogica;
			cabezal->pista=ptr->pista;
			dir=leerCabezal();
			CHS=getCHS(dir);
			sumaTiempos=sumaTiempos+abs(ptr->pista-CHS.pista);
			switch(ptr->accion){
				case 1:
					CHS=getCHS(ptr->dirLogica);
					sectLeidos(ptr->dirLogica);
					printf("\nTiempo consumido:%d ms",sumaTiempos);
					printf("\nSector %d: %s\n",ptr->dirLogica,leer(ptr->dirLogica));
				break;
			}
			grabarCabezal(ptr->dirLogica+1);
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
                       
		if (!(ret = dbp->open(dbp, NULL, "VDA.db", NULL, DB_BTREE, flags, 0))) {
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
                       
	ret = dbp->open(dbp, NULL, "VDA.db", NULL, DB_BTREE, 0, 0);		
	
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

ret=dbp->open(dbp, NULL, "VDA.db", NULL, DB_BTREE, 0, 0);
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
                       
	ret = dbp->open(dbp, NULL, "VDA.db", NULL, DB_BTREE, 0, 0);		
	
	clave=-1;
	key.data = &clave;
	key.size = sizeof(int);
    
	data.data = &dirLogica;
	data.ulen = sizeof(long);
	data.flags = DB_DBT_USERMEM;


	dbp->get(dbp, NULL, &key, &data, 0);
	
	
	return(dirLogica);
	
	dbp->close(dbp, 0); 

}	

//Leo Datos

char* leer(long clave){	
DB *dbp;int ret;           
DBT key, data; void *datos; 	
memset(&key, 0, sizeof(DBT)); 
memset(&data, 0, sizeof(DBT)); 
	crearBase();
	ret = db_create(&dbp, NULL, 0);    
                       
	ret = dbp->open(dbp, NULL, "VDA.db", NULL, DB_BTREE, 0, 0);

	key.data = &clave;
	key.size = sizeof(long);
	
	
	dbp->get(dbp, NULL, &key, &data, 0);
	
	if (ret == DB_NOTFOUND) {
		dbp->err(dbp, ret, "El dato no esta en la Base");
	}
	
	if(data.data==NULL){
		datos="\0";
	}else{
		datos=data.data;
	}
	datos=data.data;
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
                       
	ret = dbp->open(dbp, NULL, "VDA.db", NULL, DB_BTREE, 0, 0);
	
	key.data = &clave;
	key.size = sizeof(long);
 
	dbp->del(dbp, NULL, &key, 0); 
	dbp->close(dbp, 0);
}

struct chs	getCHS(long dirLogica){
	struct chs	CHS;
	CHS.pista=(dirLogica/4);	//4 sectores, 1 cabezal , 100 cilindros
	CHS.sector=(dirLogica%4)%4; 
	CHS.cabezal=1;
	return(CHS);
}


void posCabezal(void){
	long dir; struct chs CHS;
	dir=leerCabezal();
	CHS=getCHS(dir);
	printf("CHS:(%d,%d,1)\n",CHS.pista,CHS.sector);
}
Nodo* generarNodo(dir1){
Nodo *nodo1;struct chs posDato;
//HANDLE heap = HeapCreate( 0, 0, 0 );
	posDato=getCHS(dir1);
	//nodo1=(Nodo *)HeapAlloc(GetProcessHeap(),HEAP_NO_SERIALIZE, sizeof(Nodo));
	nodo1=(Nodo *)malloc(sizeof(Nodo));
	nodo1->dirLogica=dir1;
	nodo1->pista=posDato.pista;
	nodo1->tiempo=0; 
	nodo1->proximo=NULL;
return(nodo1);
}
struct buffer getSectores(long dir1,long dir2){

Nodo *nodo,*nodo1=NULL,*nodo2=NULL, *lista=NULL;
struct buffer info;				
	
	nodo1=generarNodo(dir1);
	nodo1->accion=2;
	nodo2=generarNodo(dir2);
	nodo2->accion=2;
	lista=InsertarNodo(lista,nodo1);
	lista=InsertarNodo(lista,nodo2);
	nodo=nodo1;
	while(nodo!=NULL){
		nodo=mostrarLista(lista);	
	}
	info.dato1=leer(dir1);
	info.dato2=leer(dir2);
	return(info);
}

int putSectores(struct infoGrabar datos){

Nodo *nodo1=NULL,*nodo2=NULL,*lista=NULL,*nodo;

int num=0;
	nodo1=generarNodo(datos.dir1);
	nodo1->accion=1;
	nodo1->dato=datos.dato1;
	nodo2=generarNodo(datos.dir2);
	nodo2->dato=datos.dato2;
	nodo2->accion=1;
	lista=InsertarNodo(lista,nodo1);
	lista=InsertarNodo(lista,nodo2);
	mostrarLista(lista);	
	grabar(datos.dir1,datos.dato1);
	grabar(datos.dir2,datos.dato2);
//	free(nodo1);free(nodo2);
return(num);
}