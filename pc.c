#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#define NUM 5 
int queue[NUM];   //array ,circular buffer
sem_t blank_number, product_number; //blank count, product count

//thread for producer
void *producer(void *arg){
  int i = 0;
  while (1) {
    sem_wait(&blank_number);//if there have any blank space in circular buffer,then can produce new item,else wait   
    queue[i] = rand() % 1000 + 1; //produce a random number and store in the circular buffer.
    printf("—-Produce—%d\n", queue[i]);       
    sem_post(&product_number); //increase the product number and wake up a sleep consumer(if there have any).
    i = (i+1) % NUM; //update circular buffer index
    sleep(rand()%3);
  }
}

//thread for consumer
void *consumer(void *arg){
  int i = 0;
  while (1) {
    sem_wait(&product_number); //wait until one random number in buffer
    printf("-Consume—%d\n", queue[i]);
    queue[i] = 0;
    sem_post(&blank_number); //increase the blank number and wake up a sleep producer.
    i = (i+1) % NUM;
    sleep(rand()%3);
  }
}

int main(int argc, char *argv[]){
  pthread_t pid,pid2, cid,cid2;
  int x=1, y=2;
  int *a = &x;
  int *b = &y;
  sem_init(&blank_number, 0, NUM);
  sem_init(&product_number, 0, 0); 
  pthread_create(&pid, NULL, producer, (void*)a);
  //pthread_create(&pid2, NULL, producer, (void*)b);
  pthread_create(&cid, NULL, consumer, (void*)a);
  pthread_create(&cid2, NULL, consumer, (void*)b);
  pthread_join(pid, NULL);
  //pthread_join(pid2, NULL); 
  pthread_join(cid, NULL);
  pthread_join(cid2, NULL);
  sem_destroy(&blank_number);
  sem_destroy(&product_number);
  return 0;
}
