#ifndef _SKIP_LIST_
#define _SKIP_LIST_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define SKIP_LIST_HEIGHT 8

typedef struct skipListNode{
	
	/* Id atopoy */
	int citizenID;
	
	/* Hmeromhnia emvoliasmoy (pithanon NULL)*/
	char* date;
	
	/* Deikths se citizenRecord*/
	void* citizenRecordPtr;

	/* Deiktes stoys epomenoys */
	struct skipListNode** addressOfForwardPtr;
	
}skipListNode;

typedef struct skipList{
	
	/* Megisto ypsos listas*/
	int MaxLevel;
	
	/* Deikths ston komvo kefalh */
	skipListNode* headerPtr;
	
}skipList;

/* Epistrefei th dieythynsh neas skip list, null se periptwsh apotyxias */
skipList* createSkipList(int givenMaxLevel);

/* Efoson eisaxthei neos komvos epistefei th dieythynsh toy eisagomenoy komvoy kai thetei th shmaia exists sto 0 */
/* Ean o komvos yparxei hdh epistefei th dieythynsh toy kai thetei th shmaia exists sto 1 */
skipListNode* insert(skipList* givenList,int citizenID, int* exists);

/* Epistrofh toysxetikoy komvoy se epityxia NULL se apotyxia */
skipListNode* searchNode(skipList* givenList, int citizenID);

/* Epistrefei ton komvo poy diagrafetai diaforetika NULL */
skipListNode* delete(skipList* givenList, int citizenID);

/* Diagrafh oloklhsrhs listas. */
void deleteSkipList(skipList** addressOfGivenList);

/* Epistrofh toy prwtoy komvoy */
skipListNode* getFirstNode(skipList* givenList);

/* Epistrofh toy epomenoy komvoy */
skipListNode* getNextNode(skipListNode* givenNode);

#endif




