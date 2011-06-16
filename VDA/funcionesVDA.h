struct buffer
{
char *dato1;
char *dato2;
	
};

struct info
{
long dirLogica;
char dato[512];
	
};
struct infoGrabar
{
long dir1;
char dato1[512];
long dir2;
char dato2[512];	
int accion;
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
void* leer(long);
void grabar(long ,void *);
Nodo* InsertarNodo(Nodo *,Nodo *);
Nodo* mostrarLista(Nodo *);
Nodo* algoritmo(Nodo *,Nodo *);
void sectLeidos(long);