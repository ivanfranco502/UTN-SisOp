typedef struct  {
	char IPServer[16];
	unsigned puertoServer;
	char pathRaiz[100];
	char IPKernel[16];
	unsigned puertoKernel;
	unsigned puertoFTPAKernel;
}configFTP;

void obtenerStructFTP(char *, configFTP *configuracionInicial);
int obtenerParametroParaStruct(char *buffer, char *variableParametro, int arrancaContadorBuffer);
void getConfigFTP(configFTP *config);
