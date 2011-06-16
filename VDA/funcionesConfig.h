typedef struct{
	char nombreVDA[50];
	char ipKSS[16];
	unsigned puertoKss[6];
	int cilindros;
	int headers;
	int sectores;
	int retardoRotacional;
	int posicionCabezal;
	int cacheActivada; 
}configVDA;

typedef struct{
	char ipConexion[16];
	unsigned puertoConexion[6];
	char path[100];
	char ipKSS[16];
	unsigned puertoKss[6]; 
}configFTP;

void obtenerStructVDA(char *, configVda *configuracionInicial);
int obtenerParametroParaStruct(char *buffer, char *variableParametro, int arrancaContadorBuffer);
char *damePuerto(char *); 
char *dameIP (char *);
char *getPath (char *);
void getConfigFTP (char *, char *);
void getConfigPath (char *);
char *getIPKSS(char *Buff);
char *getPortKSS(char *Buff);
void getConfigKSS (char *IP, unsigned puerto);
void getConfigVDA(configVDA *config);
