int enviar_comando(int, char *, char *);
int enviar_comando_no_recv(int,char*,char*);
int wait_for (int, char *, char *);
char *seleccionadorDeRespuesta (char *, char *, char *, char *, int);
char *obtenerComando (char *comando);
char *obtenerParametro (char *comando);
char *obtenerParametrosParaPASV(char *, int);