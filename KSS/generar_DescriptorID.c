#include <stdio.h>
#include <time.h>
#include <stdlib.h>

generar_descriptorID(){
       time_t tiempo;
       char tpo[11], num[6], DescriptorID[16];
       long aleatorio;
       
       *DescriptorID = "";
       srand(time(NULL));
       time(&tiempo);
       aleatorio = rand() % 90000;
       aleatorio += 10000;
       itoa(tiempo, tpo, 10);
       ltoa(aleatorio, num, 10);
       strcat(DescriptorID, tpo);
       strcat(DescriptorID, num);
       printf("%d\n", tiempo);
       printf("%d\n", aleatorio);
       printf("%s\n", DescriptorID);
       
       getchar();
}
