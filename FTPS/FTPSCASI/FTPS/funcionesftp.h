#define SOCKET_MAX_BUFFER 1000

typedef struct  {

                char type[8];
                int puerto_datos;
                char current_path[100];
				SOCKET socket_comando;
				char envio_o_recibo;
				char buffer[SOCKET_MAX_BUFFER];
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
char *obtenerParametrosParaPASV(char *, int);
int printLog(char *,char *, char *, char *, char *);
int command_handler(t_command_handler *, char *, char *, reg_cliente *);