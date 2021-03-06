#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <string>
#include "kill_patch.h"

using namespace std;

//Associated Files:
//DataHub.cpp
//kill_patch.h
//kill_patch64.o

//Steven Centeno
//Saurav Chhapawala

//Probe C will: 

//I. Probe C will continuously send messages to the DataHub until 

//initializing functions for use
int generateValue();
void sendToHub(int num);


//the magic seed number of the 
int rho = 251;

//retrieves the mesage queue ID that is active, will return a -1 if its not found
int qid = msgget(ftok(".", 'u'), 0);

//the buffer that is used for the messages in the queue
struct buf {
    long mtype;
    char greeting[50]; 
};

//initializing the buffer
buf msg;

//initializing the size of the message buffer
int len;
int value;
bool firstMsgSnt = false;


/*
*    generates the value divisible by rho and returns it
**/
int generateValue() {
    int num;
    bool generated = false;
    while (!generated) {

        //generates a random number and sees if it is divisible by rho
        num = rand();
	    if(num % rho == 0) {
	        generated = true;
        }
    }
    return num;
}

/*
*sends the number generated to the datahub, 
**/
void sendToHub(int num) {

    if(!firstMsgSnt){
        len = sizeof(msg)-sizeof(long);

	    // prepare my pid of C to send
        string messageToSnd = to_string(getpid());

        //The message sent needs the C in front of it because mathmatically if
        //a random value is generated, there is a possibility that
        //the number will be divisible by the Common demominator of alpha, beta and rho
        //the max number of the random function is 2147483647 and alpha * beta = 256299
        //meaning if a number is divisible by both alpha and beta, the datahub wont know
        //where the message truly came from, therfore we must add which probe the message
        //derived from
        strcpy(msg.greeting, "C: ");
        strcat(msg.greeting,  messageToSnd.c_str());
        
        //prepares the mtype (the port to send to)
        msg.mtype = 1;

        //sends the message and prints the message sent to the datahub
        msgsnd(qid, (struct msgbuf *)&msg, len, 0);
        cout << getpid() << ": sends first message: " << msg.greeting << endl;
        firstMsgSnt = true;
    }

    len = sizeof(msg)-sizeof(long);

	// prepare my message to send
    string messageToSnd = to_string(num);

    //The message sent needs the C in front of it because mathmatically if
    //a random value is generated, there is a possibility that
    //the number will be divisible by the Common demominator of alpha, beta and rho
    //the max number of the random function is 2147483647 and alpha * beta = 256299
    //meaning if a number is divisible by both alpha and beta, the datahub wont know
    //where the message truly came from, therfore we must add which probe the message
    //derived from
    strcpy(msg.greeting, "C: ");
    strcat(msg.greeting,  messageToSnd.c_str());
    
    //prepares the mtype (the port to send to)
	msg.mtype = 1;

    //sends the message and prints the message sent to the datahub
	msgsnd(qid, (struct msgbuf *)&msg, len, 0);
    cout << getpid() << ": sends greeting: " << msg.greeting << endl;
}

int main() {

    int loopUntilTerminate = true;

    //buffer to send to the datahub when it leaves
    buf exitMsg;
    len = sizeof(msg)-sizeof(long);
    exitMsg.mtype = 1;
    strcpy(exitMsg.greeting, "C_Leaves");
    kill_patch(qid, (struct msgbuf *)&exitMsg, len, 1);

    while (loopUntilTerminate) {
        //produce reading 
        value = generateValue();
        sendToHub(value);
    }
}
