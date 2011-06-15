typedef struct{
	char DescriptorID[15];   //único, si usamos la hora + un random es suficiente.
	char PayloadDescriptor;  //tipo de operacion (handshake, vda, fss, ftp, list, cd, etc)
	int PayloadLenght;  //longitud del payload, 0 si no enviamos nada.
	char Payload[1050];  //mensaje.
}MPS_Package;

/*void generar_DescriptorID(char *);
void llenar_Payload(MPS_Package*, char*, char, int, char*);*/