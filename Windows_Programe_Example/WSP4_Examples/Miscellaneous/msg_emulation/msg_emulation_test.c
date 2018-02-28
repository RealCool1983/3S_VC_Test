/* msg_emulation_test.c								*/

/*	Test the UNIX msg emulation package, which is designed			*/
/*	to operate on both Win32 and OpenVMS (using Pthreads)			*/

/*  Usage: msg_emulation_test np nc goal  				*/

/* start up "np" producer and nc consumer threads.	*/
/* One queue is created for each producer, keyed by the producer number */
/* Either nc or np (but not both) can be zero. A typical testing technique	*/
/* is to bring up one process (running this program) with 0 consumers	*/
/* and, once it is started, bring up another process with 0 producers	*/
/* and a number of consumers <= the the number of producers in the other */
/* process. This exercises the IPC aspects of the message Q emulation	*/
/* Each producer must produce a total of			*/
/* "goal" messages, where each message is tagged		*/
/* with the consumer that should receive it	(this is the message type)		*/

#ifdef _WINDOWS
#include "thread_emulation.h"
#else
#include "pthread.h"
#endif

#include "msg_emulation.h"

#include <string.h>
#include "errors.h"
#include "utility.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#define DELAY_COUNT 1000
#define ONEKB 1024
#define MAX_THREADS 1024
#define DATA_SIZE 256			

#define MSGQ_KEY 0x0fad   /* Arbitrary */

THREAD_FUNCTION producer (void *);
THREAD_FUNCTION consumer (void *);

typedef struct _THARG {
	volatile int thread_number;
	volatile int np;			/* Total number of producer threads */
	volatile int nc;			/* Total number of consumer threads */
	volatile int work_goal;    /* used by producers */
	volatile int work_done;    /* Used by producers and consumers */
	char waste [8]; /* Assure there is no quadword overlap */
} THARG;

/* Basic message as generated by producer and received by consumer */
typedef struct msg_block_tag { /* Message block */
	volatile unsigned long int message_type;
	volatile unsigned long int checksum; /* Message contents checksum	*/
	volatile unsigned long int source; /* Creating producer identity 	*/
	volatile unsigned long int destination;/* Identity of receiving thread*/
	volatile unsigned long int sequence; /* Message block sequence number	*/
	time_t timestamp;
	volatile unsigned long int data[DATA_SIZE]; /* Message Contents		*/
} msg_block_t, *pmsg_block_t;


/* Definitions of a general bounded queue structure. Queues are */
/* implemented as arrays with indices to youngest and oldest 	*/
/* messages, with wrap around. 					*/
/* Each queue also contains a guard mutex and			*/
/* "not empty" and "not full" condition variables.		*/
/* Finally, there is a pointer to an array of messages of	*/
/* arbitrary type						*/

void message_fill (msg_block_t *, long int, long int, long int);
void message_display (msg_block_t *);

static volatile int ShutDown = 0;
static volatile int DebugLevel = 0;
static long int GoodCount = 0, BadCount = 0;

int main (int argc, char * argv[])
{
	int tstatus, ithread, goal, np, nc, imq, ThId;
	pthread_t *producer_th, *consumer_th;
	THARG *producer_arg, *consumer_arg;
	void *pNull = NULL;
	MSQID_DS msgQDs;
	
	if (argc < 4) {
		printf ("Usage: ThreeStage np nc goal \n");
		return 1;
	}

	if ((np = atoi (argv[1])) < 0)
	   np = 0;
	if ((nc = atoi (argv[2])) < 0)
	   nc = 0;

	if ((np <= 0) && (nc <= 0)) {
		printf ("Cannot have both np and nc be 0\n");
		return 2;
	}

	srand ((int)time(NULL));	/* Seed the RN generator */
	
	if (np > MAX_THREADS) {
		printf ("Maximum number of producers or consumers is %d.\n", MAX_THREADS);
		return 2;
	}

	goal = atoi(argv[3]);
	if (argc >=5) DebugLevel = atoi(argv[4]);

	if (np > 0) {
		producer_th = malloc (np * sizeof(pthread_t));
		producer_arg = calloc (np, sizeof (THARG));
		if (producer_th == NULL || producer_arg == NULL)
			errno_abort ("Cannot allocate working memory for threads.");
	}
	if (nc > 0) {
		consumer_th = malloc (nc * sizeof(pthread_t));
		consumer_arg = calloc (nc, sizeof (THARG));
		if (consumer_th == NULL || consumer_arg == NULL)
			errno_abort ("Cannot allocate working memory for threads.");
	}

	/*	Create a single shared message Q keyed by: 0x0DAC9601 */
	/*  It may already have been created, simply open it */
	imq = msgget (  MSGQ_KEY, ( IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR ) );
	if (imq < 0) imq = msgget (  MSGQ_KEY, 0 );
	if (imq < 0) errno_abort ("Cannot get valid MQ.");
	
	/* Get the message queue size, set it, and get it again */
	if ( msgctl (imq, IPC_STAT, &msgQDs) != 0)
		errno_abort ("Cannot get MQ size.");

 	if (DebugLevel >= 2)
		fprintf( stdout,"qbytes=%x, owner(%d,%d) creator(%d,%d) mode(%o)\n"
 			, msgQDs.msg_qbytes, msgQDs.msg_perm.uid
 			, msgQDs.msg_perm.gid , msgQDs.msg_perm.cuid
 			, msgQDs.msg_perm.cgid, msgQDs.msg_perm.mode ) ;
 	fflush(stdout) ; 
/*  Set the overall size of the memory allocated to MQ. The current implementation
    allows 1MB. This call reduces it to 60K, potentially impacting performance. */
	msgQDs.msg_qbytes = 60 * ONEKB ; 
	if ( msgctl (imq, IPC_SET, &msgQDs) != 0)
		errno_abort ("Cannot set MQ size.");

	if ( msgctl (imq, IPC_STAT, &msgQDs) != 0)
		errno_abort ("Cannot get MQ size.");

 	if (DebugLevel >= 2)
		fprintf( stdout,"qbytes=%x, owner(%d,%d) creator(%d,%d) mode(%X)\n"
 			, msgQDs.msg_qbytes, msgQDs.msg_perm.uid
 			, msgQDs.msg_perm.gid , msgQDs.msg_perm.cuid
 			, msgQDs.msg_perm.cgid, msgQDs.msg_perm.mode ) ;
 	fflush(stdout) ; 
 

	/*  Create all the producers  */
	for (ithread = 0; ithread < np; ithread++) {

		/*	Create a producer */
		producer_arg[ithread].thread_number = ithread;
		producer_arg[ithread].work_goal = goal;
		producer_arg[ithread].work_done = 0;
		producer_arg[ithread].np = np;
		producer_arg[ithread].nc = nc;

		tstatus = pthread_create (producer_th[ithread], NULL, producer, &producer_arg[ithread]);
		if (tstatus != 0) 
			err_abort (tstatus, "Cannot create producer thread");
	}
	if (np > 0) 
		printf ("BOSS: %d producer threads are now running producing messages of size %d\n", 
				np, sizeof(msg_block_t));

	for (ithread = 0; ithread < nc; ithread++) {

		/*  Create a consumer */
		consumer_arg[ithread].thread_number = ithread;
		consumer_arg[ithread].work_goal = goal;
		consumer_arg[ithread].work_done = 0;
		consumer_arg[ithread].np = np;
		consumer_arg[ithread].nc = nc;

		tstatus = pthread_create (consumer_th[ithread], NULL, consumer, &consumer_arg[ithread]);
		if (tstatus != 0) 
			err_abort (tstatus, "Cannot create consumer thread");
	}
	if (nc > 0) printf ("BOSS: %d consumer threads are now running\n", nc);

	printf ("BOSS: All producer & consumer threads are running\n");	

	/* Wait for the producers to complete */
	for (ithread = 0; ithread < np; ithread++) {
		tstatus = pthread_join (producer_th[ithread]); 
		if (tstatus != 0) 
			err_abort (tstatus, "Cannot join producer thread");
		if (DebugLevel >= 1) printf ("BOSS: Producer %3d produced %4d work units\n",
								ithread, producer_arg[ithread].work_done);
	}
	/* Producers have completed their work. */
	if (np > 0) printf ("BOSS: All producers have completed their work.\n");

	/* Wait for the consumers to complete */
	for (ithread = 0; ithread < nc; ithread++) {
		tstatus = pthread_join (consumer_th[ithread]);
		if (tstatus != 0) 
			err_abort (tstatus, "Cannot join consumer thread");
		if (DebugLevel >= 1) printf ("BOSS: consumer %3d consumed %4d work units\n",
					ithread, consumer_arg[ithread].work_done);
	}
	if (DebugLevel >= 1) if (nc > 0) printf ("BOSS: All consumers have completed their work.\n");	

	if (np > 0) { free (producer_th); free (producer_arg); }
	if (nc > 0) { free (consumer_th); free (consumer_arg); }

	/* The consumer can remove the message queue */
	if (np == 0 && nc > 0) {
		printf ("Consumer about to remove the message queue number %d\n", msgget (MSGQ_KEY, 0));
		if (msgctl ( msgget (MSGQ_KEY, 0), IPC_RMID, 0 ) != 0)
			printf ("Failure removing message queue with key %x and id %d\n",
				MSGQ_KEY, msgget (MSGQ_KEY, 0));
	}

	printf ("System has finished. GoodCount = %d. Bad count = %d. Shutting down\n",
		GoodCount, BadCount);

	return 0;

}


THREAD_FUNCTION producer (void *arg)
{
	THARG * parg;
	long int ithread, tstatus, mq;
	msg_block_t msg;
	
	parg = (THARG *)arg;	
	ithread = parg->thread_number;
	srand ((int)time(NULL)+ithread);	/* Seed the RN generator uniquely for each thread */

	/* Get the number of the existing message Q */
	mq = msgget (MSGQ_KEY, 0);
	if (mq < 0) {
		printf ("Failure in producer #: %d\n", ithread);
		err_abort (mq, "Producer cannot find msg_q");
	}

	/* Periodically produce work units until the goal is satisfied */
	/* messages receive a source and destination address (namely,
	   the thread number plus 1), encoded as the message type, which are */
	/* the same in this case but could, in general, be different. */
	/* Without the random delay, messages are produced as fast as possible */
	
	while (parg->work_done < parg->work_goal) { 

		/* delay_cpu (DELAY_COUNT * rand() / RAND_MAX); */ 
		message_fill (&msg, ithread+1, ithread+1, parg->work_done);
		tstatus = msgsnd (mq, &msg, sizeof(msg) - sizeof(unsigned long int), 0);
		if (tstatus != 0) err_abort (tstatus, "Error: Prodcuer sending message");
		parg->work_done++;		
/*		sched_yield(); */
	}

	return 0;		
}

#define CONSUMER_FREQUENCY 2

THREAD_FUNCTION consumer (void *arg)
{
	THARG * carg;
	long int ithread, tstatus, mq;
	msg_block_t msg;
	int curseq, lastseq = -1;
	
	carg = (THARG *)arg;	
	ithread = carg->thread_number;
	srand ((int)time(NULL)+ithread+carg->np);	/* Seed the RN generator uniquely for each thread */


	/* Get the number of the existing message Q */
	mq = msgget (MSGQ_KEY, 0);
	if (mq < 0) {
		printf ("Failure in consumer #: %d\n", ithread);
		err_abort (mq, "Consumer cannot find msg_q");
	}

	/* Receive and display messages */
	/* Only receive messages targeted to this thread, 
		as specified by the message type */
	while (carg->work_done < carg->work_goal) { 
		tstatus = msgrcv (mq, &msg, sizeof(msg) - sizeof(unsigned long int), ithread+1, 0);
		if (tstatus <= 0) err_abort (tstatus, "Error: Consumer receiving message");
		if (DebugLevel >= 2) printf ("\nMessage received by consumer #: %3d.\n", ithread);
		message_display (&msg);

		/* Diagnostic: Check for messages in order */
		curseq = msg.sequence;
		if (curseq != lastseq + 1) printf ("**Sequence error in thread %3d. Last: %5d. Cur: %5d\n",
									ithread, lastseq, curseq);
		lastseq = curseq;
		/* End of diagnostic test */
		
		if (curseq % CONSUMER_FREQUENCY == 0) printf ("**Consumer %3d received message # %5d\n",
				ithread, curseq);
		carg->work_done++;		
/*		sched_yield(); */
	}

	/* Destroy the queue once the Q's receiving end terminates */
	return 0;		
}


unsigned int compute_checksum (void * msg, unsigned long int length)
{
	/* Computer an xor checksum on the entire message of "length"
	 * long integers */
	unsigned long int i, cs = 0, *pint;

	pint = (unsigned  long int *) msg;
	for (i = 0; i < length; i++) {
		cs = (cs ^ *pint);
		pint++;
	}
	return cs;		
}


void message_fill (msg_block_t *mblock, long int src, long int dest, long int seqno)
{
	/* Fill the message buffer, and include checksum and timestamp	*/
	/* This function is called from the producer thread while it 	*/
	/* owns the message block mutex					*/

	long int i;
	
	mblock->checksum = 0;	
	for (i = 0; i < DATA_SIZE; i++) {
		mblock->data[i] = rand()*rand();
	}

	mblock->message_type = src;	/* Message type is the id of sending thread */
	mblock->source = src;
	mblock->destination = dest;
	mblock->sequence = seqno;
	mblock->timestamp = time(NULL);
	mblock->checksum = compute_checksum (mblock, sizeof(msg_block_t)/sizeof(long int));

	if (DebugLevel >= 3) printf ("Generated message: %d %d %d %d %x %x\n", 
		src, dest, seqno, mblock->timestamp, 
		mblock->data[0], mblock->data[DATA_SIZE-1]);  

	return;
}

#ifndef _WINDOWS
/*  Non Windows InterlockedIncrement */
/*  NOT thread safe, but, at least, a non-zero count is indicative */
long int InterlockedIncrement (long int * pCount)
{
	*pCount += 1;
	return *pCount;
}
#endif

void message_display (msg_block_t *mblock)
{
	/* Display message buffer and timestamp, validate checksum	*/
	/* This function is called from the consumer thread while it 	*/
	/* owns the message block mutex					*/
	unsigned int tcheck = 0;
	
	/* In verifying the checksum, remember that the checksum field was 0 and then */
	/* became the value of the checksum of all the other fields, so the resulting */
	/* checksum should now be zero.													*/

	tcheck = compute_checksum (mblock, sizeof(msg_block_t)/sizeof(long int));

	if (DebugLevel >= 3) {
		printf ("\nReceived message number %d generated at: %s", 
			mblock->sequence, ctime (&(mblock->timestamp)));  
		printf ("Source and destination: %d %d\n", 
			mblock->source, mblock->destination); 
		printf ("First and last entries: %x %x\n",
			mblock->data[0], mblock->data[DATA_SIZE-1]); 
	}
	
	if (tcheck == 0) {
		if (DebugLevel >= 2) printf ("GOOD ->Checksum was validated.\n");
		InterlockedIncrement (&GoodCount);
	} else {
		if (DebugLevel >= 2) printf ("BAD  ->Checksum failed. message was corrupted\n");
		printf ("BAD.  Seq: %5d\n", mblock->sequence);
		InterlockedIncrement (&BadCount);
	}

	return;

}
