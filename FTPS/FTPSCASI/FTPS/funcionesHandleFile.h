#include "funcionesMPS.h"

int enviarSyscallOpen(char *, int, char *);
int enviarSyscallClose(int, int);
char *enviarSyscallList(char *, int);
int enviarSyscallRead(int, int, SOCKET, HANDLE);
int enviarSyscallWrite(int, int, SOCKET, HANDLE);

char *pasarListaArchivosARespuestaFTP(char *);

/*char *obtenerMesEscrito(SYSTEMTIME);
char *obtenerFechaCreacion(WIN32_FIND_DATAA);
char *pasarArchivoAMensajeFTP(WIN32_FIND_DATAA);
void leerArchivosDeCarpeta(char *, char *);
void borrarArchivo(char *, char *);
char *realizarCambioDeDirectorio(char *, char *);
char *getDirectorioNuevo(char *);
void *getDataFromFile(char *, char *);
char *getSizeOfFile(char *, char *);
void leerArchivoParaObtenerData (char *, char*);*/