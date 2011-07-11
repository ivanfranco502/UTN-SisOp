



char* existeArchivo (char *vda, char *nombreArchivo);
char* infoArchivo (char *vda, char *nombreArchivo);
char* eliminarArchivo (char *vda, char *nombreArchivo);
char* crearArchivo (char *vda, char *nombreArchivo);
char* actualizarTamanio (char *vda, char *nombreArchivo, long nuevoTamanio);
char* crearTablaSectoresLibres (char *vda, int cantidadSectores);
char* dosSectoresLibres (char *vda);
char* asignarSectores (char *vda, char *nombreArchivo, char *sectores);
char* liberarSectores (char *vda, char *listaSectores);
