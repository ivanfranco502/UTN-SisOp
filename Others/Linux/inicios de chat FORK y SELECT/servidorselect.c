#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


#define SOCKET_MAX_BUFFER 100


int main (){


        int addrlen = sizeof (struct sockaddr_in);
        char buffer[SOCKET_MAX_BUFFER];
        char msj_bienvenida[20]="bienvenido sorete\0";
        char mensaje[50];
        int pid=0;


        int fd_vec[10];         //se me ocurre 1 server y 9 clientes
        int contador_descriptores=0;
        fd_set *set_sockets=malloc(sizeof(fd_set));
        int i=0;
        int j;

        //creo el socket

        int descriptor= socket(AF_INET,SOCK_STREAM,0);
        int remote_client;

        struct sockaddr_in *local_address= malloc(sizeof (struct sockaddr_in));
        struct sockaddr_in *remote_address= malloc(sizeof (struct sockaddr_in));



        local_address->sin_family = AF_INET;
        local_address->sin_addr.s_addr=INADDR_ANY;
        local_address->sin_port = htons (20);


        bind (descriptor,(struct sockaddr *) local_address, sizeof (struct sockaddr_in));

        listen(descriptor,100);

        printf("estoy escuchando el puerto de mierda\n");

        fd_vec[contador_descriptores]=descriptor;            //coloco el descriptor del socket que hace de listener en la pos 0

        FD_ZERO(set_sockets);
        FD_SET(descriptor, set_sockets);                //agrego el socket al SET

//      sleep(2);

        while (strcmp(buffer,"apagar")){

                for (j=0;j<=contador_descriptores;j++){                 //agrego todos los descriptores al set
                FD_SET(fd_vec[j],set_sockets);

                }

                printf("estoy con el select...\n");
                select(fd_vec[contador_descriptores]+1, set_sockets,NULL,NULL,NULL);

                while(!FD_ISSET(fd_vec[i],set_sockets) ){               //busco quien llamo

                                        i++;
                }

                FD_ZERO(set_sockets);                           //limpio la bolsa de sockets

                if (fd_vec[i]==descriptor){                      //llamo el listener o sea hay una nueva conexion

                        remote_client = accept (descriptor, (struct sockaddr *)remote_address, &addrlen);
                        contador_descriptores++;
                        fd_vec[contador_descriptores]=remote_client;
                        printf("nueva conexion, el nuevo socket es:   %d\n ",remote_client);
                        printf("el contador de descriptores esta en %d\n",contador_descriptores);

                }


                else{                           //mando un mensaje algun cliente en el socket fd_vec[i]

                        recv(fd_vec[i],buffer,SOCKET_MAX_BUFFER,0);
                        printf("el mensaje recibido de %d  fue: %s\n",fd_vec[i],buffer);

                        for(j=1;j<=contador_descriptores;j++) {

                                if (j!=i){
                                        send(fd_vec[j],buffer,strlen(buffer)+1,0);
                                }
                        }
                }

        i=0;

        }

        close(descriptor);


return 0;

 

}
