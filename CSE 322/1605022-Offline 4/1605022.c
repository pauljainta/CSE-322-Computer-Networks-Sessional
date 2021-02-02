#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: SLIGHTLY MODIFIED
 FROM VERSION 1.1 of J.F.Kurose

   This code should be used for PA2, unidirectional or bidirectional
   data transfer protocols (from A to B. Bidirectional transfer of data
   is for extra credit and is not required).  Network properties:
   - one way network delay averages five time units (longer if there
       are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
       or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
       (although some can be lost).
**********************************************************************/

#define BIDIRECTIONAL 1 /* change to 1 if you're doing extra credit */
/* and write a routine called B_output */

/* a "pkt" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 5 via the students transport level protocol entities.         */


/* a packet is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined packet structure, which all   */
/* students must follow. */
#define data_frame 0
#define ack_frame 1
#define pig_frame 2
#define A 0
#define B 1
#define payload_size 4
#define invalid -1


struct pkt
{
    char data[payload_size];
};

struct frm
{
    int seqnum;
    int acknum;
    int checksum;
    char payload[payload_size];
    int type;
};

/********* FUNCTION PROTOTYPES. DEFINED IN THE LATER PART******************/
void starttimer(int AorB, float increment);
void stoptimer(int AorB);
void tolayer1(int AorB, struct frm packet);
void tolayer3(int AorB, char datasent[payload_size]);

int checksum(struct frm frame);

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/




struct frm frameA;
struct frm frameB;

struct frm ackfrmA;
struct frm ackfrmB;

int piggybacking;

int ackflagA;
int ackflagB ;


int outAck_B ;
int outAck_A ;

float timer_increment;

struct frm prevSentfrm_A;
struct frm prevSentfrm_B;

struct frm prevReceivedfrm_A;
struct frm prevReceivedfrm_B;




/* called from layer 3, passed the data to be sent to other side */
void A_output(struct pkt packet)
{
    if(ackflagA == 1)
    {
         ackflagA = 0;
      
        printf("Packet %s received at A_output\n",packet.data);

        if(prevSentfrm_A.seqnum==1) frameA.seqnum=0;
        else frameA.seqnum=1;

        


        if(outAck_A == 1)
        {
            frameA.type = pig_frame;
            frameA.acknum=prevReceivedfrm_A.seqnum;

            outAck_A = 0;
        }
        else
        {
            frameA.type = data_frame;
            frameA.acknum = invalid;
        }
        strcpy(frameA.payload , packet.data);

        frameA.checksum = checksum(frameA);


        if(prevSentfrm_A.seqnum==0) prevSentfrm_A.seqnum=1;
        else prevSentfrm_A.seqnum=0;
        
        
       

        printf("Frame seq no %d sent from A_output\n",frameA.seqnum);

        starttimer(A , timer_increment);



        tolayer1(A , frameA);
        return;
    
    }

    printf("Error at A side!!!Previous sending was not complete\n\n ");
}

/* need be completed only for extra credit */
void B_output(struct pkt packet)
{
    if(ackflagB == 1)
    {
        ackflagB =0;

       printf("Packet %s received at B_output\n",packet.data);

        if(prevSentfrm_B.seqnum==1) frameB.seqnum=0;
        else frameB.seqnum=1;

       


        if(outAck_B == 1)
        {
            frameB.type = pig_frame;
            frameB.acknum=prevReceivedfrm_B.seqnum;

            outAck_B = 0;
        }
        else
        {
            frameB.type = data_frame;
            frameB.acknum = invalid;
        }

         strcpy(frameB.payload , packet.data);

        frameB.checksum = checksum(frameB);

        if(prevSentfrm_B.seqnum==0) prevSentfrm_B.seqnum=1;
        else prevSentfrm_B.seqnum=0;



        

        printf("Frame seq no %d sent from B_output\n",frameB.seqnum);

        starttimer(B , timer_increment);


        tolayer1(B , frameB);

        return;
    }

    printf("Error at B side!!!Previous sending was not complete\n\n ");
}

/* called from layer 1, when a packet arrives for layer 4 */
void A_input(struct frm frame)
{
    

    if(checksum(frame) == frame.checksum)
    {
          if(frame.type == ack_frame)
        {
            printf("Acknowledgement Received at A_input for %d\n ",frame.acknum);
            stoptimer(A);

            ackflagA = 1;


        }

        else if(frame.type == data_frame)
        {
            printf("Data frame %d received at A_input\n",frame.seqnum);

            if(prevReceivedfrm_A.seqnum != frame.seqnum)
            {
                printf("At A_input,Data frame is normal(not duplicate)\n");

                tolayer3(A , frame.payload);


               if(piggybacking==1)
               {
                 outAck_A = 1;

                 prevReceivedfrm_A.seqnum=frame.seqnum;

                 printf("Waiting to send acknowledgement to A\n");

               }
               else
               {

                   ackfrmA.seqnum=frame.seqnum;
                   ackfrmA.acknum=frame.acknum;
                   ackfrmA.type=ack_frame;
                   ackfrmA.checksum=checksum(ackfrmA);
                   
                   printf("Sending acknowledgement frame from A_input\n");

                   tolayer1(A , ackfrmA);
                   

               }
               
                
            }

            else
            {
                printf("Duplicate frame received at A_input,sequence no:%d\n",frame.seqnum);
                ackfrmA.seqnum = frame.seqnum;
                ackfrmA.acknum = frame.seqnum;
                ackfrmA.type = ack_frame;
                ackfrmA.checksum = checksum(ackfrmA);

                outAck_A = 0;


              
               printf("Sending acknowledgement frame from A_input\n");

                tolayer1(A , ackfrmA);
            }
          

        }
      
        else
        {
            printf("Piggyback frame %d received at A_input\n",frame.seqnum); 

            if(prevReceivedfrm_A.seqnum!= frame.seqnum)
            {
                stoptimer(A);

                ackflagA = 1;


                tolayer3(A , frame.payload);

                outAck_A = 1;

                prevReceivedfrm_A.seqnum=frame.seqnum;

                printf("At A_input,Data frame is normal(not duplicate),waiting to send acknowledgement to B\n");

            }

            else
            {
                printf("Duplicate frame received at A_input,sequence no:%d\n",frame.seqnum);

                ackfrmA.seqnum = frame.seqnum;
                ackfrmA.acknum = frame.seqnum;
                ackfrmA.type = ack_frame;
                ackfrmA.checksum = checksum(ackfrmA);

                outAck_A = 0;


                printf("Sending acknowledgement frame from A_input\n");
                tolayer1(A , ackfrmA);
            }
          
        }

        return;
      
    }
    
   printf("Corrupt frame received at A_input\n");
    
}

/* called when A's timer goes off */
void A_timerinterrupt(void)
{
    printf("Inside A timerinterrupt\n");
    starttimer(A , timer_increment);


    printf("Exiting A timerinterrupt\n");

    tolayer1(A , frameA);
    
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init(void)
{
    timer_increment=50.0;
    frameA.acknum = invalid;
    frameA.seqnum = invalid;
    frameA.checksum = 0;

    ackfrmA.acknum = invalid;
    ackfrmA.seqnum = invalid;
    ackfrmA.checksum = 0;
    prevSentfrm_A.seqnum=1;
    prevReceivedfrm_A.seqnum=1;
    ackflagA=1;
    outAck_A=0;


}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 1, when a packet arrives for layer 2 at B*/
void B_input(struct frm frame)
{

    if(checksum(frame) == frame.checksum)
    {

        if(frame.type == ack_frame)
        {
            printf("Acknowledgement Received at B_input for %d\n ",frame.acknum);

            stoptimer(B);

            ackflagB = 1;

        }

        else if(frame.type == data_frame)
         {
             printf("Data frame %d received at B_input\n",frame.seqnum);


            if(prevReceivedfrm_B.seqnum != frame.seqnum)
            {
            
              printf("At B_input,Data frame is normal(not duplicate)\n");
                
               if(piggybacking==1)
               {
                 outAck_B = 1;

                 prevReceivedfrm_B.seqnum=frame.seqnum;
                 printf("Waiting to send acknowledgement to A\n");


               }
               else
               {
                   ackfrmB.seqnum=frame.seqnum;
                   ackfrmB.acknum=frame.acknum;
                   ackfrmB.type=ack_frame;
                   ackfrmB.checksum=checksum(ackfrmB);
                   
                   printf("Sending acknowledgement frame from B_input\n");

                  tolayer1(B , ackfrmB);
                   

               }

            }


            else
            {
                printf("Duplicate frame received at B_input,sequence no:%d\n",frame.seqnum);              
                ackfrmB.seqnum = frame.seqnum;
                ackfrmB.acknum = frame.seqnum;
                ackfrmB.type = ack_frame;
                ackfrmB.checksum = checksum(ackfrmB);

                outAck_B = 0;

               printf("Sending acknowledgement frame from B_input\n");

                tolayer1(B , ackfrmB);
            }
           
         }
     
        else
        {

         printf("Piggyback frame %d received at B_input\n",frame.seqnum); 


            if(prevReceivedfrm_B.seqnum != frame.seqnum)
            {
                stoptimer(B);

                ackflagB = 1;
                
                tolayer3(B , frame.payload);

                outAck_B = 1;

                prevReceivedfrm_B.seqnum=frame.seqnum;
                printf("At B_input,Data frame is normal(not duplicate),waiting to send acknowledgement to A\n");


            }

            else
            {
                 outAck_B = 0;
                 printf("Duplicate frame received at B_input,sequence no:%d\n",frame.seqnum);

                ackfrmB.seqnum = frame.seqnum;
                ackfrmB.acknum = frame.seqnum;
                ackfrmB.type = ack_frame;
                ackfrmB.checksum = checksum(ackfrmB);

                

                printf("Sending acknowledgement frame from B_input\n");

                tolayer1(B , ackfrmB);
            }
         
        }

        return;
     
    }
    
   printf("Corrupt frame received at B_input\n");
    
}

/* called when B's timer goes off */
void B_timerinterrupt(void)
{
    printf("Inside B timerinterrupt\n");
    starttimer(B , timer_increment);

    printf("Exiting B timerinterrupt\n");

    tolayer1(B , frameB);
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init(void)
{
    ackfrmB.acknum = invalid;
    ackfrmB.seqnum = invalid;
    ackfrmB.checksum = 0;

    frameB.acknum = invalid;
    frameB.seqnum = invalid;
    frameB.checksum = 0;
    prevSentfrm_B.seqnum=1;
    prevReceivedfrm_B.seqnum=1;
    ackflagB=1;
    outAck_B=0;

 
}

/*****************************************************************
***************** NETWORK EMULATION CODE STARTS BELOW ***********
The code below emulates the layer 3 and below network environment:
    - emulates the tranmission and delivery (possibly with bit-level corruption
        and packet loss) of packets across the layer 3/4 interface
    - handles the starting/stopping of a timer, and generates timer
        interrupts (resulting in calling students timer handler).
    - generates message to be sent (passed from later 5 to 4)

THERE IS NOT REASON THAT ANY STUDENT SHOULD HAVE TO READ OR UNDERSTAND
THE CODE BELOW.  YOU SHOLD NOT TOUCH, OR REFERENCE (in your code) ANY
OF THE DATA STRUCTURES BELOW.  If you're interested in how I designed
the emulator, you're welcome to look at the code - but again, you should have
to, and you defeinitely should not have to modify
******************************************************************/

struct event
{
    float evtime;       /* event time */
    int evtype;         /* event type code */
    int eventity;       /* entity where event occurs */
    struct frm *pktptr; /* ptr to packet (if any) assoc w/ this event */
    struct event *prev;
    struct event *next;
};
struct event *evlist = NULL; /* the event list */

/* possible events: */
#define TIMER_INTERRUPT 0
#define FROM_LAYER3 1
#define FROM_LAYER1 2

#define OFF 0
#define ON 1

int TRACE = 1;     /* for my debugging */
int nsim = 0;      /* number of messages from 5 to 4 so far */
int nsimmax = 0;   /* number of msgs to generate, then stop */
float time = 0.000;
float lossprob;    /* probability that a packet is dropped  */
float corruptprob; /* probability that one bit is packet is flipped */
float lambda;      /* arrival rate of messages from layer 5 */
int ntolayer1;     /* number sent into layer 3 */
int nlost;         /* number lost in media */
int ncorrupt;      /* number corrupted by media*/

void init();
void generate_next_arrival(void);
void insertevent(struct event *p);

int main()
{
    struct event *eventptr;
    struct pkt msg2give;
    struct frm pkt2give;

    int i, j;
    char c;

    init();
    A_init();
    B_init();

    while (1)
    {
        eventptr = evlist; /* get next event to simulate */
        if (eventptr == NULL)
            goto terminate;
        evlist = evlist->next; /* remove this event from event list */
        if (evlist != NULL)
            evlist->prev = NULL;
        if (TRACE >= 2)
        {
            printf("\nEVENT time: %f,", eventptr->evtime);
            printf("  type: %d", eventptr->evtype);
            if (eventptr->evtype == 0)
                printf(", timerinterrupt  ");
            else if (eventptr->evtype == 1)
                printf(", fromlayer3 ");
            else
                printf(", fromlayer1 ");
            printf(" entity: %d\n", eventptr->eventity);
        }
        time = eventptr->evtime; /* update time to next event time */
        if (eventptr->evtype == FROM_LAYER3)
        {
            if (nsim < nsimmax)
            {
                if (nsim + 1 < nsimmax)
                    generate_next_arrival(); /* set up future arrival */
                /* fill in pkt to give with string of same letter */
                j = nsim % 26;
                for (i = 0; i < payload_size; i++)
                    msg2give.data[i] = 97 + j;
                msg2give.data[payload_size-1] = 0;
                if (TRACE > 2)
                {
                    printf("          MAINLOOP: data given to student: ");
                    for (i = 0; i < payload_size; i++)
                        printf("%c", msg2give.data[i]);
                    printf("\n");
                }
                nsim++;
                if (eventptr->eventity == A)
                    A_output(msg2give);
                else
                    B_output(msg2give);
            }
        }
        else if (eventptr->evtype == FROM_LAYER1)
        {
            pkt2give.type = eventptr->pktptr->type;
            pkt2give.seqnum = eventptr->pktptr->seqnum;
            pkt2give.acknum = eventptr->pktptr->acknum;
            pkt2give.checksum = eventptr->pktptr->checksum;
            for (i = 0; i < payload_size; i++)
                pkt2give.payload[i] = eventptr->pktptr->payload[i];
            if (eventptr->eventity == A) /* deliver packet by calling */
                A_input(pkt2give); /* appropriate entity */
            else
                B_input(pkt2give);
            free(eventptr->pktptr); /* free the memory for packet */
        }
        else if (eventptr->evtype == TIMER_INTERRUPT)
        {
            if (eventptr->eventity == A)
                A_timerinterrupt();
            else
                B_timerinterrupt();
        }
        else
        {
            printf("INTERNAL PANIC: unknown event type \n");
        }
        free(eventptr);
    }

terminate:
    printf(
        " Simulator terminated at time %f\n after sending %d msgs from layer5\n",
        time, nsim);
}

void init() /* initialize the simulator */
{
    int i;
    float sum, avg;
    float jimsrand();

    printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
    printf("Enter the number of messages to simulate: ");
    scanf("%d",&nsimmax);
    printf("Enter  frame loss probability [enter 0.0 for no loss]:");
    scanf("%f",&lossprob);
    printf("Enter frame corruption probability [0.0 for no corruption]:");
    scanf("%f",&corruptprob);
    printf("Enter average time between messages from sender's layer3 [ > 0.0]:");
    scanf("%f",&lambda);
    printf("Enter TRACE:");
    scanf("%d",&TRACE);
    printf("Enter piggybacking:");
    scanf("%d",&piggybacking);
  

    srand(9999); /* init random number generator */
    sum = 0.0;   /* test random number generator for students */
    for (i = 0; i < 1000; i++)
        sum = sum + jimsrand(); /* jimsrand() should be uniform in [0,1] */
    avg = sum / 1000.0;
    if (avg < 0.25 || avg > 0.75)
    {
        printf("It is likely that random number generation on your machine\n");
        printf("is different from what this emulator expects.  Please take\n");
        printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
        exit(1);
    }

    ntolayer1 = 0;
    nlost = 0;
    ncorrupt = 0;

    time = 0.0;              /* initialize time to 0.0 */
    generate_next_arrival(); /* initialize event list */
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand(void)
{
    double mmm = RAND_MAX;
    float x;                 /* individual students may need to change mmm */
    x = rand() / mmm;        /* x should be uniform in [0,1] */
    return (x);
}

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/

void generate_next_arrival(void)
{
    double x, log(), ceil();
    struct event *evptr;
    float ttime;
    int tempint;

    if (TRACE > 2)
        printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");

    x = lambda * jimsrand() * 2; /* x is uniform on [0,2*lambda] */
    /* having mean of lambda        */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtime = time + x;
    evptr->evtype = FROM_LAYER3;
    if (BIDIRECTIONAL && (jimsrand() > 0.5))
        evptr->eventity = B;
    else
        evptr->eventity = A;
    insertevent(evptr);
}

void insertevent(struct event *p)
{
    struct event *q, *qold;

    if (TRACE > 2)
    {
        printf("            INSERTEVENT: time is %lf\n", time);
        printf("            INSERTEVENT: future time will be %lf\n", p->evtime);
    }
    q = evlist;      /* q points to header of list in which p struct inserted */
    if (q == NULL)   /* list is empty */
    {
        evlist = p;
        p->next = NULL;
        p->prev = NULL;
    }
    else
    {
        for (qold = q; q != NULL && p->evtime > q->evtime; q = q->next)
            qold = q;
        if (q == NULL)   /* end of list */
        {
            qold->next = p;
            p->prev = qold;
            p->next = NULL;
        }
        else if (q == evlist)     /* front of list */
        {
            p->next = evlist;
            p->prev = NULL;
            p->next->prev = p;
            evlist = p;
        }
        else     /* middle of list */
        {
            p->next = q;
            p->prev = q->prev;
            q->prev->next = p;
            q->prev = p;
        }
    }
}

void printevlist(void)
{
    struct event *q;
    int i;
    printf("--------------\nEvent List Follows:\n");
    for (q = evlist; q != NULL; q = q->next)
    {
        printf("Event time: %f, type: %d entity: %d\n", q->evtime, q->evtype,
               q->eventity);
    }
    printf("--------------\n");
}

/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */
void stoptimer(int AorB /* A or B is trying to stop timer */)
{
    struct event *q, *qold;

    if (TRACE > 2)
        printf("          STOP TIMER: stopping timer at %f\n", time);
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB))
        {
            /* remove this event */
            if (q->next == NULL && q->prev == NULL)
                evlist = NULL;          /* remove first and only event on list */
            else if (q->next == NULL) /* end of list - there is one in front */
                q->prev->next = NULL;
            else if (q == evlist)   /* front of list - there must be event after */
            {
                q->next->prev = NULL;
                evlist = q->next;
            }
            else     /* middle of list */
            {
                q->next->prev = q->prev;
                q->prev->next = q->next;
            }
            free(q);
            return;
        }
    printf("Warning: unable to cancel your timer. It wasn't running.\n");
}

void starttimer(int AorB /* A or B is trying to start timer */, float increment)
{
    struct event *q;
    struct event *evptr;

    if (TRACE > 2)
        printf("          START TIMER: starting timer at %f\n", time);
    /* be nice: check to see if timer is already started, if so, then  warn */
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB))
        {
            printf("Warning: attempt to start a timer that is already started\n");
            return;
        }

    /* create future event for when timer goes off */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtime = time + increment;
    evptr->evtype = TIMER_INTERRUPT;
    evptr->eventity = AorB;
    insertevent(evptr);
}

/************************** TOLAYER3 ***************/
void tolayer1(int AorB, struct frm packet)
{
    struct frm *mypktptr;
    struct event *evptr, *q;
    float lastime, x;
    int i;

    ntolayer1++;

    /* simulate losses: */
    if (jimsrand() < lossprob)
    {
        nlost++;
        if (TRACE > 0)
            printf("          TOLAYER1: frame being lost\n");
        return;
    }

    /* make a copy of the packet student just gave me since he/she may decide */
    /* to do something with the packet after we return back to him/her */
    mypktptr = (struct frm *)malloc(sizeof(struct frm));
    mypktptr->type = packet.type;
    mypktptr->seqnum = packet.seqnum;
    mypktptr->acknum = packet.acknum;
    mypktptr->checksum = packet.checksum;
    for (i = 0; i < payload_size; i++)
        mypktptr->payload[i] = packet.payload[i];
    if (TRACE > 2)
    {
        printf("          TOLAYER1: seq: %d, ack %d, check: %d ", mypktptr->seqnum,
               mypktptr->acknum, mypktptr->checksum);
        for (i = 0; i < payload_size; i++)
            printf("%c", mypktptr->payload[i]);
        printf("\n");
    }

    /* create future event for arrival of packet at the other side */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtype = FROM_LAYER1;      /* packet will pop out from layer3 */
    evptr->eventity = (AorB + 1) % 2; /* event occurs at other entity */
    evptr->pktptr = mypktptr;         /* save ptr to my copy of packet */
    /* finally, compute the arrival time of packet at the other end.
       medium can not reorder, so make sure packet arrives between 1 and 10
       time units after the latest arrival time of packets
       currently in the medium on their way to the destination */
    lastime = time;
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == FROM_LAYER1 && q->eventity == evptr->eventity))
            lastime = q->evtime;
    evptr->evtime = lastime + 1 + 9 * jimsrand();

    /* simulate corruption: */
    if (jimsrand() < corruptprob)
    {
        ncorrupt++;
        if ((x = jimsrand()) < .75)
            mypktptr->payload[0] = 'Z'; /* corrupt payload */
        else if (x < .875)
            mypktptr->seqnum = 999999;
        else
            mypktptr->acknum = 999999;
        if (TRACE > 0)
            printf("          TOLAYER1: frame being corrupted\n");
    }

    if (TRACE > 2)
        printf("          TOLAYER1: scheduling arrival on other side\n");
    insertevent(evptr);
}

void tolayer3(int AorB, char datasent[payload_size])
{
    int i;
    if (TRACE > 2)
    {
        printf("          TOLAYER3: data received: ");
        for (i = 0; i < payload_size; i++)
            printf("%c", datasent[i]);
        printf("\n");
    }
}


int checksum(struct frm frame)
{


	int returnValue=0;
	for (int i = 0; i <payload_size; i++) {
		returnValue += frame.payload[i];
	}

	returnValue=returnValue+frame.seqnum+frame.acknum+frame.type;


	return returnValue;


}