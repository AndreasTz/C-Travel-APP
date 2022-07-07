#include "skipList.h"

/* Epistrefei th dieythynsh neas skip list, null se periptwsh apotyxias */
skipList* createSkipList(int givenMaxLevel){
	
	int i=0;
	
	/* Deikths epistrofhs */
	skipList* newSkipListPtr;
	
	if(givenMaxLevel<=0)
	{
		printf("Max level error\n");
		return NULL;
	}
	
	/* Desmeysh mnhmhs */
	newSkipListPtr = (skipList*)malloc(sizeof(skipList));	
	if(newSkipListPtr==NULL)
	{
		printf("New skip list memory error\n");
		return NULL;	
	}
	
	/* Ti8etai to ypshlotero epipedo */
	newSkipListPtr->MaxLevel=givenMaxLevel;
	
	/* Kataskeyazetai o prwrtos komvos */
	newSkipListPtr->headerPtr=(skipListNode*)malloc(sizeof(skipListNode));
	
	if(newSkipListPtr->headerPtr==NULL)
	{
		
		printf("New skip list header memory error\n");
		free(newSkipListPtr);
		
		return NULL;	
	}
	
	/* Desmeyetai mnhmh gia toys deiktes ths kefalhs */
	newSkipListPtr->headerPtr->addressOfForwardPtr=(skipListNode**)malloc(sizeof(skipListNode*)*givenMaxLevel);
	if(newSkipListPtr->headerPtr->addressOfForwardPtr==NULL)
	{
		
		printf("New skip list header forward pointer memory error\n");
		
		free(newSkipListPtr->headerPtr);
		free(newSkipListPtr);
		
		return NULL;	
	}

	
	/* Oi deiktes ths kefalhs ti8entai NULL */
	for(i=0;i<givenMaxLevel;i++)
		*((newSkipListPtr->headerPtr->addressOfForwardPtr)+i)=NULL;
		
	/* Epistrefetai h dieythynsh ths skip list */
	return newSkipListPtr;
		
}

/* Efoson eisaxthei neos komvos epistefei th dieythynsh toy eisagomenoy komvoy kai thetei th shmaia exists sto 0 */
/* Ean o komvos yparxei hdh epistefei th dieythynsh toy kai thetei th shmaia exists sto 1 */
skipListNode* insert(skipList* givenList,int citizenID, int* exists){
	
	skipListNode *previousNode, *newNode,*existingNode;
	int i,height;
		
	/* Ean h lista einai kenh episterfetai NULL */
	if(givenList==NULL)
	{
		printf("Empty List\n");
		return NULL;	
	}

	/* H shmaia evreshs arxikopoieitai sto mhden */
	*exists=0;
	
	/* Eksetazetai an yparxei hdh o komvos */
	existingNode=searchNode(givenList,citizenID);
	
	
	/* An nai h shmaia exists tithetai 1 kai epistrefetai o komvos */
	if(existingNode)
	{
		*exists=1;
		return existingNode;
				
	}
	
	/* Dhmioyrgia komvoy */
	newNode=(skipListNode*)malloc(sizeof(skipListNode));

	if(newNode==NULL)
	{
		printf("New node error\n");
		return NULL;	
	}

	/* Tyxaia epilogh ypsoys */
	height=rand()%givenList->MaxLevel+1;
	
	/* Desmeysh mnhmhs gia deiktes prowthhshs */
	newNode->addressOfForwardPtr=(skipListNode**)malloc(sizeof(skipListNode*)*(givenList->MaxLevel));
	
	if(newNode->addressOfForwardPtr==NULL)
	{
		
		printf("New forward pointer memory error\n");
		free(newNode->addressOfForwardPtr);
		free(newNode);
		
		return NULL;	
	}

	newNode->citizenID=citizenID;
		
	for(i=givenList->MaxLevel-1;i>height-1;i--)
		*(newNode->addressOfForwardPtr+i)=NULL;
	
	/* Evresh prohgoymenoy komvoy */
	
	/* Arxikopoihsh sthn kefalh */
	previousNode = givenList->headerPtr;
	
	/* Arithmodeikths ypsoys */
	i=givenList->MaxLevel-1;

	/* Vroxos mexri na katevoyme sth vash ths listas */
	while(i!=0)
	{
		
		/* An o epomenos komvos einai NULL
		prepei na katevoyme epipedo, alla prin ginei ayto prepei na alegxthei an prepei na
		ginei syndesh me ton neo komvo */
		if((*(previousNode->addressOfForwardPtr+i))==NULL)
		{
			
			/* Syndesh an einai aparaithto */
			if(i<=height-1)
			{
				*(newNode->addressOfForwardPtr+i)=*(previousNode->addressOfForwardPtr+i);
				*(previousNode->addressOfForwardPtr+i)=newNode;	
			}	
			/* Katevasma epipedoy */
			i--;			
		}
		/* An to trexon kleidi einai megalytero apo ayto toy komvoy poy tha eisaxthei
		prepei na katevoyme epipedo, alla prin ginei ayto prepei na alegxthei an prepei na
		ginei syndesh me ton neo komvo */
		else if((*(previousNode->addressOfForwardPtr+i))->citizenID>citizenID)
		{
			
			/* Syndesh an einai aparaithto */
			if(i<=height-1)
			{
				*(newNode->addressOfForwardPtr+i)=*(previousNode->addressOfForwardPtr+i);
				*(previousNode->addressOfForwardPtr+i)=newNode;	
			}	
			/* Katevasma epipedoy */
			i--;			
		}
		else
		{
			/* Se diaforetikh periptwsh kinoymaste deksia */
			previousNode=*(previousNode->addressOfForwardPtr+i);
		}
	}

	/* Peretairw kinh pros ta deksia efoson exoyme ftash sth bash prokeimenoy na ftasoyme sth swsth thesh */
	while((*(previousNode->addressOfForwardPtr))!=NULL)
	{
		if((*(previousNode->addressOfForwardPtr))->citizenID<citizenID)
			previousNode=*(previousNode->addressOfForwardPtr);
		else
			break;
	}
	
	/* Apokatastash deiktwn syndeshs */
	*(newNode->addressOfForwardPtr)=*(previousNode->addressOfForwardPtr);
	*(previousNode->addressOfForwardPtr)=newNode;	
	
	/* Epistrofh neoy komvoy */
	return newNode;
	
}


/* Epistrofh toysxetikoy komvoy se epityxia NULL se apotyxia */
skipListNode* searchNode(skipList* givenList, int citizenID)
{
	
	skipListNode *previousNode;
	int i;
		
	/* Ean h lista einai kenh epistrefetai NULL */
	if(givenList==NULL){

		return NULL;	
	}

	/* An einai adeia epishs epistrefetai NULL */
	if(givenList->headerPtr==NULL)
	{
		return NULL;	
	}
	
	if(*(givenList->headerPtr->addressOfForwardPtr)==NULL)
	{
		return NULL;	
	}


	/* Apo ekei kai pera h synarthsh ergazetai paromoia me thn insert */
	previousNode = givenList->headerPtr;

	/* Katevasma sto xamhlotero epiepdo */
	i=givenList->MaxLevel-1;
	while(i!=0)
	{

		if((*(previousNode->addressOfForwardPtr+i))==NULL)
		{
			i--;			
		}
		else if((*(previousNode->addressOfForwardPtr+i))->citizenID>citizenID)
		{
			i--;			
		}
		else if((*(previousNode->addressOfForwardPtr+i))->citizenID==citizenID)
		{
			return (*(previousNode->addressOfForwardPtr+i));
		}
		else
		{
			previousNode=*(previousNode->addressOfForwardPtr+i);
		}
	}

	/* Anazhthhsh sto teleytaio epipedo */
	while((*(previousNode->addressOfForwardPtr))!=NULL)
	{
		
		/* An vrethei o komvos epistefetai */
		if((*(previousNode->addressOfForwardPtr))->citizenID==citizenID)
		{
			return (*(previousNode->addressOfForwardPtr));
		}
		/* An prosperastei h timh toy epistefetai NULL */
		else if((*(previousNode->addressOfForwardPtr))->citizenID<citizenID)
		{
			return NULL;
		}
		/* Diaforetika proxwrame ston epomeno */
		else
			previousNode=*(previousNode->addressOfForwardPtr);
	}
	
	return NULL;
	
}


/* Epistrefei ton komvo poy diagrafetai diaforetika NULL */
skipListNode* delete(skipList* givenList, int citizenID){
	
	skipListNode *previousNode, *delNode;
	int i;
	
	/* Anazhthsh komvoy pros diagrafh */
	delNode=searchNode(givenList,citizenID);
	
	/* An den brethike o komvos epistrefetai NULL */
	if(delNode==NULL)
	{
		return NULL;	
	}
	
	/*  Diaforetika anazhthsh toy prohgoymenoy */
	previousNode = givenList->headerPtr;
	
	i=givenList->MaxLevel-1;
	
	while(i!=0)
	{
			
		/* An o trexwn deikths deixnei se megalytero kleidi katebainoyme epipedo */
		if((*(previousNode->addressOfForwardPtr+i))->citizenID>citizenID)
		{			
			i--;			
		}
		/* Diaforetika an o deikths deixei ton komvo poy theloyme apokathistoyme th syndesh
		kai katebainoyme pali */
		else if(*(previousNode->addressOfForwardPtr+i)==delNode)
		{
			
			*(previousNode->addressOfForwardPtr+i)=*(delNode->addressOfForwardPtr+i);
			
			i--;
		
		}
		else
		{
			/* Diaforetika kinoymaste pros ta deksia */
			previousNode=*(previousNode->addressOfForwardPtr+i);
		}
	}
	
	/* Ean xreiastei kai pali kinoymaste sta deksia exontas ftasei sth vash */
	while((*(previousNode->addressOfForwardPtr))!=NULL){
		
		/* An vrethei o komvos spaei to loop */
		if((*(previousNode->addressOfForwardPtr))->citizenID==citizenID)
		{
			break;
		}
		/* Diaforetika proxwrame ston epomeno */
		else
			previousNode=*(previousNode->addressOfForwardPtr);
	}
	
	/* Apokatastash syndeshs */	
	*(previousNode->addressOfForwardPtr)=*(delNode->addressOfForwardPtr);
	
	/* Epistrofh komvoy */
	return delNode;
	
}


/* Diagrafh oloklhsrhs listas. */
void deleteSkipList(skipList** addressOfGivenList)
{
	
	int delCitizenID;
	
	skipListNode *delNode;
	skipListNode *prevNode;
	
	/* Periptwsh adeias listas */
	if(addressOfGivenList==NULL)
	{
		return ;	
	}
	if(*addressOfGivenList==NULL)
	{
		return ;	
	}
	
	/* Diaforetika diagrafontai oloi oi komvoi */
	
	
	delNode=*((*addressOfGivenList)->headerPtr->addressOfForwardPtr);
	
	while(delNode!=NULL){
		
		prevNode=delNode;
		
		delNode=*(delNode->addressOfForwardPtr);
		
		if(prevNode->date!=NULL)
			free(prevNode->date);
		
		free(prevNode);
		
	}
	
	
	
	/* Diagrafh kefalhs */
	free((*addressOfGivenList)->headerPtr->addressOfForwardPtr);
	free(*addressOfGivenList);
	
	/* Diagrafh skip list */
	*addressOfGivenList=NULL;

}

/* Epistrofh toy prwtoy komvoy */
skipListNode* getFirstNode(skipList* givenList)
{
	
	if(givenList==NULL)
		return NULL;
	
	
	if(givenList->headerPtr==NULL)
		return NULL;
	
	return *(givenList->headerPtr->addressOfForwardPtr);
	
	
}

/* Epistrofh toy epomenoy komvoy */
skipListNode* getNextNode(skipListNode* givenNode)
{
	
	if(givenNode==NULL)
		return NULL;
	
	return *(givenNode->addressOfForwardPtr);
	
	
}
		
