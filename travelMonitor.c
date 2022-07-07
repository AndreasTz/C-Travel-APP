#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<signal.h>

#include "bloom.h"
#include "constants.h"

/*
	// Me SIGINT o monitor typwnei log arxeia
	signal(SIGINT,print_log_handler);
	// Me SIGQUIT o monitor typwnei log arxeia
	signal(SIGQUIT,print_log_handler);
	// Me SIGQUIT o monitor elegxei gia nea arxeia 
	signal(SIGUSR1,add_files_handler);

*/

/* Domh me ta pids kai tis xwres poy analamvanei ka8e monitor */
typedef struct monitorInfo
{
	
	/* Monitor process id */
	pid_t pid;
	
	/* Ari8modeikths poy deixnei th thesh ths prwths xwras poy analamvanei o monitor sron pinaka twn xwrwn */
	unsigned char startIndex;
	
	/* Ari8modeikths poy deixnei th thesh thsteleytaias xwras poy analamvanei o monitor sron pinaka twn xwrwn */
	unsigned char endIndex;	
	
	/* To plhthos twn iwn */
	int numOfMonitorViruses;
	
	/* O ioi */
	char** monitorViruses;
	
	/* Ta blooms */
	char** monitorBlooms;
	
}monitorInfo;



/***************** Shmantikes times ********************/

/* Plh8os monitor pou dinetai apo th grammh entolwn */
int numMonitors;

/* Pinakas xwrwn (ola ta onomata) */
char** countries;

/* Synoliko Plh8os xwrwn pou 8a moirastoun stous monitors */
int numberOfCountries;

/* Mege8os bloom filter pou dinetai apo th grammh entolwn */
unsigned long sizeOfBloom;

/* Mege8os buffer pou dinetai apo th grammh entolwn */
int bufferSize;

/* O buffer */ 
char* buffer;

/* Metrhths epityxwn aithsewn */
int totalAccepts;

/* Metrhths anepityxwn aithsewn */
int totalRejects;
			

/********** Shmantikes domes diaxeirishs ****************/

/* Pinakas file descriptor gia ta name pipes gia metafora dedomenwn apo ton parent stoys monitors */
int* parentToMonitorFds;
		
/* Pinakas file descriptor gia ta name pipes gia metafora dedomenwn apo toys monitors ston parent */
int* monitorToParentFds;

/* Pinakas domwn me ta pids kai tis xwres poy analamvanei ka8e monitor */
monitorInfo* allMonitors;


/********* Ta parapanw tha einai global**************/

/* H synarthsh dexetai th symvoloseira mias xwras kai epistrefei to id toy monitor poy einai ypey8ynos (-1) se apotyxia */
int ASS_get_monitor_id_by_country(char* countryStr);

/* H synarthsh dexetai th symvoloseira mias xwras kai epistrefei to pid toy monitor poy einai ypey8ynos (-1) se apotyxia */
int ASS_get_monitor_pid_by_country(char* countryStr);

/* Diavasma iwn apo ton buffer gia logarismo enos monitorInfo */
void ASS_get_viruses_from_buffer(int monitorId);

/**************** Alla xrhsima global *******************************/

/* Pinakas onomatwn gia ta name pipes gia metafora dedomenwn apo ton parent stoys monitors */
char** parentToMonitorNames;
		
/* Pinakas onomatwn gia ta name pipes gia metafora dedomenwn apo toys monitors ston parent */
char** monitorToParentNames;
	
/* Input directory pou dinetai apo th grammh entolwn */
char* inputDir;
		

/****************** Synarthsheis diaxeirishs shmatwn *************************/

/* Entopizei pio paidi termatise kai to antika8ista */
void SIGNAL_replace_child(int s);




int main(int argc,char* argv[])
{
	
	/* Apotelesma mias aithshs kwdikoi NOT_VACCINATED ,VACCINE_OUT_OF_TIME, VACCINATED */
	int requestResult;
	
	/* Pinakas xarakthrwn gia th grammh pou plhktrologei kathe fora o xrhsths */
	char line[256];
	
	/* Pinakas xarakthrwn gia thn entolh pou plhktrologei kathe fora o xrhsths */
	char command[256];

	/* Deikths typoy DIR gia to input directory */
	DIR* dir;
	
	/* Eggrafh input directory */
	struct dirent *entryPtr;
			
	/* Ari8modeikths poy deixnei th thesh ths prwths xwras poy analamvanei o monitor sron pinaka twn xwrwn */
	unsigned char startIndex;
	
	/* Ari8modeikths poy deixnei th thesh thsteleytaias xwras poy analamvanei o monitor sron pinaka twn xwrwn */
	unsigned char endIndex;	

	/* Bohthhtikes metavlhtes */
	int bloomSizeIndex;
	int bufferSizeIndex;
	
	/* Arithmodeiktes */
	int i,j;
		
	
	/* Metablhth gia thn fork */
	int pid;
	
	/* Metavlhth gia thn epistrofh twn monitor */
	int status;
	
	/* Id citizen poy dinei o xrhsths san string */
	char citizenIDStr[12];
	
	/* Id citizen poy dinei o xrhsths */
	int citizenID;
	
	/* Hmeromhnia poy dinei o xrhsths */
	char date[12];

	/* Hmeromhnies poy dinei o xrhsths */
	char date1[12];
	char date2[12];

	/* Xwra proeleyshs poy dinei o xrhsths */
	char countryFrom[64];

	/* Xwra proorismoy poy dinei o xrhsths */
	char countryTo[64];

	/* Onoma ioy poy dinei o xrhsths */
	char virusName[32];
		
	/* Aykswn ari8mos monitor */
	int monitorID;
	
	/* Deikths se bloom */
	char* bloom;
	
	/* Paralamvanomenos metrhths epityxwn aithsewn */
	int receivedAccepts;

	/* Paralamvanomenos metrhths anepityxwn aithsewn */
	int receivedRejects;
			
	/* Auroisma paralamvanomenwn epityxwn aithsewn */
	int sumOfReceivedAccepts;

	/* Auroisma paralamvanomenwn anepityxwn aithsewn */
	int sumOfReceivedRejects;
			
	/* Deikths arxeiou log */
	FILE* fpLogFile;
	
	/* Otan einai "1" o monitor poy dhmioyrgeitai 8a prepei na steilei pisw kai toys ioys kai ta bloom*/
	char sendStructuresBack[2];
	

	
	/* Elegxos plithous orismatwn */
	if(argc!=9)
	{
		printf("Wrong number of arguments. Accepting 8 arguments.\n");		
		exit(-1);	
	}
	
	/* Elegxos shmaias -m */
	if((strcmp(argv[1],"-m")&&strcmp(argv[3],"-m"))&&strcmp(argv[5],"-m")&&strcmp(argv[7],"-m"))
	{
		printf("Wrong position of flag -m flag -m missing.\n");
		exit(-1);	
	}
		
	/* Elegxos shmaias -b */
	if((strcmp(argv[1],"-b")&&strcmp(argv[3],"-b"))&&strcmp(argv[5],"-b")&&strcmp(argv[7],"-b"))
	{
		printf("Wrong position of flag -b flag -b missing.\n");
		exit(-1);	
	}
	
	/* Elegxos shmaias -s */
	if((strcmp(argv[1],"-s")&&strcmp(argv[3],"-s"))&&strcmp(argv[5],"-s")&&strcmp(argv[7],"-s"))
	{
		printf("Wrong position of flag -s flag -s missing.\n");
		exit(-1);	
	}
		
	/* Elegxos shmaias -i */
	if((strcmp(argv[1],"-i")&&strcmp(argv[3],"-i"))&&strcmp(argv[5],"-i")&&strcmp(argv[7],"-i"))
	{
		printf("Wrong position of flag -i flag -i missing.\n");
		exit(-1);	
	}
	
	/* Diavasma plh8oys monitor pou dinetai apo th grammh entolwn */
	if(!strcmp(argv[1],"-m"))
		sscanf(argv[2],"%d",&numMonitors);
	else if(!strcmp(argv[3],"-m"))
		sscanf(argv[4],"%d",&numMonitors);
	else if(!strcmp(argv[5],"-m"))
		sscanf(argv[6],"%d",&numMonitors);
	else
		sscanf(argv[8],"%d",&numMonitors);
	
	/* Elegxos ari8mou pou diavasthke */
	if(numMonitors<=0)
	{
		printf("Wrong number of monitors. Must be positive integer.\n");
		exit(-1);		
	}

	/* Diavasma megethous buffer monitor pou dinetai apo th grammh entolwn */
	if(!strcmp(argv[1],"-b"))
	{
		sscanf(argv[2],"%d",&bufferSize);
		bufferSizeIndex=2;
	}
	else if(!strcmp(argv[3],"-b"))
	{
		sscanf(argv[4],"%d",&bufferSize);
		bufferSizeIndex=4;
	}
	else if(!strcmp(argv[5],"-b"))
	{
		sscanf(argv[6],"%d",&bufferSize);
		bufferSizeIndex=6;
	}
	else
	{
		sscanf(argv[8],"%d",&bufferSize);
		bufferSizeIndex=8;
	}
	/* Elegxos ari8mou pou diavasthke */
	if(bufferSize<=0)
	{
		printf("Wrong size of buffer. Must be positive integer.\n");
		exit(-1);		
	}
		
	/* Diavasma megethous bloom filter pou dinetai apo th grammh entolwn */
	if(!strcmp(argv[1],"-s"))
	{
		sscanf(argv[2],"%lu",&sizeOfBloom);
		bloomSizeIndex=2;
	}
	else if(!strcmp(argv[3],"-s"))
	{
		sscanf(argv[4],"%lu",&sizeOfBloom);
		bloomSizeIndex=4;
	}
	else if(!strcmp(argv[5],"-s"))
	{
		sscanf(argv[6],"%lu",&sizeOfBloom);
		bloomSizeIndex=6;
	}
	else
	{
		sscanf(argv[8],"%lu",&sizeOfBloom);
		bloomSizeIndex=8;
	}
	
	/* Elegxos ari8mou pou diavasthke */
	if(sizeOfBloom<=0)
	{
		printf("Wrong size of bloom. Must be positive integer.\n");
		exit(-1);		
	}
	
	/* Diavasma input directory pou dinetai apo th grammh entolwn */
	if(!strcmp(argv[1],"-i"))
		inputDir=argv[2];
	else if(!strcmp(argv[3],"-i"))
		inputDir=argv[4];
	else if(!strcmp(argv[5],"-i"))
		inputDir=argv[6];
	else
		inputDir=argv[8];
	

	/* Anoigma input directory */
	dir = opendir(inputDir);
	
	/* Elegxos yparkshs input directory */
	if (dir==NULL) 
	{
		printf("Wrong input directory (can not be opened).\n");
		exit(-1);		
	}

	/* Arxikopoihsh metrhth epityxwn aithsewn */
	totalAccepts=0;

	/* Arxikopoihsh metrhth anepityxwn aithsewn */
	totalRejects=0;

	/* Arxikopoihsh metrhth xwrwn */
	numberOfCountries=0;
	
	/* Metrhsh twn xwrwn */  	
	while(1)
	{
		
		/* Diavasma epomenoy katalogoy xwras */
		entryPtr=readdir(dir);
		
		/* Ean teleiwsan oi eggrafes vgainoyme apo to loop */
		if(entryPtr==NULL)
			break;
		
		/* Ean den prokeitai gia trexonta h goniko katalogo ayksanetai o metrhths */
		if(strcmp(entryPtr->d_name,".")&&strcmp(entryPtr->d_name,".."))
		{
			numberOfCountries++;						
		}
			
	}
	
	rewinddir(dir);	
	
	/* Kataskeyh pinaka apo countries */
	countries=malloc(sizeof(char*)*numberOfCountries);
	
	i=0;
	
	/* Sarwsh twn xwrwn */  	
	while(1)
	{
		
		/* Diavasma epomenoy katalogoy xwras */
		entryPtr=readdir(dir);
		
		/* Ean teleiwsan oi eggrafes vgainoyme apo to loop */
		if(entryPtr==NULL)
			break;
		
		/* Ean den prokeitai gia trexonta h goniko katalogo ayksanetai o metrhths */
		if(strcmp(entryPtr->d_name,".")&&strcmp(entryPtr->d_name,".."))
		{
			countries[i]=malloc(strlen(entryPtr->d_name)+1);
			sprintf(countries[i],"%s",entryPtr->d_name);
			
			i++;
		}
		
	}
	
	/* Kleisimo katalogou */
	closedir(dir);
	
	/* Desmeysh mnhmhs gia tis domes me ta pids kai tis xwres poy analamvanei ka8e monitor */
	allMonitors=malloc(sizeof(monitorInfo)*numMonitors);
	
	/* Arxikopoihsh ari8modeikths poy deixnei th thesh ths prwths xwras poy analamvanei o prwtos monitor */
	
	startIndex=0;
		
	for(i=0;i<numMonitors;i++)
	{
		
		/* Ypologismos ari8modeikth poy deixnei th thesh ths teleytaias xwras poy analamvanei o trexwn monitor */
		endIndex=startIndex+numberOfCountries/numMonitors-1;
		
		if(i<numberOfCountries%numMonitors)
			endIndex++;
		
		/* Enhmerwsh ari8modeiktwn sthn antistoixh domh */
		allMonitors[i].startIndex=startIndex;
		allMonitors[i].endIndex=endIndex;
		
		/* Enhmerwsh  ari8modeikth poy deixnei th thesh ths prwths xwras gia ton epomeno monitor */
		startIndex=endIndex+1;
				
	}
	
	/* Desmeysh xwroy pinaka onomatwn name pipes gia metafora dedomenwn apo ton parent stoys monitors */
	parentToMonitorNames=malloc(sizeof(char*)*numMonitors);
	
	/* Kataskeyh pinaka onomatwn */
	for(i=0;i<numMonitors;i++)
	{
		
		parentToMonitorNames[i]=malloc(strlen("parentToMonitor")+4);
		sprintf(parentToMonitorNames[i],"parentToMonitor%d",i);
		
	}
	
	/* Desmeysh xwroy pinaka file descriptors name pipes gia metafora dedomenwn apo ton parent stoys monitors */
	parentToMonitorFds=malloc(sizeof(int)*numMonitors);
		
	/* Diagrafh prohgoymenwn name pipes gia metafora dedomenwn apo ton parent stoys monitors, an yparxoyn */
	for(i=0;i<numMonitors;i++)
		remove(parentToMonitorNames[i]);
		
	/* Dhmiourgia name pipes gia metafora dedomenwn apo ton parent stoys monitors */
	for(i=0;i<numMonitors;i++)
	{
		
		/* H dhmioyrgia elegxetai */
		if(mkfifo(parentToMonitorNames[i],0666)<0)
		{
			
			/* Se periptwsh apotyxias katastrefontai ta name pipes poy dhmioyrghthhkan */
			for(j=0;j<i;j++)
				remove(parentToMonitorNames[j]);
			
			/* Apodesmeyontai ta onomata */
			for(j=0;j<numMonitors;j++)
				free(parentToMonitorNames[j]);
			
			/* Apodesmeyetai o pinakas twn onomatwn */
			free(parentToMonitorNames);
			
			/* Apodesmeyetai o pinakas twn fds */
			free(parentToMonitorFds);
			
			/* Apodesmeyontai ta onomata twn xwrwn */
			for(j=0;j<numberOfCountries;j++)
				free(countries[j]);
			
			/* Apodesmeyetai o pinakas onomatwn twn xwrwn */
			free(countries);
		
			/* Apodesmeyetai o pinakas domwn me ta pids kai tis xwres poy analamvanei ka8e monitor */
			free(allMonitors);
	
			printf("Error in creation name pipes from parent to monitors.\n");
			exit(-1);	
			
		}
		
	}
	
	/* Anoigma twn name pipes gia metafora dedomenwn apo ton parent stoys monitors */
	for(i=0;i<numMonitors;i++)
	{
		
		/* H dhmioyrgia elegxetai */
		if((parentToMonitorFds[i]=open(parentToMonitorNames[i],O_RDWR))<0)
		{
			
			/* Se periptwsh apotyxias katastrefontai ta name pipes poy dhmioyrghthhkan */
			for(j=0;j<numMonitors;j++)
				remove(parentToMonitorNames[j]);
			
			/* Apodesmeyontai ta onomata */
			for(j=0;j<numMonitors;j++)
				free(parentToMonitorNames[j]);
			
			/* Apodesmeyetai o pinakas twn onomatwn */
			free(parentToMonitorNames);
			
			/* Apodesmeyetai o pinakas twn fds */
			free(parentToMonitorFds);
			
			/* Apodesmeyontai ta onomata twn xwrwn */
			for(j=0;j<numberOfCountries;j++)
				free(countries[j]);
			
			/* Apodesmeyetai o pinakas onomatwn twn xwrwn */
			free(countries);
				
			/* Apodesmeyetai o pinakas domwn me ta pids kai tis xwres poy analamvanei ka8e monitor */
			free(allMonitors);
	
			printf("Error in creation name pipes from parent to monitors.\n");
			exit(-1);	
			
		}
		
	}
		
	/* Desmeysh xwroy pinaka onomatwn name pipes gia metafora dedomenwn apo toys monitors ston parent */
	monitorToParentNames=malloc(sizeof(char*)*numMonitors);
	
	/* Kataskeyh pinaka onomatwn */
	for(i=0;i<numMonitors;i++)
	{
		
		monitorToParentNames[i]=malloc(strlen("monitorToparent")+4);
		sprintf(monitorToParentNames[i],"monitorToparent%d",i);
		
	}
		
	/* Desmeysh xwroy pinaka file descriptors name pipes gia metafora dedomenwn apo toys monitors ston parent */
	monitorToParentFds=malloc(sizeof(int)*numMonitors);
		
	/* Diagrafh prohgoymenwn name pipes gia metafora dedomenwn apo toys monitors ston parent, an yparxoyn */
	for(i=0;i<numMonitors;i++)
		remove(monitorToParentNames[i]);
		
	/* Dhmiourgia name pipes gia metafora dedomenwn apo toys monitors ston parent */
	for(i=0;i<numMonitors;i++)
	{
		
		/* H dhmioyrgia elegxetai */
		if(mkfifo(monitorToParentNames[i],0666)<0)
		{
			
			/* Se periptwsh apotyxias katastrefontai ta name pipes poy dhmioyrghthhkan */
			for(j=0;j<i;j++)
				remove(monitorToParentNames[j]);
			
			/* Apodesmeyontai ta onomata */
			for(j=0;j<numMonitors;j++)
				free(monitorToParentNames[j]);
			
			/* Apodesmeyetai o pinakas twn onomatwn */
			free(monitorToParentNames);
			
			/* Apodesmeyetai o pinakas twn fds */
			free(monitorToParentFds);
			
			/* To idio ginetai kai gia ta name pipes gia metafora dedomenwn apo ton parent stoys monitors */
			
			/* Katastrefontai ta name pipes poy dhmioyrghthhkan */
			for(j=0;j<numMonitors;j++)
				remove(parentToMonitorNames[j]);
			
			/* Apodesmeyontai ta onomata */
			for(j=0;j<numMonitors;j++)
				free(parentToMonitorNames[j]);
			
			/* Apodesmeyetai o pinakas twn onomatwn */
			free(parentToMonitorNames);
			
			/* Apodesmeyetai o pinakas twn fds */
			free(parentToMonitorFds);
			
			/* Apodesmeyontai ta onomata twn xwrwn */
			for(j=0;j<numberOfCountries;j++)
				free(countries[j]);
			
			/* Apodesmeyetai o pinakas onomatwn twn xwrwn */
			free(countries);
			
			/* Apodesmeyetai o pinakas domwn me ta pids kai tis xwres poy analamvanei ka8e monitor */
			free(allMonitors);
	
			printf("Error in creation name pipes from monitors to parent.\n");
			exit(-1);	
			
		}
		
	}
	
	/* Anoigma twn name pipes gia metafora dedomenwn apo toys monitors ston parent */
	for(i=0;i<numMonitors;i++)
	{
		
		/* H dhmioyrgia elegxetai */
		if((monitorToParentFds[i]=open(monitorToParentNames[i],O_RDWR))<0)
		{
			
			/* Se periptwsh apotyxias katastrefontai ta name pipes poy dhmioyrghthhkan */
			for(j=0;j<numMonitors;j++)
				remove(monitorToParentNames[j]);
			
			/* Apodesmeyontai ta onomata */
			for(j=0;j<numMonitors;j++)
				free(monitorToParentNames[j]);
			
			/* Apodesmeyetai o pinakas twn onomatwn */
			free(monitorToParentNames);
			
			/* Apodesmeyetai o pinakas twn fds */
			free(monitorToParentFds);
			
			/* To idio ginetai kai gia ta name pipes gia metafora dedomenwn apo ton parent stoys monitors */

			/* Katastrefontai ta name pipes poy dhmioyrghthhkan */
			for(j=0;j<numMonitors;j++)
				remove(parentToMonitorNames[j]);
			
			/* Apodesmeyontai ta onomata */
			for(j=0;j<numMonitors;j++)
				free(parentToMonitorNames[j]);
			
			/* Apodesmeyetai o pinakas twn onomatwn */
			free(parentToMonitorNames);
			
			/* Apodesmeyetai o pinakas twn fds */
			free(parentToMonitorFds);			
			
			/* Apodesmeyontai ta onomata twn xwrwn */
			for(j=0;j<numberOfCountries;j++)
				free(countries[j]);
			
			/* Apodesmeyetai o pinakas onomatwn twn xwrwn */
			free(countries);
			
			/* Apeleytherwsh mnhmhs iwn kai bloom*/
			for(i=0;i<numMonitors;i++)
			{
				for(j=0;j<allMonitors[i].numOfMonitorViruses;j++)
					free(allMonitors[i].monitorViruses[j]);
				
				free(allMonitors[i].monitorViruses);	
				
			}
			
			/* Apodesmeyetai o pinakas domwn me ta pids kai tis xwres poy analamvanei ka8e monitor */
			free(allMonitors);
	
			printf("Error in creation name pipes from parent to monitors.\n");
			exit(-1);	
			
		}
		
	}
		
	/* An xreiastei diorthwnnetai to buffer size */
	if(bufferSize<MINIMUM_BUFFER_SIZE)
	{
		
		bufferSize=MINIMUM_BUFFER_SIZE;
		printf("Buffer size is set to %d\n",bufferSize);
				
	}
	
	
	/******************************* Fash dhmioyrgias monitor ************************************/
	
	/* H arxikh diergasia 8a zhthsei apo toyw monitors poy 8a dhmioyrgh8oyn, toyw ious kai ta bloom */
	strcpy(sendStructuresBack,"1");
	
	/* Desmeysh mnhmhs gia ton buffer */ 
	buffer=malloc(bufferSize);
	
	/* Dhmioyrgia diergasiwn monitor */
	for(i=0;i<numMonitors;i++)
	{
		
		pid=fork();
		
		if(pid!=0)
		{
			
			/* O parent apo8hkeyei to pid */
			allMonitors[i].pid=pid;
			
		}
		else
		{
				 
			/* To child metatrepetai se diergasia monitor */
			execlp("./monitor","./monitor",parentToMonitorNames[i],monitorToParentNames[i],argv[bloomSizeIndex],argv[bufferSizeIndex],inputDir,sendStructuresBack,NULL);
			
		}
		
	}
	
	
	/************** Fash arxikopoihshs epikoinwnias (antallagh plhroforiwn-kataskeyh domwn) ********************/

	
	/* Ta directories dhladh oi xwres poy analamvanei o kathe monitor apostellontai sea ayton */
	for(i=0;i<numMonitors;i++)
	{
		
		/* Arxika ston buffer  grafetai to plhthos twn xwrwn poy analamvanei o monitor */
		sprintf(buffer,"%d ",allMonitors[i].endIndex-allMonitors[i].startIndex+1);
		
		/* Sth synexeia grafontai kai oi xwres */
		for(j=allMonitors[i].startIndex;j<=allMonitors[i].endIndex;j++)
		{
		
			strcat(buffer,countries[j]);
			strcat(buffer," ");
		
		}
		
		/* Apostolh ston buffer */
		write(parentToMonitorFds[i],buffer,bufferSize);
		
	}
	
	/* Gia kathe monitor lamvanontai oi ioi */
	
	for(i=0;i<numMonitors;i++)
	{
		
		/* Diavazetai o buffer poy periexei toys ioys */
		read(monitorToParentFds[i],buffer,bufferSize);
		
		/* Ekxwroyntai oi ioi me th vohtheia ths sxetikhs synarthshs */
		ASS_get_viruses_from_buffer(i);
		
	}
	
	/* Efoson to plhthos twn blooms gia kathe monitor einai plhthos twn iwn, kai efoson kseroyme
	to plhthos twn iwn, mporoyme na desmeysoyme thn katallhlh mnhmh gia ta blooms */
	
	for(i=0;i<numMonitors;i++)
	{
		
		allMonitors[i].monitorBlooms=malloc(allMonitors[i].numOfMonitorViruses*sizeof(char*));
		
		for(j=0;j<allMonitors[i].numOfMonitorViruses;j++)
		{
			
			allMonitors[i].monitorBlooms[j]=malloc(sizeOfBloom);
			
		}
	
	
	}
	
	/* Diabasma twn blooms */
	for(i=0;i<numMonitors;i++)
		for(j=0;j<allMonitors[i].numOfMonitorViruses;j++){
			
			BLOOM_receive(monitorToParentFds[i],buffer,bufferSize,allMonitors[i].monitorBlooms[j],sizeOfBloom);
			
		}
	
	/****************************** Ana8eseis diaxeirishs shmatwn *************************************/

	/* Dhlwnetai oti to SIGCHLD 8a diaxeiristei apo ton SIGNAL_replace_child */
	signal(SIGCHLD,SIGNAL_replace_child);

	/* Dhlwnetai oti to SIGUSR1 8a diaxeiristei apo ton SIGNAL_print_log_file */
	// signal(SIGINT,SIGNAL_print_log_file);
	
	/* Dhlwnetai oti to SIGUSR1 8a diaxeiristei apo ton SIGNAL_print_log_file */
	// signal(SIGINT,SIGNAL_print_log_file);	
	
		
	/********************************** Menu xrhsth ******************************************************/
	
	printf("/");
	
	fgets(line,255,stdin);
	
	sscanf(line,"%s",command);
	
	/* Dhlwnetai oti SIGUSR1 8a agnohthei */
	signal(SIGUSR1,SIG_IGN);
	
	while(strcmp(command,"exit"))
	{
		
		/* Periptwsh entolhs travelRequest */
		if(!strcmp(command,"travelRequest"))
		{
			/* Prin diabastoyn oi parametroi arxikopoiountai */
			citizenID=-1;
			strcpy(citizenIDStr,"");
			strcpy(date,"");
			strcpy(countryFrom,"");
			strcpy(countryTo,"");
			strcpy(virusName,"");
							
			/* Diavazontntai */
			sscanf(line,"%s %s %s %s %s %s",command,citizenIDStr,date,countryFrom,countryTo,virusName);
						
			/* An to teleytaio orisma exei meinei keno shmainei oti den exoyn diavastei swsta ta orismata */
			
			if(!strcmp(virusName,""))
			{
				
				printf("Argument missing or read error!\n");
				continue;
				
			}
			
			/* Metatroph toy id se akeraio */
			citizenID=atoi(citizenIDStr);
			
			/* Entopizoyme to monitor ths xwras proeleyshs */
			monitorID=ASS_get_monitor_id_by_country(countryFrom);
						
			/* Entopizoyme to bloom toy sxetikoy ioy */
			for(i=0;i<allMonitors[monitorID].numOfMonitorViruses;i++)
				if(!strcmp(allMonitors[monitorID].monitorViruses[i],virusName))
					bloom=allMonitors[monitorID].monitorBlooms[i];
			
			/* Ean h anazhthsh sto bloom den deiksei katagrafh tote o citizen sigoyra
			den einai emboliasmenos */
			if(!BLOOM_check_key(bloom,sizeOfBloom,citizenID))
				requestResult=NOT_VACCINATED;
			else
			{
				
				/* Diaforetika xreiazetai psaksimo sth xwra proeleyshs */
				
				/* Grafoyme ston buffer "travelRequestFrom". Etsi o monitor tha "katalabei" oti
				prokeitai na toy zhththoun plhrofories emvoliasmoy. Mazi me ton kediko entolhs tha
				stalei kai to citizenID kathws kai o ios h hmerominia toy request. */
				
				sprintf(buffer,"travelRequestFrom %d %s %s ",citizenID,virusName,date);
				
				/* Apostolh pros ton sxetiko monitor */
				write(parentToMonitorFds[monitorID],buffer,bufferSize);
				
				/* Lhpsh apanthshs */
				read(monitorToParentFds[monitorID],buffer,bufferSize);
				
				/* Diabasma apo ton buffer toy kwdikoy apanthshs */
				sscanf(buffer,"%d",&requestResult);
				
			}
			
			/* Probolh apanthshshs kai enhmerwsh ka8olikwn statistikwn */
			if(requestResult==NOT_VACCINATED)
			{
				
				printf("REQUEST REJECTED – YOU ARE NOT VACCINATED\n");
				totalRejects++;
				
			}
			else if(requestResult==VACCINE_OUT_OF_TIME)
			{
				
				printf("REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE\n");
				totalRejects++;		
				
			}
			else{
				
				printf("REQUEST ACCEPTED – HAPPY TRAVELS\n");
				totalAccepts++;			
			
			}
			
			/* Amesws meta prepei na enhmerw8ei h xwra proorismoy */
			
			
			/* Entopizoyme to monitor ths xwras proorismoy */
			monitorID=ASS_get_monitor_id_by_country(countryTo);
						
			/* Ston buffer prepei na graftei o kwdikos "travelRequestTo" h xwra, h hmeromhmia, kai to apotelesma ths aithshs */
			sprintf(buffer,"travelRequestTo %s %s %s %d ",virusName,countryTo,date,requestResult);
			
			/* Apostelletai o buffer ston antistoixo monitor */			
			write(parentToMonitorFds[monitorID],buffer,bufferSize);
			
		}
		/* Periptwsh entolhs travelStats */
		else if(!strcmp(command,"travelStats"))
		{
			/* Prin diabastoyn oi parametroi arxikopoiountai */
			strcpy(virusName,"");
			strcpy(date1,"");
			strcpy(date2,"");
			strcpy(countryTo,"");
			
			/* Diavazontntai */
			sscanf(line,"%s %s %s %s %s",command,virusName,date1,date2,countryTo);
			
			/* An to proteleytaio orisma exei meinei keno shmainei oti den exoyn diavastei swsta ta orismata */
			if(!strcmp(date2,""))
			{
				
				printf("Argument missing or read error!\n");
				continue;
				
			}
			
						
			/* An to teleytaio orisma exei meinei keno shmainei oti den exoyn exei dothei xwra. H erwthsh 8a treksei
			gia oloys toys monitors	*/
			if(!strcmp(countryTo,""))
			{
				
				/* Ston buffer prepei na graftei o kwdikos "travelRequestTo" h xwra, h hmeromhmia, kai to apotelesma ths aithshs */
				sprintf(buffer,"travelStats %s %s %s - ",virusName,date1,date2);
				
				/* Apostolh erwthshs se loys toys monitor */
				for(i=0;i<numMonitors;i++)
					write(parentToMonitorFds[i],buffer,bufferSize);
				
				/* Mhdenismos athroismatwn paralamvanomenwn metrhtan */
				sumOfReceivedAccepts=0;
				sumOfReceivedRejects=0;
											
				/* Paralavh apanthsewn */
				for(i=0;i<numMonitors;i++)
				{
					read(monitorToParentFds[i],buffer,bufferSize);
					
					/* Oi paralamvanomenoi metrhtes prosti8entai sta a8roismata */
					sscanf(buffer,"%d %d",&receivedAccepts,&receivedRejects);
					
					sumOfReceivedAccepts+=receivedAccepts;
					sumOfReceivedRejects+=receivedRejects;

				}
				
				/* Provolh apanthshs */
				printf("TOTAL REQUESTS %d\n",sumOfReceivedAccepts+sumOfReceivedRejects);
				printf("ACCEPTED %d\n",sumOfReceivedAccepts);
				printf("REJECTED %d\n",sumOfReceivedRejects);
				
			}
			/* Diaforetika exei dothei xwra. H erwthsh 8a treksei sygkekrimeno monitor	*/
			else
			{
				
				/* Ston buffer prepei na graftei o kwdikos "travelRequestTo" h xwra, h hmeromhmia, kai to apotelesma ths aithshs */
				sprintf(buffer,"travelStats %s %s %s %s ",virusName,date1,date2,countryTo);
				
				/* Entopizetai o monitor poy einai ypey8ynos gia th xwra */
				monitorID=ASS_get_monitor_id_by_country(countryTo);
								
				/* Ston buffer prepei na graftei o kwdikos "travelRequestTo" h xwra, h hmeromhmia, to apotelesma ths aithshs kai h xwra */
				sprintf(buffer,"travelStats %s %s %s %s ",virusName,date1,date2,countryTo);
				
				/* Apostolh erwthshs ston  monitor */
				write(parentToMonitorFds[monitorID],buffer,bufferSize);
			
				/* Paralavh apanthshs */
				read(monitorToParentFds[monitorID],buffer,bufferSize);
					
				/* Oi paralamvanomenoi metrhtes diavazobtai */
				sscanf(buffer,"%d %d",&receivedAccepts,&receivedRejects);
				
				/* Provolh apanthshs */
				printf("TOTAL REQUESTS %d\n",receivedAccepts+receivedRejects);
				printf("ACCEPTED %d\n",receivedAccepts);
				printf("REJECTED %d\n",receivedRejects);
				
			}
			
			
		}		
		/* Periptwsh entolhs addVaccinationRecords */
		else if(!strcmp(command,"addVaccinationRecords"))
		{
				
			/* Prin diabastoyn oi parametroi arxikopoiountai */
			strcpy(countryTo,"");
				
			/* Diavazontntai */
			sscanf(line,"%s %s ",command,countryTo);
		
			/* An to teleytaio orisma exei meinei keno shmainei oti den exoyn diavastei swsta ta orismata */
			if(!strcmp(countryTo,""))
			{
				
				printf("Argument missing or read error!\n");
				continue;
				
			}
			
			/* Entopizetai to pid toy monitor poy exei th xwra */
			pid = ASS_get_monitor_pid_by_country(countryTo);

			/* Entopizetai to id toy monitor poy exei th xwra */
			monitorID = ASS_get_monitor_id_by_country(countryTo);
	
			/* Apostelletai se ayton to shma SIGUSR1 */
			kill(pid,SIGUSR1);
			
			/* Amesws meta paralamvanontai ta nea blooms apo ton monitor */
			
			for(j=0;j<allMonitors[monitorID].numOfMonitorViruses;j++){
			
				BLOOM_receive(monitorToParentFds[monitorID],buffer,bufferSize,allMonitors[monitorID].monitorBlooms[j],sizeOfBloom);
				
			}
			
			
		}
		/* Periptwsh entolhs searchVaccinationStatus*/
		else if(!strcmp(command,"searchVaccinationStatus"))
		{
				 
			/* Prin diabastoyn oi parametroi arxikopoiountai */
			citizenID=-1;
				
			/* Diavazontntai */
			sscanf(line,"%s %d ",command,&citizenID);
			
		
			/* An to teleytaio orisma exei meinei -1 shmainei oti den exoyn diavastei swsta ta orismata */
			if(citizenID==-1)
			{
				
				printf("Argument missing or read error!\n");
				continue;
				
			}
			
			/* Ston buffer prepei na graftei o kwdikos "searchVaccinationStatus" to citizenID */
			sprintf(buffer,"searchVaccinationStatus %d ",citizenID);

			/* Apostelletai to erwthma se oloys toys monitors */
			for(i=0;i<numMonitors;i++)
				write(parentToMonitorFds[i],buffer,bufferSize);
			
			/* Paralamvanontai oi apanthseis kai emfanizontai sthn o8onh */			
			for(i=0;i<numMonitors;i++)
			{
				read(monitorToParentFds[i],buffer,bufferSize);
				printf("%s",buffer);
			}
						
			
		}
		
		/*  Diavasma ths epomenhs entolhs */
		
		printf("/");
	
		fgets(line,255,stdin);
	
		sscanf(line,"%s",command);
		
	}	
	
	
	/************************************* Diadikasia eksodoy *****************************************/
	
	/* H diaxeirish toy SIGCHLD epanerxetai sthn default */
	signal(SIGCHLD,SIG_IGN);
	
	/* Kataskeyazeai h diadromh toy log file */
	sprintf(line,"log_file.%d",getpid());

	/* Anoigetai */
	fpLogFile=fopen(line,"w");

	memset(buffer,0,bufferSize);
	
	/* Kataskeyazetai to periexomeno tyo arxeioy sto zhtoymeno format */
	for(i=0;i<numberOfCountries;i++)
		sprintf(buffer+strlen(buffer),"%s\n",countries[i]);

	sprintf(buffer+strlen(buffer),"TOTAL TRAVEL REQUESTS  %d\nACCEPTED %d\nREJECTED %d\n",totalAccepts+totalRejects,totalAccepts,totalRejects);

	/* To arxeio grafetai kai kleinei */
	fprintf(fpLogFile,"%s",buffer);
	
	fclose(fpLogFile);
	
	/* H diergasia skotwnei toys monitors */
	for(i=0;i<numMonitors;i++)
	{
		
		kill(allMonitors[i].pid,SIGKILL);
		
	}
	

	/* Epistrofh diergasiwn monitor */
	for(i=0;i<numMonitors;i++){
		
		wait(&status);
		
	}
		
	/* Kleisimo twn name pipes gia metafora dedomenwn apo toys monitors ston parent */
	for(i=0;i<numMonitors;i++)
		close(monitorToParentFds[i]);
	
	/* Katastrefontai ta name pipes poy dhmioyrghthhkan */
	for(j=0;j<numMonitors;j++)
		remove(monitorToParentNames[j]);
	
	/* Apodesmeyontai ta onomata */
	for(j=0;j<numMonitors;j++)
		free(monitorToParentNames[j]);
	
	/* Apodesmeyetai o pinakas twn onomatwn */
	free(monitorToParentNames);
	
	/* Apodesmeyetai o pinakas twn fds */
	free(monitorToParentFds);
		
	/* Kleisimo twn name pipes gia metafora dedomenwn apo ton parent stoys monitors */
	for(i=0;i<numMonitors;i++)
		close(parentToMonitorFds[i]);
	
	/* Katastrefontai ta name pipes poy dhmioyrghthhkan */
	for(j=0;j<numMonitors;j++)
		remove(parentToMonitorNames[j]);
	
	/* Apodesmeyontai ta onomata */
	for(j=0;j<numMonitors;j++)
		free(parentToMonitorNames[j]);
	
	/* Apodesmeyetai o pinakas twn onomatwn */
	free(parentToMonitorNames);
	
	/* Apodesmeyetai o pinakas twn fds */
	free(parentToMonitorFds);			
			
	/* Apodesmeyontai ta onomata twn xwrwn */
	for(j=0;j<numberOfCountries;j++)
		free(countries[j]);
	
	
		
	printf("BYEEEEE\n");
	
	exit(0);
	
}

/*********************************** Boh8htikes synarthseis **************************************************************/

/* H synarthsh dexetai th symvoloseira mias xwras kai epistrefei to id toy monitor poy einai ypey8ynos (-1) se apotyxia */
int ASS_get_monitor_id_by_country(char* countryStr)
{
	
	int countryIndex;
	int monitorId;
	
	/* Seiriakh anazhthsh kai an vroume th xwra spame to loop */
	for(countryIndex=0;countryIndex<numberOfCountries;countryIndex++)
		if(!strcmp(countries[countryIndex],countryStr))
			break;
	
	/* An to countryIndex exei ginei numberOfCountries den exei vrethei h xwra */
	if(countryIndex==numberOfCountries)
		return -1;
	
	/* Se diaforetikh periptwsh sarwnontai oi domes twn monitor gia na vrethei h armodia */
	for(monitorId=0;monitorId<numMonitors;monitorId++)
		if(allMonitors[monitorId].startIndex<=countryIndex)
			if(allMonitors[monitorId].endIndex>=countryIndex)
				return monitorId;
	
}

/* H synarthsh dexetai th symvoloseira mias xwras kai epistrefei to pid toy monitor poy einai ypey8ynos (-1) se apotyxia */
int ASS_get_monitor_pid_by_country(char* countryStr)
{
	
	int countryIndex;
	int monitorId;
	
	/* Seiriakh anazhthsh kai an vroume th xwra spame to loop */
	for(countryIndex=0;countryIndex<numberOfCountries;countryIndex++)
		if(!strcmp(countries[countryIndex],countryStr))
			break;
	
	/* An to countryIndex exei ginei numberOfCountries den exei vrethei h xwra */
	if(countryIndex==numberOfCountries)
		return -1;
	
	/* Se diaforetikh periptwsh sarwnontai oi domes twn monitor gia na vrethei h armodia */
	for(monitorId=0;monitorId<numMonitors;monitorId++)
		if(allMonitors[monitorId].startIndex<=countryIndex)
			if(allMonitors[monitorId].endIndex>=countryIndex)
				return allMonitors[monitorId].pid;
	
}  


/* Diavasma iwn apo ton buffer gia logarismo enos monitorInfo */
void ASS_get_viruses_from_buffer(int monitorId)
{
	
	/* Deikths char* gia thn strtok poy tha xrhsimopoihthei amesws meta */
	char* token;
	
	int i,j;
	
	/* Diavasma prwtoy token apo ton buffer */
	token=strtok(buffer," ");
	   
	/* Ayto to prwto token einai kai to plhthos twn iwn */
	allMonitors[monitorId].numOfMonitorViruses=atoi(token);
	
	/* Desmeyetai mnhmh gia toys ioys */
	allMonitors[monitorId].monitorViruses=malloc(allMonitors[monitorId].numOfMonitorViruses*sizeof(char*));
	
	/* Diavazontai oi ioi */
	
	for(i=0;i<allMonitors[monitorId].numOfMonitorViruses;i++)
	{
		token=strtok(NULL," ");
		
		/* Desmeysh mnhmhs gia ton epomeno io */
		allMonitors[monitorId].monitorViruses[i]=malloc(strlen(token)+1);
		
		/* Antigrafh */
		strcpy(allMonitors[monitorId].monitorViruses[i],token);

	}
	
	
}

/***************************************** Handlers ********************************************************/

void SIGNAL_replace_child(int s)
{
	
	/* Xrhsimoi arithmodeiktes */
	int i,j;
	
	/* Otan einai "0" o monitor poy dhmioyrgeitai den 8a steilei pisw kai toys ioys kai ta bloom*/
	char sendStructuresBack[2];
	
	/* To pid toy child poy ekane exit */
	pid_t pid=wait(NULL);
	
	/* 8a xrhsimopoihsoyme topiko buffer gia na mh dhmioyrghsoyme problhma se endexomenh entolh poy trexei*/
	
	char* buffer=malloc(bufferSize);

	/* To mege8os toy bloom san symboloseira */
	char strBloomSize[24];
	
	/* To mege8os toy buffer san symboloseira */	
	char strBufferSize[24];
	
	/* Kataskeyazetai to mege8os toy bloom san symboloseira */
	sprintf(strBloomSize,"%lu",sizeOfBloom);
	
	/* Kataskeyazetai to mege8os toy buffer san symboloseira */
	sprintf(strBufferSize,"%d",bufferSize);
	
	printf("Omg they killed Kenny!\n");
	
	/* Entopizoyme to id toy monitor poy prepei na antikatasta8ei */
	for(i=0;i<numMonitors;i++)
		if(allMonitors[i].pid==pid)
			break;
		
	pid=fork();
		
	if(pid!=0)
	{
		
		/* O parent apo8hkeyei to pid */
		allMonitors[i].pid=pid;
		
	}
	else
	{
			 
		/* To child metatrepetai se diergasia monitor xwris thn ypoxrewsh na steilei pisw ioys kai blooms */
		strcpy(sendStructuresBack,"0");
		
		execlp("./monitor","./monitor",parentToMonitorNames[i],monitorToParentNames[i],strBloomSize,strBufferSize,inputDir,sendStructuresBack,NULL);
		
	}
	
	/* H mhtrikh stelnei kanonika ta direcotries sto monitor antikatastath */
	
	/* Arxika ston buffer  grafetai to plhthos twn xwrwn poy analamvanei o monitor */
	sprintf(buffer,"%d ",allMonitors[i].endIndex-allMonitors[i].startIndex+1);
	
	/* Sth synexeia grafontai kai oi xwres */
	for(j=allMonitors[i].startIndex;j<=allMonitors[i].endIndex;j++)
	{
	
		strcat(buffer,countries[j]);
		strcat(buffer," ");
	
	}
	
	/* Apostolh ston buffer */
	write(parentToMonitorFds[i],buffer,bufferSize);
	
	/* To pid toy antikatastath pairnei th thesh toy ston sxetiko pinaka twn montors*/
	allMonitors[i].pid=pid;
	
	free(buffer);
	
}



