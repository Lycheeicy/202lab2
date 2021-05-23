 #include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

#define PGSIZE 4096
typedef uint lock_t;

int nowtid;
int pass=1;
int* tids;
int tnum=0;
lock_t *mylock;
 void
 lock_init(lock_t *lock){
     lock=(uint*)malloc(sizeof(uint));
     *lock=0;
 }

 void
 lock_acquire(lock_t *lock){
     while(xchg(lock, 1) != 0)
         ;
 }

 void
 lock_release(lock_t *lock){
     *lock=0;
 }
void routine(void * arg){
     printf(1,"this is thread %d\n",getpid());
     /*
    while(nowtid!=getpid());

    lock_acquire(mylock);
    int nexttid=tids[(pass-1)%tnum];
    printf(3,"Pass number no: %d, Thread %d is passing the token to thread %d",pass,getpid(),nexttid);
    pass++;
    nowtid=nexttid;
    lock_release(mylock);
      */
    exit();
}

int
thread_create(void (*routine) (void *), void *arg)
{
    int tid;
    char* stack;

    // Allocate stack.
    if((stack = malloc(PGSIZE)) == 0){
        return -1;
    }

    // Call clone and return thread ID
    tid=clone(stack,PGSIZE,routine,arg);

    return tid;
}



int main(int argc, char *argv[])
{
    lock_init(mylock);
    int threadnum=atoi(argv[1]);
    tnum=threadnum;
    int passnum=atoi(argv[2]);
    printf(2, "threadnum:%d passnum:%d\n", threadnum,passnum);
    tids=(int*)malloc(threadnum*sizeof(int));
    lock_acquire(mylock);
    int i=0;
    for(i=0;i<threadnum;i++){
        int temp=1;
        void *arg=&temp;
        tids[i]=thread_create(routine,arg);
    }
    nowtid=tids[0];
    lock_release(mylock);
    exit();
}
