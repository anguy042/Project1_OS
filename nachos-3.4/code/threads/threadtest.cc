// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h"

// testnum is set in main.cc
int testnum = 1;



#ifdef HW1_SEMAPHORES //SimpleThread() modified version for Excercise 1
int numThreadsActive; // used to implement barrier upon completion
int SharedVariable;
Semaphore *mutex = new Semaphore("simple_thread_semaphore", 1);

void SimpleThread(int which){

    int num, val;
    for(num = 0; num < 5; num++){

        //Entry section----------
        mutex->P();

        val = SharedVariable;
        printf("*** thread %d sees value %d\n", which, val);
        currentThread->Yield();
        SharedVariable = val+1;

        //Exit section------------
        mutex->V();

        currentThread->Yield();
    }

    numThreadsActive = numThreadsActive - 1; //Should I synchronize this?

    while(numThreadsActive > 0){
        currentThread->Yield();
    }


val = SharedVariable;
printf("Thread %d sees final value %d\n", which, val);

}

#endif //HW1 Semaphores end


#ifdef HW1_LOCKS //Exercise 2 code-------------------------------
int numThreadsActive; // used to implement barrier upon completion
int SharedVariable;
Semaphore *mutex = new Semaphore("simple_thread_semaphore", 1);

void SimpleThread(int which){

    int num, val;
    for(num = 0; num < 5; num++){

        //Entry section----------
        mutex->P();

        val = SharedVariable;
        printf("*** thread %d sees value %d\n", which, val);
        currentThread->Yield();
        SharedVariable = val+1;

        //Exit section------------
        mutex->V();

        currentThread->Yield();
    }

    numThreadsActive = numThreadsActive - 1; //Should I synchronize this?

    while(numThreadsActive > 0){
        currentThread->Yield();
    }


val = SharedVariable;
printf("Thread %d sees final value %d\n", which, val);

}
#endif //HW1_LOCKS


#if !defined(HW1_SEMAPHORES) && !defined(HW1_LOCKS)
//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}
#endif //original SimpleThread()

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------
#if defined(HW1_SEMAPHORES) || defined(HW1_LOCKS)
//Both demonstrations of the semaphores and the locks
//require this implementation of ThreadTest() 
//to call the threads. 


// Taken from TA's Materials ~~~~~~~~ 
// Modified version of ThreadTest that takes an integer n
// and creates n new threads, each calling SimpleThread and
// passing on their ID as argument.



void
ThreadTest(int n) {
    DEBUG('t', "Entering SimpleTest");
    Thread *t;
    numThreadsActive = n;
    printf("NumthreadsActive = %d\n", numThreadsActive);

    for(int i=1; i<n; i++)
    {
        t = new Thread("forked thread");
        t->Fork(SimpleThread,i);
    }
    SimpleThread(0);
}


#else //Original version of ThreadTest():
void
ThreadTest()
{
    //printf("I changed Nachos and it is still working.");
    switch (testnum) {
    case 1:
	ThreadTest1();
	break;
    default:
	printf("No test specified.\n");
	break;
    }
}
#endif //ThreadTest() end
