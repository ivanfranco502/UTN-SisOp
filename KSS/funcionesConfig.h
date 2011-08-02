typedef struct  {
	unsigned puertoFTPS;
	unsigned puertoVDA;
	int logActivado;
}configKSS;

void obtenerStructKSS(char *buffer, configKSS *configuracionInicial);
int obtenerParametroParaStruct(char *buffer, char *variableParametro, int arrancaContadorBuffer);
void getConfigKSS(configKSS *config);
