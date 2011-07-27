#define SOCKET_MAX_BUFFER 5000

typedef struct  {

                char type[8];
                unsigned puerto_datos;
				char IP[16];
				char ftp_path[50];
				int socketKSS;
				int thDatosOK;
				SOCKET socket_comando;
				char envio_o_recibo;
				char buffer[SOCKET_MAX_BUFFER];
				unsigned threadID;
				HANDLE hThreadDatos;
				HANDLE evento1;
				HANDLE evento2;
				HANDLE socketOcupado;
				} reg_cliente;

typedef struct 	{
		
		int (*pfunc)(char *,reg_cliente *);
		char mensaje[5];
		} t_command_handler;

void paraElMain(t_command_handler *);
char *seleccionadorDeRespuesta (char *, char *, char *, char *, int);
void obtenerComando (char *, char *);
void obtenerParametro (char *, char *);
void obtenerParametrosParaPASV(char *, unsigned, char *);
int command_handler(t_command_handler *, char *, char *, reg_cliente *);
