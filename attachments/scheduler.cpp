//Scheduler 1
#include <stdio.h>
#include <sys/types.h>  

#include <iostream>//SOLO PARA IMPRIMIR POR AHORA

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
#define KEY_CPU 1400
#define KEY_CONTADOR 1500
#define MAXBUF_PROCESO 5
#define MAXBUF_CPU 4
#define SLEEP 1000000

using namespace std; 
	
	typedef struct {
		int tipo;
		int estado;   //1.nuevo , 2.listo ,3.ejecutandose , 4.bloqueo , 5.finalizado
		int prioridad;
		int progreso;
	} proceso;

	typedef struct {
		int estado;  //1.disponible     0.no disponible
		int proceso; //id proceso que esta ejecutando
	} cpu;
   //Estado:
   //1.disponible 0.no disponible
	
	
	int main () {
	//PROCESO/////////////////////////////////////////////////////////////////
	proceso * procesos;
	int key_proceso = ftok(FILEKEY, KEY_PROCESO);
	if (key_proceso == -1) {
		fprintf (stderr, "Error con la key (ftok) PROCESO\n");
		return -1; 
	}                     //clave     ,tamañp de la memoria compartida , permisos  lectura escritua 
	int id_zone1 = shmget (key_proceso, sizeof(procesos)*MAXBUF_PROCESO, 0777 | IPC_CREAT);
	if (id_zone1 == -1) {
		fprintf (stderr, "Error creando memoria compartida PROCESO\n");
		return -1; 
	}                     //    identificador, busca esa zoma de memoria no en una direccion absoluta, flag
	procesos = (proceso *)shmat(id_zone1, (char *)0, 0);	//puntero a la zoma de memoria compartida
	if (procesos == NULL) {
		fprintf (stderr, "Error reservando memoria procesos PROCESOS\n");
		return -1; 
	}
	////////////////////////////////////////////////////////////////////////////


	//CPU///////////////////////////////////////////////////////////////////////
	cpu * cpus;
	
	int key_cpu = ftok(FILEKEY, KEY_CPU);
	if (key_cpu == -1) {
		fprintf (stderr, "Error con la key (ftok) CPU\n");
		return -1; 
	}    
	int id_zone2 = shmget (key_cpu, sizeof(cpus)*MAXBUF_CPU, 0777 | IPC_CREAT);
	if (id_zone2 == -1) {
		fprintf (stderr, "Error creando memoria compartida CPU\n");
		return -1; 
	} 
	cpus = (cpu *)shmat(id_zone2, (char *)0, 0);	
	if (cpus == NULL) {
		fprintf (stderr, "Error reservando memoria procesos CPU\n");
		return -1; 
	}
	////////////////////////////////////////////////////////////////////////////
	
	//CONTADOR CPU y PROCESOS///////////////////////////////////////////////////////////////////////
	/* Key to shared memory */
	int key_contador = ftok(FILEKEY, KEY_CONTADOR);
	if (key_contador == -1) {
		fprintf (stderr, "Error con la key (ftok) CPU Y PROCESOS\n");
		return -1; 
	}    
	
	/* we create the shared memory */
	int id_zone3 = shmget (key_contador, sizeof(int)*2, 0777 | IPC_CREAT);
	if (id_zone3 == -1) {
		fprintf (stderr, "Error creando memoria compartida CPU Y PROCESOS \n");
		return -1; 
	} 
		
	int * contador; /* shared buffer */
	/* we declared to zone to share */
	contador = (int *)shmat (id_zone3, (char *)0, 0);
	if (contador == NULL) {
		fprintf (stderr, "Error reservando memoria contador CPU Y PROCESOS\n");
		return -1; 
	}
	
	contador[0]=0;//Contador de CPUs
	contador[1]=0;//Contador de Procesos
	
	
	for (int i=0; i<MAXBUF_PROCESO;i++){
		procesos[i].tipo=-1;
		procesos[i].estado=-1;
		procesos[i].prioridad=-1;
		procesos[i].progreso=0;
	}
	
	for (int i=0; i<MAXBUF_CPU;i++){
		cpus[i].estado=-1;
		cpus[i].proceso=-1;
	}
	////////////////////////////////////////////////////////////////////////////
    
	printf ("Inicializacion: lista \n");
	
	//Scheduler funcionando
	while (1){
		printf ("Scheduler trabajando..\n");
		/*printf ("------------------------------------------\n");
		printf ("Numero de CPUs: %i \n", contador[0]);
		printf ("Numero de Procesos: %i \n", contador[1]);*/
		//printf ("------------------------------------------\n");
		
		for (int i=0; i<MAXBUF_PROCESO;i++){ //Verifico por todos los procesos
			if (procesos[i].estado==2){//Verificar por procesos listos
				for (int j=0; j<contador[0];j++){//Verifico todos los cpu
						if (cpus[j].estado==1 && cpus[j].proceso==-1){ //Verifico por 1 cpu libre
								cpus[j].proceso=i;
							        cout<<"Fue asignado al cpu numero:"<<j << "  el proceso numero:"<<i<<endl;
								break;
						}
				}
			}
		
		}
		usleep(SLEEP);
	}
	///////////////////////////////////////
	
   /* Free the shared memory */
   /*shmdt ((char *)buffer);
   shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);*/
   return 0;
}
