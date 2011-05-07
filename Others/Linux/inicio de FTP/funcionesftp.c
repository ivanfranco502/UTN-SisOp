#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SOCKET_MAX_BUFFER 100

typedef struct  {

//registro del cliente TYPE, PUERTO DE DATOS, CURRENT PATH, QUE MAS?
                char *type;
                int puerto_datos;
                char current_path[100];

                } reg_cliente;

//estructura que tengo que vectorizar con las funciones para cada comando
typedef struct 	{
		
		int (*pfunc)(char *,reg_cliente *);
		char mensaje[5];
		} t_command_handler;


//funciones de respuesta!! hay que agregar el codigo de ivan
//prototipos, podrian ir en el .h dps vemoooos

int rta_PASV (char *,reg_cliente *);
int rta_NOOP (char *,reg_cliente *);
int rta_DELE (char *,reg_cliente *);
int rta_TYPE (char *,reg_cliente *);
int rta_LIST (char *,reg_cliente *);
int rta_CWD (char *,reg_cliente *);
int rta_PWD (char *,reg_cliente *);
int rta_HELP (char *,reg_cliente *);
int rta_RETR (char *,reg_cliente *);
int rta_STOR (char *,reg_cliente *);


int rta_PASV (char *arg,reg_cliente *datos_cliente){
}

int rta_NOOP (char *arg,reg_cliente *datos_cliente){

}

int rta_DELE (char *arg,reg_cliente *datos_cliente){
}

int rta_TYPE (char *arg,reg_cliente *datos_cliente){
}

int rta_LIST (char *arg,reg_cliente *datos_cliente){
}

int rta_CWD (char *arg,reg_cliente *datos_cliente){

}

int rta_PWD (char *arg,reg_cliente *datos_cliente){

}

int rta_HELP (char *arg,reg_cliente *datos_cliente){

}

int rta_RETR (char *arg,reg_cliente *datos_cliente){

}

int rta_STOR (char *arg	,reg_cliente *datos_cliente){

}

int para_el_main(void){

int (*puntero)(char*, reg_cliente*);

t_command_handler vector_comandos[10];

vector_comandos[0].pfunc=&rta_PASV;
strcpy(vector_comandos[0].mensaje,"PASV");
vector_comandos[1].pfunc=&rta_NOOP;
strcpy(vector_comandos[1].mensaje,"NOOP");
vector_comandos[2].pfunc=&rta_DELE;
strcpy(vector_comandos[2].mensaje,"DELE");
vector_comandos[3].pfunc=&rta_TYPE;
strcpy(vector_comandos[3].mensaje,"TYPE");
vector_comandos[4].pfunc=&rta_LIST;
strcpy(vector_comandos[4].mensaje,"LIST");
vector_comandos[5].pfunc=&rta_CWD;
strcpy(vector_comandos[5].mensaje,"CWD");
vector_comandos[6].pfunc=&rta_PWD;
strcpy(vector_comandos[6].mensaje,"PWD");
vector_comandos[7].pfunc=&rta_HELP;
strcpy(vector_comandos[7].mensaje,"HELP");
vector_comandos[8].pfunc=&rta_RETR;
strcpy(vector_comandos[8].mensaje,"RETR");
vector_comandos[9].pfunc=&rta_STOR;
strcpy(vector_comandos[9].mensaje,"STOR");
}
//handler de comandos  recibe el vector de registros (funciones y comandos) el puntero al registro del cliente, el comando y el argumento

int command_handler(t_command_handler *vector_comandos,char *comando, char *argumento,reg_cliente* datos_cliente){

	int i=0;

       	while (strcmp(vector_comandos[i].mensaje,comando)&&(i<10)){
		i++;
	}

	if (i==10){
//aca marco error de comando!!
	}
	else{
		(*(vector_comandos[i].pfunc)) (argumento, datos_cliente);
	}

}



int enviar_comando (int cliente,char *comando,char *buffer){

        int corrector;

        send(cliente,comando,strlen(comando),0);
        printf("enviado comando %s\n",comando);
        corrector=recv(cliente,buffer,SOCKET_MAX_BUFFER,0);
        buffer[corrector]='\0';
        printf("%s\n",buffer);
        sleep(1);
        return 0;
}


int enviar_comando_no_recv (int cliente,char *comando,char *buffer){

        int corrector;

        send(cliente,comando,strlen(comando),0);
        printf("enviado comando %s\n",comando);

        sleep(1);
        return 0;
}


int wait_for (int cliente,char *comando,char *buffer){

        int corrector;

        while(strcmp(buffer,comando)){
                send(cliente,"500 NO PERMITIDO\r\n",strlen("500 NO PERMITIDO\r\n"),0);
                printf("le mande el 500\n");
                corrector=recv(cliente,buffer,SOCKET_MAX_BUFFER,0);
                buffer[corrector]='\0';
                printf("%s\n",buffer);
                sleep(2);
        }
        return 0;
}



