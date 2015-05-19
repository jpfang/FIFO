#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h> /* for open */
#include <unistd.h> /* for close */
#include <errno.h>
#include <sys/errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>


#define MAXBUFFSIZE 1024
#define FIFONAME "MyFIFO"

void *ReadFIFO(void)
{
	int iFIFO;	
	char acFIFO_Name[MAXBUFFSIZE] = FIFONAME;
	char acMessageBuffer[MAXBUFFSIZE];

	fd_set FDList;
	fd_set ReadFD;

	struct timeval tTimeOut;

	int iMaxFDNumber, iFDIndex;

	/* Clear FD */
	FD_ZERO(&FDList);
	FD_ZERO(&ReadFD);

	/* Open the FIFO */
	if ((iFIFO = open(acFIFO_Name, O_RDONLY)) < 0)
	{
		fprintf(stderr, "Failed to open FIFO\n");
	}

	/* Setting fd to FDList */
	FD_SET(iFIFO, &FDList);

	/* Trace Max number of fd */
	iMaxFDNumber = iFIFO;

	while (1)
	{
		/* Setting time out information */
		tTimeOut.tv_sec  = 10;
		tTimeOut.tv_usec = 0;

		ReadFD = FDList; /* Copy it */

		fprintf(stdout, "Select\n");

		/* select */
		switch (select(iMaxFDNumber + 1, &ReadFD, NULL, NULL, &tTimeOut))
		{
			case -1:
				fprintf(stdout, "Failed to select FD\n");
				break;

			case  0:
				fprintf(stdout, "Time out\n");
				break;

			default:
				/* Check every one fd */
				for (iFDIndex = 0; iFDIndex < (iMaxFDNumber + 1); iFDIndex++)
				{
					/* Check iFDIndex was set? */
					if (FD_ISSET(iFDIndex, &ReadFD))
					{
						if (iFDIndex == iFIFO)
						{
							if(read(iFIFO, acMessageBuffer, MAXBUFFSIZE) < 0)
							{
								fprintf(stderr, "Failed to read FIFO\n");
							}

							fprintf(stdout, "*****Received in thread:*****\n%s*****************************\n",acMessageBuffer);
						}
					}
				}
				break;
		}
	}
	
	close(iFIFO);
}

int main(int argc, char *argv[])
{
	int iFIFO; /* The fd of fifo */
	int iMessageLen; /* Message Lenth */
	
	/* FIFO name */
	char acFIFO_Name[MAXBUFFSIZE] = FIFONAME;
	
	/* Message buffer */
	char acMessageBuffer[MAXBUFFSIZE];
	pthread_t tThread_ReadFIFO; /* thread of ReadFIFO function */

	/* Create the iFIFO (ignore the error if it already exists). */
	if (mkfifo(acFIFO_Name, 0666) < 0)
	{
		if (errno != EEXIST)
		{
			fprintf(stderr ,"Failed to create FIFO\n");
			return -1;
		}
	}

	/* open the FIFO */
	iFIFO = open(acFIFO_Name, O_RDWR);

	if (iFIFO < 0)
	{
		fprintf(stderr, "Failed to open FIFO");
		return -1;
	}

	fprintf(stdout, "Main thread writing some message\n");

	/* Create th e ReadFIFO function thread */
	pthread_create(&tThread_ReadFIFO, NULL, (void *)ReadFIFO, NULL);

	/* Until you don't want to say anymore */
	while (1)
	{
		/* Key in */
		if (fgets(acMessageBuffer, sizeof(acMessageBuffer), stdin) == NULL)
		{
			fprintf(stderr, "Failed to get key in string\n");
		}

		iMessageLen = strlen(acMessageBuffer);

		/* Because write function, only write strlen - 1 */
		/* We need force the write include '\0', so strlen + 1 */
		if (write(iFIFO, acMessageBuffer, iMessageLen + 1) < 0)
		{
			fprintf(stderr, "Failed to write message to FIFO\n");
			return -1;
		}

		fprintf(stdout, "Writing Done!\n");
	}

	close(iFIFO);

	pthread_join(tThread_ReadFIFO, NULL);

	return 0;
}

