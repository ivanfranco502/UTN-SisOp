#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>




// genera un descriptor aleatorio y lo guarda en DescriptorID que debe ser char [16]
void generar_DescriptorID(char *DescriptorID){
	time_t tiempo;
	char tpo[11], num[6];
	long aleatorio;
	
	strcpy(DescriptorID, "");
	srand(time(NULL));
	time(&tiempo);
	aleatorio = rand() % 90000;
	aleatorio += 10000;
	sprintf(tpo,"%d",tiempo);;
	sprintf(num,"%ld",aleatorio);
	//itoa(tiempo, tpo, 10);
	//ltoa(aleatorio, num, 10);
	strcat(DescriptorID, tpo);
	strcat(DescriptorID, num);
	
}


/* llena el mensaje con los campos siguientes
void llenar_Payload(MPS_Package* mensaje, char* DescriptorID, char PayloadDescriptor, int PayloadLenght, char* Payload){
	strcpy(mensaje->DescriptorID, DescriptorID);
	mensaje->PayloadDescriptor= PayloadDescriptor;
	mensaje->PayloadLenght= PayloadLenght;
	strcpy(mensaje->Payload, Payload);
}
*/
