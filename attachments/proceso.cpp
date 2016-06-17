//Proceso 1-*

#include <iostream>
#include <stdio.h>
#include <sys/types.h>

#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h> /* shm*  */

#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>

#define FILEKEY "/bin/cat"
#define KEY_PROCESO 1300
#define MAXBUF_PROCESO 5
#define KEY_CONTADOR 1500
#define KEY_FUNCIONES 1600
#define SLEEP 1000000

using namespace std;

	typedef struct {
		int tipo; 	//1.search , 2.unisntall ,3.install
		int estado;	//1.nuevo , 2.listo ,3.ejecutandose , 4.bloqueo , 5.finalizado
		int prioridad;
		int progreso;
	} proceso;
      
   int main (){
	
	//PROCESO///////////////////////////////////////////////////////////////////////
	proceso * procesos;
	
	int key_proceso = ftok(FILEKEY, KEY_PROCESO);
	if (key_proceso == -1) {
		fprintf (stderr, "Error con la key \n");
		return -1; 
	}    
	int id_zone1 = shmget (key_proceso, sizeof(procesos)*MAXBUF_PROCESO, 0777 | IPC_CREAT);
	if (id_zone1 == -1) {
		fprintf (stderr, "Error con id_zone 1 \n");
		return -1; 
	} 
	procesos = (proceso *)shmat(id_zone1, (char *)0, 0);	
	if (procesos == NULL) {
		fprintf (stderr, "Error reservando memoria procesos\n");
		return -1; 
	}
	
	////////////////////////////////////////////////////////////////////////////
	
	//CONTADOR CPU y PROCESOS///////////////////////////////////////////////////////////////////////
	/* Key to shared memory */
	int key_contador = ftok(FILEKEY, KEY_CONTADOR);
	if (key_contador == -1) {
		fprintf (stderr, "Error con la key \n");
		return -1; 
	}    
	
	/*Creacion de memoria compartida */
	int id_zone3 = shmget (key_contador, sizeof(int)*2, 0777 | IPC_CREAT);
	if (id_zone3 == -1) {
		fprintf (stderr, "Error con id_zone \n");
		return -1; 
	} 
		
	int * contador; /* shared buffer */
	/* we declared to zone to share */
	contador = (int *)shmat (id_zone3, (char *)0, 0);
	if (contador == NULL) {
		fprintf (stderr, "Error reservando memoria contador\n");
		return -1; 
	}
	////////////////////////////////////////////////////////////////////////////
	
	printf ("Memoria compartida: listo \n");
	

	contador[1]+=1; //Contador de CPUs
	
	int pos =0;
	for (int i=0; i<MAXBUF_PROCESO;i++){
		if (procesos[i].tipo!=0 && procesos[i].estado!=0){pos = i ; break;}
	}
	
	procesos[pos].tipo = 0; //Tipo nulo
	procesos[pos].estado = 0; //Estado nulo
	
	//cout<<"POS:"<<pos<<endl;
	
	//printf ("Inicializacion y creacion de buffer : ok \n");
	
	//Crear proceso
	printf ("Creando Proceso:\n");
	int c;
	cout<<"---------------------------------"<<endl;
	cout<<"0  .-Buscar."<<endl;
	cout<<"1  .-Desintalar."<<endl;
	cout<<"2  .-Instalar."<<endl;
	cout<<"---------------------------------"<<endl;
	cout<<"Ingrese tipo: ";
	cin>>c;
	///////////////////////////////////////
	
	//Guardar proceso en el buffer
	procesos[pos].tipo = c;//tipo
	procesos[pos].estado=2;//estado
	///////////////////////////////////////
	
	//Esperar a que finalize
	while (procesos[pos].estado!=5){ //mientras no se haya finalizado
		usleep(SLEEP);
	}
	///////////////////////////////////////
	//Borrar proceso
		procesos[pos].tipo=-1;
		procesos[pos].estado=-1;
		procesos[pos].prioridad=-1;
		contador[1]-=1;
	///////////////////////////////////////
	cout<<"Proceso finalizado exitosamente!"<<endl;
	
	return 0;
}
