#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "elevator.h"

//We will implement the elevator problem using global variables
//and semaphores to protect the global variables.

//Global Variables and Semaphores:
int currElevatorFloor = 1;
Semaphore *mutexFloor = new Semaphore("Elevator floor mutext semaphore", 1);

int occupancy = 0;
Semaphore *mutexOccupancy = new Semaphore("Semaphore for mutex on elevator capacity", 1);

int nextPersonId = 1;
Semaphore *mutexPersonID = new Semaphore("Semaphore for person ID", 1);

//I want to implement the elevator to at least stop at the end of the demo
//instead of cycling forever.
//Therefore I will use a counter for the number of people waiting and
//a switch to turn the elevator off at the end. 
int numPeopleWaiting = 0;
Semaphore *mutexNumPeople = new Semaphore("Semaphore controls num of people", 1);

bool elevatorOn = 1; //initialize to true.
Semaphore *mutexElevatorSwitch = new Semaphore("Semaphore controls elevator off switch", 1);

ELEVATOR *e;

void ELEVATOR::start(int numFloors)
{
    //printf("Starting Elevator!\n");
    //This boolean will keep track of if the elevator is going up or down.
    //Since the elevator presumably starts on floor 1 and can only  go up,
    //we will initialize it to true. 
    bool up = true; 

    
    //The elevator will simply ascend and descend floors when called
    //and take people where they want to go.
    //However, with the global boolean it should be able to stop
    //when occupancy reaches 0 and start when called.
    while (elevatorOn)
    {
        //printf("Entering eternal while loop!\n");
        //I will start by having the elevator always running and then see if we can implement 
        //a way to call it.
        if(up){
            //printf("Elevator is going up!\n");
            //Elevator goes up one floor:
            //      3. Spin for some time
            for (int j = 0; j < 1000000; j++)
            {
                currentThread->Yield();
            }

            //Entry section----------
            //Call P() before incrementing floor number:
            mutexFloor->P();
            //printf("Semaphore signaled! Incrementing floor number!\n");
            currElevatorFloor = currElevatorFloor + 1;
            
            printf("Elevator arrives on floor %d\n", currElevatorFloor);
            
            //before calling V(), check if it is time to go down.
            if(currElevatorFloor == numFloors){
                //printf("Time to go down!\n");
                up = false;
            }

            //Exit section------------
            mutexFloor->V();
            //printf("Semaphore signaled!\n");

            //Yield so people threads can run:
            currentThread->Yield();          

        }else{
            //If elevator is going down, a similar procedure:
    
            //      3. Spin for some time
            for (int j = 0; j < 1000000; j++)
            {
                currentThread->Yield();
            }

            //Entry section----------
            //Call P() before decrementing floor number:
            mutexFloor->P();
            currElevatorFloor = currElevatorFloor - 1;
            
            printf("Elevator arrives on floor %d\n", currElevatorFloor);
            
            //before calling V(), check if it is time to go up.
            if(currElevatorFloor == 1){
                up = true;
            }

            //Exit section------------
            mutexFloor->V();

            //Yield so people threads can run:
            currentThread->Yield();          

        }

    }//while(elevatorOn)
    
}//Start()

void ElevatorThread(int numFloors)
{
    printf("Elevator function invoked! Elevator has %d floors.\n", numFloors);
    
    e = new ELEVATOR(numFloors);

    e->start(numFloors);
}

ELEVATOR::ELEVATOR(int numFloors)
{
   //Did not use this function 
   //for semaphore implementation
}

void Elevator(int numFloors)
{
    //Create Elevator Thread
    Thread *t = new Thread("Elevator");
    t->Fork(ElevatorThread, numFloors);
}

void ELEVATOR::hailElevator(Person *p)
{
   //did not use this method for the semaphores
   //implementation. 
}

//Fork() can only pass one integer as the second argument.
void PersonThread(int person)
{
    //printf("Starting person thread!\n");

    Person *p = (Person *)person;

    printf("Person %d wants to go to floor %d from %d\n", p->id, p->toFloor, p->atFloor);

    //Increment global counter for number of people waiting for elevator:
    //Whenever this function is called, we want the elevator to be turned on:
    mutexNumPeople->P();
    numPeopleWaiting = numPeopleWaiting + 1;
    mutexNumPeople->V();
    currentThread->Yield();


    bool waiting = true;
    bool isOnElevator = false;
    bool arrived = false;

    while(waiting){
        //Continuously check what floor the elevator is on.
        if(currElevatorFloor == p->atFloor){
            //Check if elevator is at capacity or not:
            if(occupancy < 5){
                //get on the elevator.
                printf("Person %d got into the elevator.\n", p->id);
                
                //increment occupancy:
                mutexOccupancy->P();
                occupancy = occupancy + 1;
                mutexOccupancy->V();

                //used for testing:
                //printf("The occupancy is now %d\n", occupancy);

                waiting = false;
                isOnElevator = true;

            }else{
                //for testing
                //printf("Oh no! The elevator is full and person %d needs to wait.\n", p->id);
                currentThread->Yield();
            }
           //Making sure other threads get to run while inside this loop
           currentThread->Yield();
            
        }
        //etc
        currentThread->Yield();
    }//on elevator

    while(isOnElevator){
        //Continuously check what floor the elevator is on.
        if(currElevatorFloor == p->toFloor){
            //no need to check occupancy before getting off:
             printf("Person %d got out of the elevator.\n", p->id);

                //decrement occupancy:
                mutexOccupancy->P();
                occupancy = occupancy - 1;
                mutexOccupancy->V();

                isOnElevator = false;
                arrived = true; //I didn't use this but I like having it.

                //decrement line of people waiting for elevator:
                mutexNumPeople->P();
                numPeopleWaiting = numPeopleWaiting - 1;
                mutexNumPeople->V();

                //Check if last person has been served:
                if(numPeopleWaiting == 0){
                    mutexElevatorSwitch->P();
                    elevatorOn = false;
                    mutexElevatorSwitch->V();
                }
                
                //Elevator should now stop at the end of the demo
                //instead of cycling forever. 


        }
        currentThread->Yield();
    }

}

int getNextPersonID()
{
    int personId = nextPersonId;
    
    mutexPersonID->P();
    nextPersonId = nextPersonId + 1;
    mutexPersonID->V();
    return personId;
}

void ArrivingGoingFromTo(int atFloor, int toFloor)
{

    //Create Person struct:
    Person *p = new Person;
    p->id = getNextPersonID();
    p->atFloor = atFloor;
    p->toFloor = toFloor;

    //Create Person thread
    Thread *t = new Thread("Person " + p->id);
    t->Fork(PersonThread, (int)p);
}