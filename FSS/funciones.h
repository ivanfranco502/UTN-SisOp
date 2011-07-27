
typedef struct{
	char DescriptorID[16];   //Unico, si usamos la hora + un random es suficiente.
	char PayloadDescriptor;  //tipo de operacion (handshake, vda, fss, ftp, list, cd, etc)
	int PayloadLenght;  //longitud del payload, 0 si no enviamos nada.
	char Payload[1050];  //mensaje.
}__attribute__ ((__packed__)) MPS_Package;



int print_pkg (MPS_Package *);
char* generar_DescriptorID(char *);
char* existeArchivo (char *, char *);
void obtenerFuncion(char *, char * );
void infoArchivo (char *, char *, char *);
char* eliminarArchivo (char *, char *);
char* crearArchivo (char *, char *);
char* actualizarTamanio(char *, char *,double);
char* crearTablaSectoresLibres (char *, int );
void dosSectoresLibres (char *, char *);
char* asignarSectores (char *, char *, char *);
char* liberarSectores (char *, char *);
char* formatear (char* , int);
char* tieneFormato (char *);
void listarDirectorio(char *, char *);
void tamanioArchivo (char *, char *, char *);
