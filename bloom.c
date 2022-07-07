#include "bloom.h"

/* Dedomenh synarthsh hash */
unsigned long hash_i(unsigned char *str, unsigned int i);

/* Dhmioyrgia filtroy bloom */
char* BLOOM_create(unsigned long bloomSize)
{
	
	/* Dhmioyrgia bloom */
	char* bloom = malloc(bloomSize);
	
	/* Ola ta bytes toy bloom ginontai reset */
	memset(bloom,0,bloomSize);

	/* To filtor epistrefetai */
	return bloom;
	
}

/* Prosthhkh kleidioy sto filtro bloom */
void BLOOM_add_key(char* bloom,unsigned long bloomSize,int key)
{
	/* By default ginetai hash symvoloseira mhkoys KEY_SIZE */
	char bloomInput[KEY_SIZE];
	/* Apotelesma hash */
	unsigned long hashResult;
	
	/* To byte pou psaxnoyme */
	unsigned long numOfByte;
	
	/* To bit pou psaxnoyme mesa sto byte*/
	unsigned long numOfBit;
	
	/* To byte to opoio sthn oysia kanei eisagwgh timhs sto bloom */
	unsigned char byteInsertor;
	
	/* H mnhmh ginetai reset */
	memset(bloomInput,0,KEY_SIZE);
	
	/* Grafetai se aythn to kleidi ws string */
	sprintf(bloomInput,"%d",key);
	
	
	int j;
	
	for(j=0;j<16;j++)
	{		
		/* Ypologizetai h timh hash */				
		hashResult=(hash_i(bloomInput,j))%(8*bloomSize);
		
		/* Ypologizetai to byte toy bloom */
		numOfByte=hashResult/8;
		
		/* Ypologizetai to bit toy byte toy bloom */
		numOfBit=hashResult%8;
		
		/* Analoga me thn timh toy bit, o antistoixos "eisagwgeas" pairnei analogh timh, h opoia einai dynamh toy 2 */
		if(numOfBit==0)
		{
			byteInsertor=1;					
		}
		else if(numOfBit==1)
		{
			byteInsertor=2;					
		}
		else if(numOfBit==2)
		{
			byteInsertor=4;					
		}
		else if(numOfBit==3)
		{
			byteInsertor=8;					
		}
		else if(numOfBit==4)
		{
			byteInsertor=16;					
		}
		else if(numOfBit==5)
		{
			byteInsertor=32;					
		}
		else if(numOfBit==6)
		{
			byteInsertor=64;					
		}
		else
		{
			byteInsertor=128;					
		}
			
		bloom[numOfByte]=bloom[numOfByte]|byteInsertor;
		
	}
	
}

/* Elegxei thn kataxwrish enos kleidioy sto boom. Epistrefei 0 sto "oxi" kai 1 sto "isws" */
int BLOOM_check_key(char* bloom,unsigned long bloomSize,int key)
{
	/* By default ginetai hash symvoloseira mhkoys KEY_SIZE */
	char bloomInput[KEY_SIZE];
	
	/* Apotelesma hash */
	unsigned long hashResult;
	
	/* To byte pou psaxnoyme */
	unsigned long numOfByte;
	
	/* To bit pou psaxnoyme mesa sto byte*/
	unsigned long numOfBit;
		
	/* To byte to opoio sthn oysia kanei anixneysh timhs sto bloom */
	unsigned char byteInspector;
	
	/* H mnhmh ginetai reset */
	memset(bloomInput,0,KEY_SIZE);
	
	/* Grafetai se aythn to kleidi ws string */
	sprintf(bloomInput,"%d",key);
	
	
	int j;
	
	for(j=0;j<16;j++)
	{
		
		/* Ypologizetai h timh hash */				
		hashResult=(hash_i(bloomInput,j))%(8*bloomSize);
		
		/* Ypologizetai to byte toy bloom */
		numOfByte=hashResult/8;
		
		/* Ypologizetai to bit toy byte toy bloom */
		numOfBit=hashResult%8;
		
		/* Analoga me thn timh toy bit, o antistoixos "eisagwgeas" pairnei analogh timh, h opoia einai dynamh toy 2 */
		if(numOfBit==0)
		{
			byteInspector=1;					
		}
		else if(numOfBit==1)
		{
			byteInspector=2;					
		}
		else if(numOfBit==2)
		{
			byteInspector=4;					
		}
		else if(numOfBit==3)
		{
			byteInspector=8;					
		}
		else if(numOfBit==4)
		{
			byteInspector=16;					
		}
		else if(numOfBit==5)
		{
			byteInspector=32;					
		}
		else if(numOfBit==6)
		{
			byteInspector=64;					
		}
		else
		{
			byteInspector=128;					
		}
			
		if((bloom[numOfByte]&byteInspector)!=byteInspector)
			break;
						
		
	}
	
	/* An o vroxos kataferei na termatisei bre8hkan ola ta bit. */
	if(j==16)
		return 1;
	else
		return 0;		
	
}


/* Apostolh bloom */
void BLOOM_send(int fdSend,char* buffer,int bufferSize,char* bloomSource,unsigned long bloomSize)
{
	
	int i;
	
	/* Deikths enarkshs dedomenwn apostolhs */
	char* sendPointer;
	
	/* Arithmos apostolwn */
	int numOfSends;
	
	/* Ypologizoume to plhthos twn apostolwn */
	
	numOfSends=bloomSize/bufferSize;
	
	if(bloomSize%bufferSize)
		numOfSends++;

	/* O deikths enarkshs dedomenwn apostolhs arxikopoieitai sto prwto byte to bloom*/
	sendPointer=bloomSource;
	
	/* An oi apostoles einai perissoteres apo mia tote oles ektos ths teleytaias
	xrhsimopoioyn oloklhro ton buffer */ 
	for(i=0;i<numOfSends-1;i++)
	{
				
		/* Antigrafh dedomenwn ston buffer */
		memcpy(buffer,sendPointer,bufferSize);

		/* Apostolh dedomenwn */
		write(fdSend,buffer,bufferSize);
		
		/* Enhmerwsh deikth enarkshs dedomenwn apostolhs */
		sendPointer+=bufferSize;
		
	}
	
	/* H teleytaia apostolh exei megethos pali oso o buffer an to ypoloipo bloomSize%bufferSize
	enai mhdeniko, enw einai isko me to ypoloipo, an to ypoloipo einai mh mhdeiniko */
	if(bloomSize%bufferSize)
		memcpy(buffer,sendPointer,bloomSize%bufferSize);
	else
		memcpy(buffer,sendPointer,bufferSize);

	/* Se kathe periptwsh ta dedomena apostellontai */
	write(fdSend,buffer,bufferSize);
	
}


/* Paralabh bloom */
void BLOOM_receive(int fdReceive,char* buffer,int bufferSize,char* bloomDestination,unsigned long bloomSize)
{
	
	int i;
	
	/* Deikths enarkshs dedomenwn paralabhs */
	char* receivePointer;
	
	/* Arithmos paralabwn */
	int numOfReceives;
	
	/* Ypologizoume to plhthos twn paralabwn */
	
	numOfReceives=bloomSize/bufferSize;
	
	if(bloomSize%bufferSize)
		numOfReceives++;
	
	/* O deikths enarkshs dedomenwn paralabhs arxikopoieitai sto prwto byte to bloom*/
	receivePointer=bloomDestination;
	
	/* An oi paralabes einai perissoteres apo mia tote oles ektos ths teleytaias
	xrhsimopoioyn oloklhro ton buffer */ 
	for(i=0;i<numOfReceives-1;i++)
	{
	
		/* Paralabh dedomenwn */
		read(fdReceive,buffer,bufferSize);
		
		/* Antigrafh dedomenwn ston bloom */
		memcpy(receivePointer,buffer,bufferSize);
		
		
		/* Enhmerwsh deikth enarkshs dedomenwn paralabhs */
		receivePointer+=bufferSize;
		
	}
	
	/* Paralabh dedomenwn */
	read(fdReceive,buffer,bufferSize);
	
	/* H teleytaia paralabh exei megethos pali oso o buffer an to ypoloipo bloomSize%bufferSize
	enai mhdeniko, enw einai isko me to ypoloipo, an to ypoloipo einai mh mhdeiniko */
	if(bloomSize%bufferSize)
		memcpy(receivePointer,buffer,bloomSize%bufferSize);
	else
		memcpy(receivePointer,buffer,bufferSize);


}

/* Ypologismos checksum (gia dikh mas xrhsh epivevaiwshs orthothtas apostolhs paralabhs) */
int BLOOM_check_sum(char* bloom,unsigned long bloomSize)
{
	int checkSum=0;
	
	int i;
	
	for(i=0;i<bloomSize;i++)
		checkSum+=bloom[i];
	
	return checkSum;	
	
}

/* Kwdikas poy mas epitrepsate na xrhsimopoihsoyme etoimo */

/**********************************************************************************/

// This algorithm (k=33) was first reported by dan bernstein many years 
// ago in comp.lang.c. The magic of number 33 (why it works better than
// many other constants, prime or not) has never been adequately explained.

unsigned long djb2(unsigned char *str) {
	int c;
	unsigned long hash = 5381;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; // hash * 33 + c

	return hash;
}

// This algorithm was created for sdbm (a public-domain reimplementation of ndbm) 
// database library. it was found to do well in scrambling bits, causing better 
// distribution of the keys and fewer splits. it also happens to be a good 
// general hashing function with good distribution. The actual function 
// is hash(i) = hash(i - 1) * 65599 + str[i]; what is included below 
// is the faster version used in gawk. There is even a faster, duff-device 
// version. The magic constant 65599 was picked out of thin air while experimenting 
// with different constants, and turns out to be a prime. this is one of the 
// algorithms used in berkeley db (see sleepycat) and elsewhere.

unsigned long sdbm(unsigned char *str) {
	int c;
	unsigned long hash = 0;

	while ((c = *str++))
		hash = c + (hash << 6) + (hash << 16) - hash;

	return hash;
}

// Return the result of the ith hash function. This function uses djb2 and sdbm.
// None of the functions used here is strong for cryptography purposes but they
// are good enough for the purpose of the class.
//
// The approach in this function is based on the paper:
// https://www.eecs.harvard.edu/~michaelm/postscripts/rsa2008.pdf

unsigned long hash_i(unsigned char *str, unsigned int i) {
	return djb2(str) + i * sdbm(str) + i * i;
}




















