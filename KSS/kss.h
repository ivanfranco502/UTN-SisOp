typedef struct sect{
        unsigned int sector;
        struct sect *punteroSector;
}Sector;

typedef struct nTDD{
        unsigned int descriptor; // especie de "handle" al archivo abierto
        char nombreVDA[5];  // ubicacion del archivo. VDA1, VDA2, etc.
        char nombreArchivo[31];  //secuencia de hasta 30 caracteres combinando alfanum�ricos y el car�cter punto
        int tipoApertura; // lectura o escritura (0 o 1)
        long size;// size del archivo. Si es lectura permanece constante, si es escritura se debe actualizar.
        char buffer[1025]; // toda lectura o escritura se realiza en bloques de tamanio fijo 1k
        Sector* sector; // struct con el numero de sector + puntero al proximo num de sector
        struct nTDD* siguiente;
}nodoTDD;

typedef struct{
        char DescriptorID[16];   //unico, si usamos la hora + un random es suficiente.
        char PayloadDescriptor;  //tipo de operacion (handshake, vda, fss, ftp, list, cd, etc)
        int PayloadLenght;  //longitud del payload, 0 si no enviamos nada.
        char Payload[1050];  //mensaje.
        }__attribute__ ((__packed__)) MPS_Package;

typedef struct nodoL{
        int socket;
        char nombre[5];
        struct nodoL* puntero_siguiente;
//	void *request;
	char estado[10];
        }nodo_lista_sockets;

nodoTDD*  generar_nodo();
nodoTDD* agregar_nodo_TDD(nodoTDD*, nodoTDD*);
unsigned int descriptor_nuevo(nodoTDD*);
int buscar_socket(nodo_lista_sockets*, char*);
int buscar_tipo_apertura(nodoTDD*, unsigned int);
int rellenar_sectores(char*);




typedef struct {
                char nombre_request[10];
                nodoTDD* (*punt_request)(nodoTDD*, nodo_lista_sockets*, char*, int, char*);
}posible_request;

nodoTDD* sys_open(nodoTDD*, nodo_lista_sockets*, char*, int, char*);
nodoTDD* sys_close(nodoTDD*, nodo_lista_sockets*, char*, int, char*);
nodoTDD* sys_read(nodoTDD*, nodo_lista_sockets*, char*, int, char*);
nodoTDD* sys_write(nodoTDD*, nodo_lista_sockets*, char*, int, char*);
nodoTDD* sys_list(nodoTDD*, nodo_lista_sockets*, char*, int, char*);

nodoTDD* mount(nodoTDD*, nodo_lista_sockets*, char*, int, char*);
nodoTDD* umount(nodoTDD*, nodo_lista_sockets*, char*, int, char*);
nodoTDD* ls(nodoTDD*, nodo_lista_sockets*, char*, int, char*);
nodoTDD* format(nodoTDD*, nodo_lista_sockets*, char*, int, char*);
nodoTDD* md5sum(nodoTDD*, nodo_lista_sockets*, char*, int, char*);
nodoTDD* tdd_dump(nodoTDD*, nodo_lista_sockets*, char*, int, char*);






int init_vector_requests(posible_request*);

int atender_handshake(nodo_lista_sockets*,nodo_lista_sockets*, MPS_Package*);
int atender_request(nodo_lista_sockets*, MPS_Package*,nodoTDD*, nodo_lista_sockets*,posible_request*);

int print_pkg (MPS_Package*);
nodo_lista_sockets* insertar_nodo_ordenado(nodo_lista_sockets*, nodo_lista_sockets*, int);
nodo_lista_sockets* generar_nodo_sockets();
void sockets_activos(nodo_lista_sockets *);
nodo_lista_sockets*  eliminar_nodo(nodo_lista_sockets*, nodo_lista_sockets*, nodo_lista_sockets*,int);



int buscar_request(char *,posible_request*);
int parseo_parentesis (char *,char *,char *);

nodoTDD* eliminar_nodo_TDD(nodoTDD*, unsigned int);


int encontrar_mayor(nodo_lista_sockets*);