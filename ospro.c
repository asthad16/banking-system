#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include<semaphore.h>

#define NUM_ACC 10

struct account
{
  int credits;
  int debits;
  int accno;
  int balance;
  pthread_mutex_t lock; //the lock on the account involved in transaction
};


struct transaction
{
  int id;
  int amt;             
  struct account * creditor;   
  struct account * debitor;  
  pthread_t thread_id; //POSIX thread id
};

struct account accounts[NUM_ACC];
sem_t lock1,lock2;


    
//for transaction   
void  * transfer(void * args)

{
    struct transaction * trans  = (struct transaction *) args;
 
    if(trans->creditor->accno<trans->debitor->accno)
    {
      pthread_mutex_lock(&accounts[trans->creditor->accno].lock);
      pthread_mutex_lock(&accounts[trans->debitor->accno].lock);
    }
    
    else
    {
      pthread_mutex_lock(&accounts[trans->debitor->accno].lock);
      pthread_mutex_lock(&accounts[trans->creditor->accno].lock);
    }

printf("Initial debit, credit, balance of account %d by thread %d are: %d  %d  %d\n",trans->creditor->accno,trans->id,accounts[trans->creditor->accno].debits,accounts[trans->creditor->accno].credits,accounts[trans->creditor->accno].balance);

printf("Initial debit, credit, balance of account %d by thread %d are: %d  %d  %d\n",trans->debitor->accno,trans->id,accounts[trans->debitor->accno].debits,accounts[trans->debitor->accno].credits,accounts[trans->debitor->accno].balance);

 
   if(accounts[trans->debitor->accno].balance>=1000)
   {
     accounts[trans->creditor->accno].balance += trans->amt;
     accounts[trans->creditor->accno].credits  += trans->amt;

     accounts[trans->debitor->accno].balance -= trans->amt;
     accounts[trans->debitor->accno].debits += trans->amt;

printf("Final debit, credit, balance of account %d by thread %d are: %d  %d  %d\n",trans->creditor->accno,trans->id,accounts[trans->creditor->accno].debits,accounts[trans->creditor->accno].credits,accounts[trans->creditor->accno].balance);

printf("Final debit, credit, balance of account %d by thread%d are: %d  %d  %d\n",trans->debitor->accno,trans->id,accounts[trans->debitor->accno].debits,accounts[trans->debitor->accno].credits,accounts[trans->debitor->accno].balance);

  }
 
  else
    printf("insufficient balance");

    
  if(trans->creditor->accno < trans->debitor->accno)
  {
      pthread_mutex_unlock(&accounts[trans->debitor->accno].lock);
      pthread_mutex_unlock(&accounts[trans->creditor->accno].lock);
  }
  
  else
  {
      pthread_mutex_unlock(&accounts[trans->creditor->accno].lock);
      pthread_mutex_unlock(&accounts[trans->debitor->accno].lock);
  }
 
 sem_post(&lock1);

  return 0;
}

//for details of all account 
void display()
{
  int i;
  printf("\n\n---------DETAILS OF ALL ACCOUNTS----------");
  printf("\n\naccno.\tdebits\tcredits\tbalance\n");

  for(i=0;i<NUM_ACC;i++)
  {
   printf("%d\t%d\t%d\t%d\n",accounts[i].accno,accounts[i].debits,accounts[i].credits,accounts[i].balance);
  }

}

  

int main() 
{
 
 int i,n,j,k;
 sem_init(&lock1,0,1);
 

  for(i=0;i<NUM_ACC;i++)
 {
    accounts[i].accno=i;
    pthread_mutex_init(&accounts[i].lock, NULL);
    accounts[i].balance=1000;
    accounts[i].credits=0;
    accounts[i].debits=0;
  }
 printf("\n\n**********WELCOME TO BANKING SYSTEM***********");
 
 printf("\n\nenter no of transactions");
 scanf("%d",&n);
 
 struct transaction t[n];
 printf("\nenter required data:\n");
  

  for(i=0;i<n;i++)
  {
    sem_wait(&lock1);
    t[i].id = i;
    
    printf("\nenter creditor account no(0-9):");
    scanf("%d",&j);
    
    t[i].creditor=&accounts[j];
    
    printf("\nenter debitor account no(0-9):");
    scanf("%d",&k);
    
    t[i].debitor = &accounts[k];
    
    printf("\nenter transfer amt:");
    scanf("%d",&t[i].amt);
    
    //thread creation
    pthread_create(&(t[i].thread_id), NULL,&transfer, (void *) (&t[i]));

   }
      

  //join the threads
  for(i=0;i<n;i++)
  {
    pthread_join(t[i].thread_id, NULL);
  }

display();
 return 0;
}