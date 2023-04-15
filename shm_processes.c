// Worked with Keerthana Pullela
// Kuira Edwards @02942519
#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void  ClientProcess(int SharedMemory[]) {
     int i, accountID, num;
     srand(getpid());
     
     for (i = 0; i < 25; i++) {
          sleep(rand() % 6);
          accountID = SharedMemory[0];

          while (SharedMemory[1] != 1);
          num = rand() % 51;
          printf("Poor student needs $%d\n", num);

          if (SharedMemory[0] <= accountID) {
               accountID -= SharedMemory[0];
               printf("Poor student: withdrew $%d / Balance = $%d\n", SharedMemory[0], accountID);
               SharedMemory[0] = accountID;
          }
          else {
               printf("Poor student: Not Enough Cash ($%d)\n", accountID);
          }
          SharedMemory[1] = 0;
     }
}

void ParentProcess(int SharedMemory[]) {
  int i, accountID, num;
  srand(getpid()); // pseudo random number generator
  
  for(i=0; i<25;i++){
    sleep(rand() % 6);
    accountID = SharedMemory[0];
    
    while(SharedMemory[1] != 0);
    
    if (accountID <= 100) { // deposits money
      num = rand() % 101; // random number between 0-100
      
      if (num % 2 == 0) { 
        accountID += num;
        
        printf("Dear old dad: Deposited $%d / Balance = $%d\n", SharedMemory[0], accountID);
        
      }
      else {
        printf("Dear old dad: Not Enough Cash to Give\n");
      }
      SharedMemory[0] = accountID; 
      SharedMemory[1] = 1; 
    } 
    else  {
      printf("Dear old dad: Student has enough Cash ($%d)\n", accountID);
    }
  }
}

int  main(int  argc, char *argv[])
{
     int    ShmID;
     int    *ShmPTR;
     pid_t  pid;
     int    status;

     ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
     printf("Server has received a shared memory of four integers...\n");

     ShmPTR = (int *) shmat(ShmID, NULL, 0);

     if (*ShmPTR == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     printf("Server has attached the shared memory...\n");

     ShmPTR[0] = 0; // bank account ID
     ShmPTR[1] = 0;
     printf("Server is about to fork a child process...\n");
     
     pid = fork();
     if (pid < 0) {
          printf("*** fork error (Main) ***\n");
          exit(1);
     }
     else if (pid == 0) { //poor student
          ClientProcess(ShmPTR);
          exit(0);
     }
     else {
          ParentProcess(ShmPTR);
     }

     wait(&status);
     printf("Server has detected the completion of its child...\n");
     shmdt((void *) ShmPTR);
     printf("Server has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Server has removed its shared memory...\n");
     printf("Server exits...\n");
     exit(0);
}