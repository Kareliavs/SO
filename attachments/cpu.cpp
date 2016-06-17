//CPU 1-*
#include <stdio.h>
#include <sys/types.h>

#include <iostream>

#include <sys/ipc.h>
#include <sys/time.h>
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
#define MAXBUF_CPU 4
#define MAXBUF_PROCESO 5
#define KEY_CONTADOR 1500
#define SLEEP 1000000
#define QUANTUM 30000 //3 segundos

using namespace std;

	typedef struct {
		int estado;  //1.disponible 0.no disponible
		int proceso; //id proceso que esta ejecutando
	} cpu;
	
	typedef struct {
		int tipo;
		int estado;
		int prioridad;
		int progreso;
	} proceso;
	
	cpu * cpus;
	proceso * procesos;
	int * contador;

	void search(int id_proc, int id_cpu) {
		
		struct timeval tv;
		struct timeval start_tv;
	
		gettimeofday(&start_tv, NULL);

		double tiempo = 0.0;
		
		procesos[id_proc].estado=3;
		for (int i = procesos[id_proc].progreso; i<101; ++i) {
			
			usleep(rand()%25000);
			gettimeofday(&tv, NULL);
			tiempo = abs((tv.tv_sec - start_tv.tv_sec) + (tv.tv_usec - start_tv.tv_usec))/10.0;
			
			cout <<"ID de proceso: "<<id_proc<<" Progreso buscar: " << i << "% "<<" --tiempo:"<<tiempo<<endl;
			if (tiempo > QUANTUM){
				cout<<"Quantum finalizado"<<endl;
				procesos[id_proc].progreso = ++i;
				procesos[id_proc].estado = 4;
				cpus[id_cpu].estado=1;
				return;
			}
		}
		procesos[id_proc].progreso = 100; 
		procesos[id_proc].estado=5;
		cout<<"Busqueda terminada"<<endl;
	}

	void uninstall(int id_proc, int id_cpu) {
		struct timeval tv;
		struct timeval start_tv;
	        gettimeofday(&start_tv, NULL);

		double tiempo = 0.0;
		
		procesos[id_proc].estado=3;
		for (int i = procesos[id_proc].progreso; i<101; ++i) {
			
			usleep(rand()%50000);
			gettimeofday(&tv, NULL);
			tiempo = abs((tv.tv_sec - start_tv.tv_sec) + (tv.tv_usec - start_tv.tv_usec))/10.0;
			
			cout <<"Proceso: "<<id_proc<<" Progreso desinstalar: " << i << "% "<<" --tiempo:"<<tiempo<<endl;
			if (tiempo > QUANTUM){
				cout<<"Quantum finalizado"<<endl;
				procesos[id_proc].progreso = ++i;
				procesos[id_proc].estado = 4;
				cpus[id_cpu].estado=1;
				return;
			}
		}
		procesos[id_proc].progreso = 100; 
		procesos[id_proc].estado=5;
		cout<<"Desinstalar terminado"<<endl;
	}

	void install(int id_proc, int id_cpu) {
				struct timeval tv;
		struct timeval start_tv;
	
		gettimeofday(&start_tv, NULL);

		double tiempo = 0.0;
		
		procesos[id_proc].estado=3;
		for (int i = procesos[id_proc].progreso; i<101; ++i) {
			
			usleep(rand()%100000);
			gettimeofday(&tv, NULL);
			tiempo = abs((tv.tv_sec - start_tv.tv_sec) + (tv.tv_usec - start_tv.tv_usec))/10.0;
			
			cout <<"Proceso: "<<id_proc<<" Progreso Instalar: " << i << "% "<<" --tiempo:"<<tiempo<<endl;
			if (tiempo > QUANTUM){
				cout<<"Quantum finalizado"<<endl;
				procesos[id_proc].progreso = ++i;
				procesos[id_proc].estado = 4;
				cpus[id_cpu].estado=1;
				return;
			}
		}
		procesos[id_proc].progreso = 100; 
		procesos[id_proc].estado=5;
		cout<<"Instalacion completa"<<endl;
	}
	
	void (*funciones[3])(int,int) = {search,uninstall,install};
	

int main (){
	//PROCESO/////////////////////////////////////////////////////////////////
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
	
	//CPU///////////////////////////////////////////////////////////////////////

	
	int key_cpu = ftok(FILEKEY, KEY_CPU);
	if (key_cpu == -1) {
		fprintf (stderr, "Error con la key \n");
		return -1; 
	}    
	int id_zone2 = shmget (key_cpu, sizeof(cpus)*MAXBUF_CPU, 0777 | IPC_CREAT);
	if (id_zone2 == -1) {
		fprintf (stderr, "Error con id_zone2 \n");
		return -1; 
	} 
	cpus = (cpu *)shmat(id_zone2, (char *)0, 0);	
	if (cpus == NULL) {
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
	
	/* we create the shared memory */
	int id_zone3 = shmget (key_contador, sizeof(int)*2, 0777 | IPC_CREAT);
	if (id_zone3 == -1) {
		fprintf (stderr, "Error con id_zone \n");
		return -1; 
	} 
	/* we declared to zone to share */
	contador = (int *)shmat (id_zone3, (char *)0, 0);
	if (contador == NULL) {
		fprintf (stderr, "Error reservando memoria contador\n");
		return -1; 
	}
	////////////////////////////////////////////////////////////////////////////
	
	printf ("Memoria compartida: ok \n");
	
	int pos = contador[0];	
	contador[0]+=1; //Contador de CPUs
	cpus[pos].estado = 1; //Estado disponible
	cpus[pos].proceso = -1; //Sin ningun proceso
	
	printf ("Inicializacion y creacion de buffer : ok \n");
		
	//CPU////////////////////////////////////////////////////////
	while (1){
		//printf ("CPU trabajando\n");
		//cout<<"Estado CPU"<<cpus[pos].estado<<endl;
		//cout<<"Tipo CPU"<<cpus[pos].tipo<<endl;
		int id_actual_proceso = cpus[pos].proceso;
		if (procesos[id_actual_proceso].estado==4){//esta bloqueado
			for (int i=0; i<MAXBUF_PROCESO;i++){ //Verifico por todos los procesos
				if (procesos[i].estado==2){//Verificar por procesos listos
					cpus[pos].proceso=i;
					cout<<"Nuevo proceso asignado: "<<i<<endl;
					break;
				}
			}
			procesos[id_actual_proceso].estado=2;
		}
		id_actual_proceso = cpus[pos].proceso;
		if (cpus[pos].estado == 1  && id_actual_proceso !=-1){ //esta disponible?
			cpus[pos].estado=0; // Lo pongo no disponible
			
			funciones[procesos[id_actual_proceso].tipo](id_actual_proceso, pos);
			
			if (procesos[id_actual_proceso].estado==5 ){//termino? limpio el CPU
				cpus[pos].estado=1;	
				cpus[pos].proceso=-1;
				procesos[id_actual_proceso].progreso=0;
			}
			
		}
		usleep(SLEEP);
	}
	/////////////////////////////////////////////////////////////
	
	return 0;
}
