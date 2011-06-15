#define SOCKET_MAX_BUFFER 5000

typedef struct  {

                char type[8];
                unsigned puerto_datos;
				char IP[16];
                char current_path[100];
				char original_path[100];
				char ftp_path[50];
				char ipKSS[16];
				unsigned puertoKSS;
				SOCKET socketKSS;
				SOCKET socket_comando;
				char envio_o_recibo;
				char buffer[SOCKET_MAX_BUFFER];
				unsigned threadID;
				HANDLE hThreadDatos;
				HANDLE evento1;
				HANDLE evento2;
				} reg_cliente;

typedef struct 	{
		
		int (*pfunc)(char *,reg_cliente *);
		char mensaje[5];
		} t_command_handler;

void paraElMain(t_command_handler *);
char *seleccionadorDeRespuesta (char *, char *, char *, char *, int);
char *obtenerComando (char *comando);
char *obtenerParametro (char *comando);
char *obtenerParametrosParaPASV(char *, unsigned);
int command_handler(t_command_handler *, char *, char *, reg_cliente *);
