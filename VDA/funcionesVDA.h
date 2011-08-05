#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
struct buffer
{
char dato1[512];
char dato2[512];
	
};
struct caches
{
long dirLog;
long count;
	
};
PACK(struct infoGrabar
{
	long dir1;
	char dato1[512];
	long dir2;
	char dato2[512];	
});
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

char vda[8];
struct caches cache[10];


int buscarSector(long, long );
int putSectores(struct infoGrabar *,HANDLE);
void getSectores(long,long,char *,HANDLE);
void crearBase(void);
void grabarCabezal(long);
long leerCabezal(void);
void posCabezal(void);
void consola(void);
struct chs takeCHS(long);
Nodo* atenderSector(int);
void borrar(long);
void leer(long,char *);
void grabar(long ,void *);
Nodo* InsertarNodo(Nodo *,Nodo *);
Nodo* mostrarLista(Nodo *,HANDLE);
Nodo* algoritmo(Nodo *,Nodo *);
int sectLeidos(long);
struct chs getCHS(void);
Nodo* generarNodo(long,HANDLE);