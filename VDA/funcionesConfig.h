typedef struct  {
	char nombreVDA[50];
	char ipKSS[16];
	unsigned puertoKss[6];
	int cilindros;
	int headers;
	int sectores;
	int retardoRotacional;
	int posicionCabezal;
	int cacheActivada; 
	int logActivada;
}configVDA;

void obtenerStructVDA(char *, configVDA *configuracionInicial);
int obtenerParametroParaStruct(char *buffer, char *variableParametro, int arrancaContadorBuffer);
void getConfigVDA(configVDA *config);
