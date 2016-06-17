#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#define SHMSIZE 27
#define SHMSIZES 32
#define FILEKEY "/bin/cat"
#define KEY_TURNO 1300
#define MAXBUF_PROCESO 5
#define KEY_CONTADOR 1500
#define KEY_PROCESO 1400
#define KEY_FUNCIONES 1600
#define SLEEP 1000000
/*gcc process3.c -w -o process3.x 
^[[Akarelia@localhost ~/Downloads $ ./process3.x
*/ 

int numero;
int * turno=NULL;
int * interesado=NULL; /* shared buffer */
int * flags=NULL;
char *shm, *s;

int TRUE=1;
int FALSE=0;
void entrar(int proc)
{
  /*int otro;
  otro=(proc+1)%n;
  interesado[proc]= 1;dd
  turno[0]=proc;
  while(turno[0]==proc && interesado[otro]==1);
  sleep(1);*/
  
  int count,k;
  for (count=0;count<interesado[0]-1;count++)
  {
    flags[proc]=count;
    turno[count]=proc;
    sleep(1);
    for(k=0;k<interesado[0]-1;k++)
    { if(k != proc)
      { while (!((flags[k]<count) || (turno[count] != proc)));
      }
    }
  } 
  printf(" recibido ");
}
void salir(int proc)
{
  flags[proc]=-1;
}

main()
{//PROCESO///////////////////////////////////////////////////////////////////////
	
	
	int key_proceso = ftok(FILEKEY, KEY_PROCESO);
	if (key_proceso == -1) {
		fprintf (stderr, "Error con la key \n");
		return -1; 
	}    
	int id_zone0 = shmget (key_proceso, sizeof(flags)*MAXBUF_PROCESO, 0777 | IPC_CREAT);
	if (id_zone0 == -1) {
		fprintf (stderr, "Error con id_zone 1 \n");
		return -1; 
	} 
	flags = shmat(id_zone0, (char *)0, 0);	
	if (flags == NULL) {
		fprintf (stderr, "Error reservando memoria flags\n");
		return -1; 
	}
	
////////////////////////////////////////////////////////////////////////////
	
  
  
 //turno///////////////////////////////////////////////////////////////////////
	
	
	int key_turno = ftok(FILEKEY, KEY_TURNO);
	if (key_turno == -1) {
		fprintf (stderr, "Error con la key \n");
		return -1; 
	}    
	int id_zone1 = shmget (key_turno, sizeof(turno)*MAXBUF_PROCESO, 0777 | IPC_CREAT);
	if (id_zone1 == -1) {
		fprintf (stderr, "Error con id_zone 1 \n");
		return -1; 
	} 
	turno = shmat(id_zone1,NULL , 0);	
	if (turno == NULL) {
		fprintf (stderr, "Error reservando memoria en turno\n");
		return -1; 
	}
	
//////////////////////////////////////////////////////////////////////////// 
 //interesado///////////////////////////////////////////////////////////////////////
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
		
	/* we declared to zone to share */
	interesado = (int *)shmat (id_zone3, (char *)0, 0);
	if (interesado == NULL) {
		fprintf (stderr, "Error reservando memoria interesado\n");
		return -1; 
	}
////////////////////////////////////////////////////////////////////////////


 
 char c;
 int shmid;
 key_t key;
 
 
 /*Lo llama con es ID */
 key = 5678;

 /* Crea segmento */
 if ((shmid = shmget(key, SHMSIZE, IPC_CREAT | 0666)) < 0) {
 perror("shmget");//describe el error
 exit(1);
 }

 /*Añade segmento . */
 if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
 perror("shmat");
 exit(1);
 }

 printf ("Memoria compartida: listo \n");

////////////////////////////////////////////////////////////////////////////

 scanf("%d", &numero);
 int index;
 for(index=0;index<numero;index++)
{
   flags[index] =-1;
}
 for(index=0;index<numero-1;index++)
{
   turno[index] =0;
}

 /*Pone 1 en primer lugar */
 s=shm;
 *s++='1';
 *s=NULL;
 printf("Process1:- he puesto el mensaje %s\n",shm);
  /*
Espera por otros procesos 
sabe donde comienza y donde acaba
 */
int i;

/* while (*shm != '2' && *shm+6 != 'a')
 sleep(1);
 printf("MENSAJE: %s\n",shm);
 while (*shm != '3' && *shm+7 != 'y')
 sleep(1);
 */
pid_t pid1,pid2;
pid1=0, pid2=0;
interesado[0]=0;
while(interesado[0]<numero)
      {
	//while(1){
	  /*if(pid1==0)
	  {
	  pid1=fork();
	  
	  }*/
	  /*entrar(i);
	  
	  salir(i);*/
	  sleep(3);//interesado[0]++
	  printf("\n MENSAJE: %s\n",shm);
	}

	shmdt ((char *)interesado);
	shmctl (id_zone3 , IPC_RMID, (struct shmid_ds *)NULL);
	
	shmdt ((char *)turno);
	shmctl (id_zone1, IPC_RMID, (struct shmid_ds *)NULL);
	
	shmdt ((char *)shm);
	shmctl (shmid, IPC_RMID, (struct shmid_ds *)NULL);	
	
	shmdt ((char *)flags);
	shmctl (id_zone0, IPC_RMID, (struct shmid_ds *)NULL);
 /*printf("Process1:- Process2 ha puesto el mensaje %s\n",shm);
 while (*shm != '3' && *shm+7 != 'y')
 sleep(1);

 printf("Process1:- Process3 ha puesto el mensaje %s\n",shm);*/
 printf("Process1:- Termino, cerrando...\n");
 exit(0);
} 