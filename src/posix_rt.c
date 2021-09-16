/*                                                                  
 * POSIX Real Time Example
 * using a single pthread as RT thread
 */
 
#include "worker_thread.h"

#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>


#define WORKER_THREAD_PRIORITY (80)
#define WORKER_THREAD_CPU_AFFINITY (3)
 
 /*
void *thread_func(void *data)
{
        printf("rt thread is running\n");
        while (1) {
            printf("rt thread is running\n");
            usleep(1000);
        }
        return NULL;
}
*/
 
int main(int argc, char* argv[])
{
        struct sched_param param;
        pthread_attr_t attr;
        pthread_t thread;
        int ret;
 
        /* Lock memory */
        if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
                printf("mlockall failed: %m\n");
                exit(-2);
        }
 
        /* Initialize pthread attributes (default values) */
        ret = pthread_attr_init(&attr);
        if (ret) {
                printf("init pthread attributes failed\n");
                goto out;
        }
 
        /* Set a specific stack size  */
        ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
        if (ret) {
            printf("pthread setstacksize failed\n");
            goto out;
        }
 
        /* Set scheduler policy and priority of pthread */
        ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        if (ret) {
                printf("pthread setschedpolicy failed\n");
                goto out;
        }
        param.sched_priority = WORKER_THREAD_PRIORITY;
        ret = pthread_attr_setschedparam(&attr, &param);
        if (ret) {
                printf("pthread setschedparam failed\n");
                goto out;
        }

        /* Use scheduling parameters of attr */
        ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        if (ret) {
                printf("pthread setinheritsched failed\n");
                goto out;
        }
        
        /* Set CPU affinity */
        unsigned int cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
        cpu_set_t cpuset;
        //for (int i=0; i<cpu_count; i++) {
                //CPU_ZERO(&cpuset);
                //CPU_SET(i, &cpuset);
        //}
        CPU_ZERO(&cpuset);
        CPU_SET(WORKER_THREAD_CPU_AFFINITY, &cpuset);
        ret = pthread_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
        if (ret) {
                printf("pthread setaffinity failed\n");
                goto out;
        }


        //ret = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
        //if (ret) {
        //    printf("setsched self failed\n");
        //}
 
        /* Create a pthread with specified attributes */
        ret = pthread_create(&thread, &attr, thread_func, NULL);
        if (ret) {
                printf("create pthread failed\n");
                goto out;
        }
 
        /* Join the thread and wait until it is done */
        ret = pthread_join(thread, NULL);
        if (ret)
                printf("join pthread failed: %m\n");
 
out:
        return ret;
}

