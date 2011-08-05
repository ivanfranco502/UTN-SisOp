typedef struct  {
	char ipKSS[16];
	unsigned puertoKSS;
	int logActivado;
}configFSS;

void obtenerStructFSS(char *buffer, configFSS *configuracionInicial);
int obtenerParametroParaStruct(char *buffer, char *variableParametro, int arrancaContadorBuffer);
void getConfigFSS(configFSS *config);
