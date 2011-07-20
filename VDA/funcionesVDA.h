struct buffer
{
char dato1[512];
char dato2[512];
	
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

char vda[8];

int buscarSector(long, long );
int putSectores(struct infoGrabar *);
char* getSectores(long,long);
void crearBase(void);
void grabarCabezal(long);
long leerCabezal(void);
void posCabezal(void);
void consola(void);
struct chs takeCHS(long);
Nodo* atenderSector(int);
void borrar(long);
char* leer(long);
void grabar(long ,void *);
Nodo* InsertarNodo(Nodo *,Nodo *);
Nodo* mostrarLista(Nodo *);
Nodo* algoritmo(Nodo *,Nodo *);
int sectLeidos(long);
struct chs getCHS(void);
