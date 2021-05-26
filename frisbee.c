 #include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

#define PGSIZE 4096
typedef uint lock_t;
int nowtid;
int tnum=0;
int starttid=-1;
static int tids[50];
void ** stacks;
 int pass=1;
 int finalpass=-1;
int donet=0;
lock_t *mylock;
 void
 lock_init(lock_t *lock){

     *lock=0;
 }

 void
 lock_acquire(lock_t *lock){
     while(xchg(lock, 1) != 0);

 }

 void
 lock_release(lock_t *lock){
     *lock=0;
 }
 int
 getpidpos(){
     int i=0;
     for(;i<tnum;i++){
         if(tids[i]==getpid())
             return i;
     }
     return -1;
 }
void routine(){
    while(pass<=finalpass){
        if(nowtid!=getpid())    continue;
        lock_acquire(mylock);
        if(pass>finalpass){
            lock_release(mylock);
            break;
        }
        printf(2,"Pass number no: %d, Thread %d is passing the token to thread %d\n",pass,getpid(),tids[(pass)%tnum]);

        nowtid=tids[(pass)%tnum];
        pass++;
        lock_release(mylock);
    }


     free(stacks[getpidpos()]);
     donet++;
    exit();
}

int
thread_create(void (*routine) (void *), void *arg,int pos)
{
    int tid;

    // Allocate stack
    void *stack;
    if((stack = malloc(PGSIZE*2)) == 0){
        return -1;
    }
    stacks[pos]=stack;

    if((uint)stack % PGSIZE) {
        stack = stack + (PGSIZE - (uint)stack % PGSIZE);
    }

    // Call clone and return thread ID
    tid=clone(stack,PGSIZE,routine,arg);
    return tid;
}



int main(int argc, char *argv[])
{
    mylock=(uint*)malloc(sizeof(uint));
    lock_init(mylock);
    int threadnum=atoi(argv[1]);
    tnum=threadnum;
    int passnum=atoi(argv[2]);
    finalpass=passnum;
    stacks=(void**)malloc(threadnum*sizeof(void*));

    lock_acquire(mylock);
    int i=0;
    for(i=0;i<threadnum;i++){
        int temp=i;
        void *arg=&temp;
        tids[i]=thread_create(routine,arg,i);
    }
    nowtid=tids[0];
    lock_release(mylock);
    exit();
}
