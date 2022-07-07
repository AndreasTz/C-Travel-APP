#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<signal.h>

#include "bloom.h"
#include "citizen.h"
#include "skipList.h"
#include "constants.h"
#include "date.h"
#include "request.h"


/***************** Shmantikes times ********************/

// Fifo descriptor paralavhs
int recvFifoFd;

// Fifo descriptor apostolhs
int sendFifoFd;

/* Plh8os xwrwn pou exei analavei o monitor */
int numberOfCountries;

/* Pinakas me ta onomata twn xwrwn */
char** countries;

/* Pinakas me requests ths xwras */
request** requests;

/* Lista twn citizen (Arxikopoieitai sto NULL) */
citizenRecordNode* citizenRecordList=NULL;

/* Pinakas me ta onomata twn iwn (Arxikopoieitai sto NULL)*/
char** viruses=NULL;

/* Pinakas me ta blooms (Arxikopoieitai sto NULL - ena bloom gia kathe io)*/
char** blooms=NULL;


/* Plhthos iwn (Arxikopoieitai sto 0)*/
int numOfViruses=0;

/* Pinakas apo skip lists emvoliasmenwn (Arxikopoieitai sto NULL)*/
skipList** vaccinatedPeopleSkipLists=NULL;

/* Pinakas apo skip lists mh emvoliasmenwn (Arxikopoieitai sto NULL)*/
skipList** nonVaccinatedPeopleSkipLists=NULL;


/* Pinakas me tis diadromes twn ypokatalogwn twn xwrwn */
char** countryPaths;

/* Pinakas me to plhthos twn arxeiwn ana xwra */
int* numOfFilesPerCountry;

/* Pinakas me ta onomata twn arxeiwn ana xwra */
char*** fileNamesPerCountry;

/* Mege8os bloom filter pou dinetai apo th grammh entolwn */
unsigned long sizeOfBloom;

/* Mege8os buffer pou dinetai apo th grammh entolwn */
int bufferSize;

/* O buffer */ 
char* buffer;

/* Metrhths synolikwn apodoxwn */
int totalAccepts;

/* Metrhths synolikwn aporripsewn */
int totalRejects;

/* Eggrafh citizen */
citizenRecord* foundRecord;

/************************** Diaxeirish shmatwn ******************************************/

/* Handler poy xeirizetai to SIGUSR1. Pros8etei eggrafes newn arxeiwn stis domes. */
void SIGNAL_new_files_reader(int s);

/* Handler poy xeirizetai ta SIGINT kai SIGQUIT. Pros8etei eggrafes newn arxeiwn stis domes. */
void SIGNAL_print_log_file(int s);

int main(int argc,char* argv[]){
	
	/* O kentrikos katalogos */
	char* inputDir;
	
	int test;
	
	/* Gia paralaves apo ton buffer */
	char tempString[128];
	
	/* Gia paralaves apo ton buffer */
	int offset;
	
	/* Anoigma fifo paralavhs */
	recvFifoFd=open(argv[1],O_RDWR);
	
	/* Anoigma fifo apostolhs */
	sendFifoFd=open(argv[2],O_RDWR);
	
	/* Metablhth genikhs kai pollaplhs xrhshs */
	int length;
	
	/* Arithmodeiktes genikhs kai pollaplhs xrhshs*/
	int i,j;
	
	/* Deikths katalogoy */
	DIR* dir;

	/* Deikths eggrafhs katalogoy */ 
	struct dirent* entry;

	/* Pinakas xarakthrwn gia thn entolh pou erxetai apo ton gonio */
	char command[256];
	
	/* Id citizen san string */
	char citizenIDStr[12];
	
	/* Id citizen */
	int citizenID;
	
	/* Hmeromhnia */
	char date[12];

	/* Hmeromhnia */
	char date1[12];

	/* Hmeromhnia */
	char date2[12];

	/* Xwra */
	char country[64];

	/* Onoma ioy */
	char virusName[32];
	
	/* Kwdisko emvoliasmoy h mh emvoliasmou*/
	int vaccineCode;
	
	/* Deikths komvou poy exei prokypsei apo anazhthsh */
	skipListNode* foundSkipListNode; 
	
	/* Deikter request */
	request* newRequest;
	request* currentRequest;
	
	/* Metrhths epityxwn aithsewn poy apostelletai */
	int sendAccepts;

	/* Metrhths anepityxwn aithsewn poy apostelletai */
	int sendRejects;
	
	/* Elegxos epityxoys anoigmatos */
	if(recvFifoFd<0)
	{
		
		printf("Open fifo fail\n");		
		exit(-1);
		
	}
	
	if(sendFifoFd<0)
	{
		
		printf("Open fifo fail\n");		
		exit(-1);
		
	}
	
	/* Diavasma mege8ous bloom */
	sscanf(argv[3],"%lu",&sizeOfBloom);
	
	/* Diavasma mege8ous buffer */
	bufferSize=atoi(argv[4]);
	
	/* Diavasma onomatos kentrikoy katalogoy */
	inputDir=argv[5];
	
	/* An xreiastei diorthwnnetai to buffer size */
	if(bufferSize<MINIMUM_BUFFER_SIZE)
	{
		
		bufferSize=MINIMUM_BUFFER_SIZE;
		printf("Buffer size is set to %d\n",bufferSize);
				
	}
	
	/* Arsikopoihsh metrhth synolikwn apodoxwn */
	totalAccepts=0;

	/* Arsikopoihsh metrhth synolikwn aporripsewn */
	totalRejects=0;
	
	/* Desmeysh mnhmhs gia ton buffer */ 
	buffer=malloc(bufferSize);
	
	/* Paralavh twm onomatwn twn xwrwn */
	read(recvFifoFd,buffer,bufferSize);

	sscanf(buffer,"%s",tempString);

	sscanf(tempString,"%d",&numberOfCountries);
	
	/* Enhmerwnetai to offset dhladh h thesh diavasmatos toy apomenoy dedomenoy */
	offset=strlen(tempString)+1;
	
	/* Desmeysh mnhmhs gia ron pinaka me ta onomata twn xwrwn*/
	countries=malloc(numberOfCountries*sizeof(char*));

	/* Desmeysh mnhmhs gia ron pinaka me ta requests twn xwrwn*/
	requests=malloc(numberOfCountries*sizeof(request*));

	/* Desmeysh mnhmhs gia ron pinaka me tis diadromes twn ypokatalogwn twn xwrwn*/
	countryPaths=malloc(numberOfCountries*sizeof(char*));

	for(i=0;i<numberOfCountries;i++)
	{
		
		/* Diavazetai h epomenh xwra */
		sscanf(buffer+offset,"%s",tempString);
		
		length=strlen(tempString);
		
		/* Desmeyetai katallhlos xwros gia to onoma ths kai th diadromh toy ypokatalogoy ths */
		countries[i]=malloc(length+1);
		
		countryPaths[i]=malloc(strlen(inputDir)+1+length+1);
		
		/* Ekxwroyntai to onoma ths kai th diadromh toy ypokatalogoy ths */
		strcpy(countries[i],tempString);
				
		sprintf(countryPaths[i],"%s/%s",inputDir,tempString);
		
		/* Enhmerwnetai to offset */
		offset+=(length+1);
		
		/* O antistoixos deikths sto prwto request ti8etai NULL */
		requests[i]=NULL;
	}
	
	/* Desmeyetai xwros gia ton pinaka me to plhthos twn arxeiwn ana xwra */
	numOfFilesPerCountry=malloc(sizeof(int)*numberOfCountries);
	
	/* Arxikopoihsh twn metrhtwn */
	for(i=0;i<numberOfCountries;i++)
		numOfFilesPerCountry[i]=0;
	
	for(i=0;i<numberOfCountries;i++)
	{
		
		/* Anoigei o ypokatalogos ths xwras */
		dir=opendir(countryPaths[i]);
		
		// Diavasma arxeiwn eggrafwn
		while(1){
			
			/* Diavazetai h epomenh eggrafh */
			entry=readdir(dir);
			
			/* An einai kenh shmainei oti teleiwsame me ton ypokatalogo - xwra kai pame ston epomeno */
			if(!entry)
				break;
			
			/* An h eggrafh den einai oute ayth toy trexonta oyte ayth toy gonikoy katalogou prosmetratai */
			if(strcmp(entry->d_name,"..")&&strcmp(entry->d_name,"."))
				numOfFilesPerCountry[i]++;
				
		}
		
		/* Kleinei o ypokatalogos ths xwras */
		closedir(dir);
		
	}
		
	/* Edw ginetai ek neoy sarwsh, alla aythn th fora, apothhkeyontai oi diadromes twn arxeiwn
	(pragma xrhsimo gia to erwthma 3 ths topothethshs neoy arxeioy) kai diavazontai ta arxeia */
	
	fileNamesPerCountry=malloc(numberOfCountries*sizeof(char**));
	
	for(i=0;i<numberOfCountries;i++)
	{
		
		fileNamesPerCountry[i]=malloc(numOfFilesPerCountry[i]*sizeof(char*));
		
		/* Anoigei o ypokatalogos ths xwras */
		dir=opendir(countryPaths[i]);
		
		j=0;
		
		// Diavasma arxeiwn eggrafwn
		while(1){
			
			/* Diavazetai h epomenh eggrafh */
			entry=readdir(dir);
			
			/* An einai kenh shmainei oti teleiwsame me ton ypokatalogo - xwra kai pame ston epomeno */
			if(!entry)
				break;
			
			/* An h eggrafh den einai oute ayth toy trexonta oyte ayth toy gonikoy katalogou prosmetratai */
			if(strcmp(entry->d_name,"..")&&strcmp(entry->d_name,"."))
			{
				
				// Desmeyetai xwros gia th diadromh toy arxeioy
				length=strlen(countryPaths[i])+1+strlen(entry->d_name)+1;
				
				fileNamesPerCountry[i][j]=malloc(length);
				
				strcpy(fileNamesPerCountry[i][j],countryPaths[i]);
									
				strcat(fileNamesPerCountry[i][j],"/");
				
				strcat(fileNamesPerCountry[i][j],entry->d_name);
								
				/* Edw tha ginei h eisagwgh twn eggrafwn stis domes */
				
				CITIZEN_get_records_of_file(fileNamesPerCountry[i][j],countries[i],&citizenRecordList,&viruses,&numOfViruses,&blooms,sizeOfBloom,&vaccinatedPeopleSkipLists,&nonVaccinatedPeopleSkipLists);
								
				j++;
				
				
				/* Telos eisagwghs twn eggrafwn toy arxeioy stis domes */
			}
				
		}
		
		/* Kleinei o ypokatalogos ths xwras */
		closedir(dir);
		
	}
	
	if(!strcmp(argv[6],"1"))
	{
	
		/* Apostolh iwn */
		
		/* Arxika ftiaxnetai o buffer */
		sprintf(buffer,"%d ",numOfViruses);
		
		for(i=0;i<numOfViruses;i++)
		{
			
			strcat(buffer,viruses[i]);
			strcat(buffer," ");
					
		}
		/* Apostolh toy buffer me toys ious */
		write(sendFifoFd,buffer,bufferSize);
		
		/* Apostolh twn blooms */

		for(i=0;i<numOfViruses;i++)
		{
			BLOOM_send(sendFifoFd,buffer,bufferSize,blooms[i],sizeOfBloom); 
		
		}
	
	}
	else
		printf("You bastards!\n");
	
	/****************************** Ana8eseis diaxeirishs shmatwn *************************************/

	/* Dhlwnetai oti to SIGUSR1 8a diaxeiristei apo ton SIGNAL_new_files_reader */
	signal(SIGUSR1,SIGNAL_new_files_reader);

	/* Dhlwnetai oti to SIGUSR1 8a diaxeiristei apo ton SIGNAL_print_log_file */
	signal(SIGINT,SIGNAL_print_log_file);

	/* Dhlwnetai oti to SIGUSR1 8a diaxeiristei apo ton SIGNAL_print_log_file*/
	signal(SIGQUIT,SIGNAL_print_log_file);


	/*************** O monitor mpainei se fash allhlepidrashs me ton gonio toy ************************/

	while(1)
	{
		
		/* Paralavh erwthshs apo ton gonio */
		read(recvFifoFd,buffer,bufferSize);
		
		/* Diavazetai h erwthshsh */
		sscanf(buffer,"%s",command);
		
		/* Periptwsh travelRequest sthn opoia o monitor periexei th xwra proeleyshs */
		if(!strcmp("travelRequestFrom",command))
		{
		
			/* Periptwsh travelRequestFrom (zhteitai na eksetastei h aithsh) */
			/* Sthn periptwsh ayth diavazontai ta citizenID, virusName kai date */
			
			sscanf(buffer+strlen(command)+1,"%d %s %s",&citizenID,virusName,date);
						
			/* Entopismos toy iou */
			for(i=0;i<numOfViruses;i++)
			{
				
				if(!strcmp(viruses[i],virusName))
					break;
				
			}
			
			/* Ean den vrethhke, tote h xwra den emvoliazei fia ton io. Ua prepei na epistrafei kedikow mh emvoliasmenoy */
			if(i==numOfViruses)
			{
					
				sprintf(buffer, "%d ",NOT_VACCINATED);
				write(sendFifoFd,buffer,bufferSize);
				
			}
			else
			{
				
				/* Se periptwsh poy exei vre8ei o ios anazhteitai -an yparxei- o komvos skip list tou citizen*/
				foundSkipListNode=searchNode(vaccinatedPeopleSkipLists[i],citizenID);
				
				/* Ean h anazhthsh apotyxei o xitizen den einai emboliasmenos gia ton en logw io */
				if(!foundSkipListNode)
				{
						
					sprintf(buffer, "%d ",NOT_VACCINATED);
					write(sendFifoFd,buffer,bufferSize);
					
				}
				else
				{
					/* Diaforetika einai. Prepei omws na ejetastei h hmeromhnia */
					
					/* Ean h hmeromhniaemvoiasmoy einai mesa sto wksamhno to peras toy opoioy
					orizetai apo th dosmenh, tote o emvoliasmos einai mesa sto zhtoymeno xroniko diasthma. 
					Sthn periptwsh ayth epistrefetai VACCINATED. */
					
					if(DATE_isInSemesterOf(foundSkipListNode->date,date))
						sprintf(buffer, "%d ",VACCINATED);
					/* Diaforetika epistrefetai VACCINE_OUT_OF_TIME */
					else
						sprintf(buffer, "%d ",VACCINE_OUT_OF_TIME);
					
					write(sendFifoFd,buffer,bufferSize);
									
				}			
			}		
		}		
		/* Periptwsh travelRequest sthn opoia o monitor periexei th xwra proorismoy */
		else if(!strcmp("travelRequestTo",command))
		{
		
			/* To lhf8en mhnyma periexei plhroforia apodoxhs 'h aporripshs ths aithshs */
				
			sscanf(buffer+strlen(command)+1,"%s %s %s %d",virusName,country,date,&vaccineCode);
						
			/* Enhmerwsh metrhtwn */
			if(vaccineCode==VACCINATED)
				totalAccepts++;
			else
				totalRejects++;
			
			/* Entopismos ths xwras */
			for(i=0;i<numberOfCountries;i++)
			{
				
				if(!strcmp(countries[i],country))
					break;
				
			}
						
			/* Desmeyetai xwros gia to neo request */
			newRequest=malloc(sizeof(request));
			
			/* Antigrafetai h hmeromhnia aithshs */
			strcpy(newRequest->date,date);
			
			/* Antigrafetai to onoma toy ioy */
			strcpy(newRequest->virusName,virusName);
			
			/* Kataxwreitai to apotelesma ths aithshs */
			if(vaccineCode==VACCINATED)
				newRequest->result=1;
			else
				newRequest->result=0;
					
			/* Eisagetai to request */
			newRequest->next=requests[i];
			
			requests[i]=newRequest;
			
		}
		/* Periptwsh travelStats  */
		else if(!strcmp("travelStats",command))
		{
			
			/* To lhf8en mhnyma periexei plhroforia apodoxhs 'h aporripshs ths aithshs */
				
			sscanf(buffer+strlen(command)+1,"%s %s %s %s",virusName,date1,date2,country);
			
			/* Arxikopoihsh metrhtwn poy 8a apostalloyn */
			sendAccepts=0;
			sendRejects=0;
			
			/* Periptwsh anazhthshs se oles tis xwres */
			if(!strcmp(country,"-"))
			{
								
				/* PSarwnontai oles oi xwres... */
				for(i=0;i<numberOfCountries;i++)
				{
					
					/* ...kia gia kathe xwra sarwnonontai oi eggrafes twn aithsewn */
					currentRequest=requests[i];
					
					while(currentRequest)
					{
						
						/* An mia po8hkeymenh aithsh afora ston io poy mas endiaferei... */
						if(!strcmp(currentRequest->virusName,virusName))
						{
							
							/* ...eksetazetai an einai kai mesa sto eyros twn hmeromhniwn poy mas endiaferei. */
							if(DATE_isBetween(currentRequest->date,date1,date2))
							{
															
								/* Ean symvaninei kai ayto katametratai h aithsh */
								if(currentRequest->result)
									sendAccepts++;
								else
									sendRejects++;
								
							}							
						}
												
						currentRequest=currentRequest->next;
						
					}					
				
				}
				
				/* Ta apotelesmata typwnontai ston buffer... */
				sprintf(buffer, "%d %d ",sendAccepts,sendRejects);
				
				/* ... kai apostellontai */
				write(sendFifoFd,buffer,bufferSize);
				
			}
			/* Periptwsh anazhthshs se sygkekrimenh xwra */
			else
			{
								
				/* Entopismos ths xwras */
				for(i=0;i<numberOfCountries;i++)
				{
					
					if(!strcmp(countries[i],country))
						break;
					
				}
				
				currentRequest=requests[i];
					
				while(currentRequest)
				{
					
					/* An mia po8hkeymenh aithsh afora ston io poy mas endiaferei... */
					if(!strcmp(currentRequest->virusName,virusName))
					{
						
						/* ...eksetazetai an einai kai mesa sto eyros twn hmeromhniwn poy mas endiaferei. */
						if(DATE_isBetween(currentRequest->date,date1,date2))
						{
							/* Ean symvaninei kai ayto katametratai h aithsh */
							if(currentRequest->result)
								sendAccepts++;
							else
								sendRejects++;
							
						}							
					}
											
					currentRequest=currentRequest->next;
					
				}
			
				/* Ta apotelesmata typwnontai ston buffer... */
				sprintf(buffer, "%d %d ",sendAccepts,sendRejects);
				
				/* ... kai apostellontai */
				write(sendFifoFd,buffer,bufferSize);
				
			}
					
		}
		/* Periptwsh searchVaccinationStatus  */
		else if(!strcmp("searchVaccinationStatus",command))
		{
			
			/* To lhf8en mhnyma periexei plhroforia apodoxhs 'h aporripshs ths aithshs */				
			sscanf(buffer+strlen(command)+1,"%d",&citizenID);
			
			/* Ka8arizei o buffer */
			memset(buffer,0,bufferSize);
			
			/* Anazhthsh tou citizen */
			foundRecord=CITIZEN_get(citizenRecordList,citizenID);
			
			if(foundRecord)
			{
			
				/* Ean vrethei grafontai oi sxetikes plhrofories ston buffer */
				sprintf(buffer,"%d %s %s %s\nAGE %d\n",citizenID,foundRecord->firstName,foundRecord->lastName,foundRecord->country,foundRecord->age);
				
				/* Elegxontai oles oi skip lists kai symplhrwnontai oi plhrofories emvoliasmoy */
				for(i=0;i<numOfViruses;i++)
				{
					
					/* Elegxetai h skip list twn emvoliasmenwn ston io */
					foundSkipListNode=searchNode(vaccinatedPeopleSkipLists[i],citizenID);	
					
					/* Ean vrethoyn plhrofories, symplhrwnetai to epistrefomeno */
					if(foundSkipListNode)
					{					
						strcat(buffer,viruses[i]);					
						strcat(buffer," VACCINATED ON ");					
						strcat(buffer,foundSkipListNode->date);	
						strcat(buffer,"\n");	
					}
					else
					{
						/* Elegxetai h skip list twn anemvoliastwn ston io */
						foundSkipListNode=searchNode(nonVaccinatedPeopleSkipLists[i],citizenID);	
						
						if(foundSkipListNode)
						{					
							strcat(buffer,viruses[i]);					
							strcat(buffer," NOT YET VACCINATED\n");	
						}
					}
					
				}
			}
			
			/* O buffer apostelletai ws apanthsh */
			write(sendFifoFd,buffer,bufferSize);
			
		}
	}


	
	/* Apodesmeyetai h mnhmh gia ton buffer */ 
	free(buffer);
	
	/* Apodesmeyetai h mnhmh gia ton pinaka onomatwn arxeiwn ana ypokatalogo (xwra) */	
	
	for(i=0;i<numberOfCountries;i++){
		
		/* Ka8arismos listas requests */
		REQUEST_clear_requests(requests[i]);
			
		for(j=0;j<numOfFilesPerCountry[i];j++){
			
			free((fileNamesPerCountry[i])[j]);
			
		}
	}
	
	free(requests);
	
	for(i=0;i<numberOfCountries;i++)
		free(fileNamesPerCountry[i]);
	
	free(fileNamesPerCountry);
	
	/* Apodesmeyetai h mnhmh gia ton pinaka metrhtwn arxeiwn ana ypokatalogo (xwra) */	
	free(numOfFilesPerCountry);
	
	/* Kleisimo fifos */
	close(recvFifoFd);
	close(sendFifoFd);
	
	return 0;
	
}

/* Handler poy xeirizetai to SIGUSR1. Pros8etei eggrafes newn arxeiwn stis domes. */
void SIGNAL_new_files_reader(int s)
{

	/* Diadromh arxeioy gia to opoio 8a elegx8ei h yparksh toy*/
	char checkFilePath[1024];
	
	/* Arithmodeiktes genikhs kai pollaplhs xrhshs*/
	int i,j;
	
	/* Deikths katalogoy */
	DIR* dir;

	/* Deikths eggrafhs katalogoy */ 
	struct dirent* entry;
	
	/* Pinakas xarakthrwn gia thn entolh pou erxetai apo ton gonio */
	char command[256];
	
	/* Metablhth genikhs kai pollaplhs xrhshs */
	int length;
	
	/* Shmaia yparkshs */
	int exists;

	for(i=0;i<numberOfCountries;i++)
	{
		
		/* Anoigei o ypokatalogos ths xwras */
		dir=opendir(countryPaths[i]);
		
		j=0;
		
		// Diavasma arxeiwn eggrafwn
		while(1){
			
			/* Diavazetai h epomenh eggrafh */
			entry=readdir(dir);
			
			/* An einai kenh shmainei oti teleiwsame me ton ypokatalogo - xwra kai pame ston epomeno */
			if(!entry)
				break;
			
			/* An h eggrafh den einai oute ayth toy trexonta oyte ayth toy gonikoy katalogou prosmetratai */
			if(strcmp(entry->d_name,"..")&&strcmp(entry->d_name,"."))
			{
				
				// Desmeyetai xwros gia th diadromh toy arxeioy
				length=strlen(countryPaths[i])+1+strlen(entry->d_name)+1;
								
				strcpy(checkFilePath,countryPaths[i]);
									
				strcat(checkFilePath,"/");
				
				strcat(checkFilePath,entry->d_name);
				
						
				/* Edw tha prepei na elegx8ei ean h diadromh einai hdh kataxwrismenh */
				
				exists=0;
				
				/* Sarwnontai oi apothikeymenes diadromes. An vrethei h kataskeyasmenh, tote to
				arxeio exei diavastei, opote proxwrame sto epomeno. */
				for(j=0;j<numOfFilesPerCountry[i];j++)
				{
					
					if(!strcmp(fileNamesPerCountry[i][j],checkFilePath))
					{
						exists=1;
						break;			
						
					}
					
				}
				
				if(exists)
					continue;
				
				/* An h eltelesh ftasei edw shmainei oti h diadromh checkFilePath antiproswpeyei neo-eiserxomeno arxeio */
				
				
				/* Edw tha ginei eisagwgh ths neas diadromhs*/
				
				/* Desmeyetai xwros gia neo deikth pros ayth */
				fileNamesPerCountry[i]=realloc(fileNamesPerCountry[i],sizeof(char*)*(numOfFilesPerCountry[i]+1));
				
				/* Desmeyetai xwros gia thn akoloythia twn xarakthrwn ths */
				fileNamesPerCountry[i][numOfFilesPerCountry[i]]=malloc(strlen(checkFilePath)+1);
				
				/* Antigrafetai */
				strcpy(fileNamesPerCountry[i][numOfFilesPerCountry[i]],checkFilePath);
				
				/* Ayksanetai o arithmos twn diadromwn */
				numOfFilesPerCountry[i]++;				
				
				/* Edw tha ginei h eisagwgh twn eggrafwn stis domes */
				
				CITIZEN_get_records_of_file(checkFilePath,countries[i],&citizenRecordList,&viruses,&numOfViruses,&blooms,sizeOfBloom,&vaccinatedPeopleSkipLists,&nonVaccinatedPeopleSkipLists);
								
				j++;
				
				
				/* Telos eisagwghs twn eggrafwn toy arxeioy stis domes */
			}
				
		}
		
		/* Kleinei o ypokatalogos ths xwras */
		closedir(dir);
		
	}
	
	
	/* Apostolh twn blooms */

	for(i=0;i<numOfViruses;i++)
	{
		BLOOM_send(sendFifoFd,buffer,bufferSize,blooms[i],sizeOfBloom); 
	
	}
	
	
}

void SIGNAL_print_log_file(int s)
{
	int i;
	
	/* To logfile arxeio */
	char logName[24];
	
	/* Log symvoloseira */
	char log[1024];
	
	/* Deikths arxeioy */
	FILE* fpLogFile;
	
	/* Kataskeyazeai h diadromh toy log file */
	sprintf(logName,"log_file.%d",getpid());

	/* Anoigetai */
	fpLogFile=fopen(logName,"w");

	memset(log,0,bufferSize);
	
	/* Kataskeyazetai to periexomeno tyo arxeioy sto zhtoymeno format */
	for(i=0;i<numberOfCountries;i++)
		sprintf(log+strlen(log),"%s\n",countries[i]);

	sprintf(log+strlen(log),"TOTAL TRAVEL REQUESTS  %d\nACCEPTED %d\nREJECTED %d\n",totalAccepts+totalRejects,totalAccepts,totalRejects);

	/* To arxeio grafetai kai kleinei */
	fprintf(fpLogFile,"%s",log);
	
	fclose(fpLogFile);
	
	
}














