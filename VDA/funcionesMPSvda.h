#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )

PACK(typedef struct{
        char DescriptorID[16];   //unico, si usamos la hora + un random es suficiente.
        char PayloadDescriptor;  //tipo de operacion (handshake, vda, fss, ftp, list, cd, etc)
        int PayloadLenght;  //longitud del payload, 0 si no enviamos nada.
        char Payload[1050];  //mensaje.
}MPS_Package);

SOCKET conectarConKernel(void);

