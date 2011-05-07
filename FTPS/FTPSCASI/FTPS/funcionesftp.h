char *seleccionadorDeRespuesta (char *, char *, char *, char *, int);
char *obtenerComando (char *comando);
char *obtenerParametro (char *comando);
char *obtenerParametrosParaPASV(char *, int);
int printLog(char *,char *, char *, char *, char *);
int command_handler(t_command_handler *, char *, char *, reg_cliente *);