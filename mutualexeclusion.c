#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <limits.h>
#include <time.h>

sem_t full_buffer;
sem_t empty_buffer;
sem_t counter;
sem_t buffer_current_access; // 0 >> cant use it now /  1 >> available
int N = 5 ; // number of mcounter threads
int buffer_size = 4;
int thread_count = 0 ;
int collectorcount = 0;


// queue (buffer implementation)

int MAX = 3;

int queue_array[3]; // buffer max size here is 3 

int rear = - 1;

int front = - 1;



void enqueue(int add_item)

{

    

    if (rear == MAX - 1){
        printf("Full buffer\n");
    }

    

    else

    {

        if (front == - 1)

        

        front = 0;


        rear = rear + 1;

        queue_array[rear] = add_item;

    }

} 

 

void dequeue()

{

    if (front == - 1 || front > rear)

    {

        

        return ;

    }

    else

    {

        printf("Element deleted from queue is : %d\n", queue_array[front]);

        front = front + 1;

    }

} 

int isFull()
{
    if(rear == MAX - 1)
    return 1;
    else
    {
        return 0;
    }
    
}

int isEmpty()
{
    if (front == - 1 || front > rear)
    return 1;
    else
    {
        return 0;
    }
    
}
  
 




// generate random value for threads for sleeping
int start = 0;
int end = 12;

int generate(){
    return (rand() % (end-start+1))+start;
}


// implementing the functions to utilize threads
// firstly ... producer (monitor)

void* monitor(){
    while (1)
    {
        
        int randsleeping_t = generate();
        sleep(randsleeping_t);

        printf("monitor thread :waiting to read counter\n");
         sem_wait(&counter);
        printf("monitor thread : read counter of value %d \n",thread_count);
         sem_post(&counter);
         if (isFull())
        {
            printf("monitor thread : buffer is full \n");
        }
        sem_wait(&full_buffer); // generating 1 element decrements total elemets available for buffer
        sem_wait(&buffer_current_access); // currently using the buffer (mutual execlusion)
        
        // critical section 
        


        enqueue(thread_count);
        
        printf("monitor thread : writing to the buffer at position %d\n",rear);
        
        thread_count = 0 ;
        
        sem_post(&buffer_current_access);
        sem_post(&empty_buffer); // empty spaces decreased by 1 
    }
    
}


void* counterr(int j){
    while (1)
    {
        int sleeptime = generate();
        sleep(sleeptime);
        printf("counter thread %d : received a message \n",j);
        printf("counter thread %d : waiting to write \n",j);
        sem_wait(&counter);
        // critical section
        thread_count += 1;
        printf("counter thread %d : now adding to counter , counter value : %d\n",j,thread_count);
        sem_post(&counter);
    }
    
}

void* collector(){
    while (1)
    {
        sleep(generate());

            if (isEmpty())
        {
            printf("collector thread : buffer is empty nothing to get !! \n");
        }
        
        sem_wait(&empty_buffer);
        sem_wait(&buffer_current_access);
        //critical section
        

        printf("collector thread : reading from buffer in position %d \n ",front);
        dequeue();
        collectorcount += 1;
        
        sem_post(&buffer_current_access);
        sem_post(&full_buffer);
    }
    
}

int main()
{

    // initializing semaphores
    sem_init(&counter,0,1);
    sem_init(&buffer_current_access,0,1);
    sem_init(&empty_buffer,0,0);
    sem_init(&full_buffer,0,N);
    
    pthread_t mcounter [N];
    pthread_t mmonitor ;
    pthread_t mcollector ;


    // starting to utilize threads 
    //.....
    // producing 

    int i=0;
    for(i=0;i<N;i++){
        int * temp = (int*)malloc(sizeof(int));
        temp = i;
        pthread_create(&mcounter[i],NULL,&counterr,temp);
    }
    pthread_create(&mmonitor,NULL,&monitor,NULL);
    pthread_create(&mcollector,NULL,&collector,NULL);
   
    for ( i = 0; i < N; i++)
    {
         pthread_join(mcounter[i],NULL);
    }
    
    pthread_join(mmonitor,NULL);
    pthread_join(mcollector,NULL);




    return 0;
}


