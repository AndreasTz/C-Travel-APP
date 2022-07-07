#include "citizen.h"

/* Eisagei tis eggrafes pou prokyptoyn apo tis grammes enos arxeioy stis domes */

void CITIZEN_get_records_of_file(char* filename,char* countryName,citizenRecordNode** citizenRecordList,char*** viruses,int* numOfViruses, char*** blooms,unsigned long sizeOfBloom, skipList*** vaccinatedPeopleSkipLists,skipList*** nonVaccinatedPeopleSkipLists){
	
	/* Anoigma arxeioy */
	FILE* citizencitizenRecordsFileFp=fopen(filename,"r");
	
	char lineInFile[256];
		
	/* To citizen id */
	int citizenID;
	
	/* Onoma atomou */
	char firstName[16];
	
	/* Epwnymo atomou */
	char lastName[16];

	/* Onoma xwras */
	char country[24];

	/* H hlikia */
	int age;
	
	/* Onoma iou */
	char virusName[24];
	
	/* Emvoliasmos */
	char yesOrNo[6];
	
	/* Hmeromhnies */
	char date1[12];
	char date2[12];
	char date[12];

	/* Bohthitikh metablhth */
	int found;
	
	/* Shmaia */
	int exists;
	
	/* Deikths komvoy me eggrafh */
	citizenRecordNode* currentNode;	
	
	/* Deikths neoy komvoy me eggrafh */
	citizenRecordNode* newNode;	
		
	/* Arithmodeikths */
	int i;
	
	/* Deikths se komvo skip list */
	skipListNode* skNode;

	while(fgets(lineInFile,256,citizencitizenRecordsFileFp))
	{
			
		if(!strcmp(lineInFile,"\n"))
			continue;
		
		/* An h lista einai kenh dhmiourgeitai */
		if((*citizenRecordList)==NULL){
			
			(*citizenRecordList)=malloc(sizeof(citizenRecordNode));
			
			/* Grafetai se ayton ton komvo h prwth eggrafh */
			(*citizenRecordList)->rec=malloc(sizeof(citizenRecord));
			(*citizenRecordList)->next=NULL;
			
			/* Diavazontai ta pedia ths eggrafhs */			
			sscanf(lineInFile,"%d %s %s %s %d %s %s %s",&citizenID,firstName,lastName,country,&age,virusName,yesOrNo,date);
						
			/* Ekxwreitai h taytothta toy atomoy */
			(*citizenRecordList)->rec->citizenID = citizenID;
			
			/* Desmeyetai xwros gia to onoma */
			(*citizenRecordList)->rec->firstName=malloc(strlen(firstName)+1);
			
			/* Grafetai to onoma */
			sprintf((*citizenRecordList)->rec->firstName,"%s",firstName);
			
			/* Desmeyetai xwros gia to epwnymo */
			(*citizenRecordList)->rec->lastName=malloc(strlen(lastName)+1);
			
			/* Grafetai to onoma */
			sprintf((*citizenRecordList)->rec->lastName,"%s",lastName);
										
			/* O deikths ths eggrafhs tithetai na deixnei sth xwra */
			(*citizenRecordList)->rec->country=countryName;
			
			/* Ekxwreitai h hlikia toy atomoy */
			(*citizenRecordList)->rec->age = age;
						
			/* Desmeyetai xwros gia ton pinaka twn iwn */
			(*viruses)=malloc(sizeof(char**));
			
			/* Desmeyetai xwros gia ton pinaka twn bloom */
			(*blooms)=malloc(sizeof(char**));
			
			/* Desmeyetai xwros gia ton prwto io */
			(*viruses)[0]=malloc(strlen(virusName)+1);
			
			/* Antigrafetai o prwtos ios */
			sprintf((*viruses)[0],"%s",virusName);
			
			/* Dhmiourgeitai to prwto bloom */
			(*blooms)[0]=BLOOM_create(sizeOfBloom);
			
			/* Desmeyetai xwros gia ton prwto deikth se vaccinated skip list */
			(*vaccinatedPeopleSkipLists)=malloc(sizeof(skipList*));
			
			/* Dhmioyrgeitai h prwth vaccinated skip list */
			(*vaccinatedPeopleSkipLists)[0]=createSkipList(SKIP_LIST_HEIGHT);
			
			/* Desmeyetai xwros gia ton prwto deikth se non vaccinated skip list */
			(*nonVaccinatedPeopleSkipLists)=malloc(sizeof(skipList*));
			
			/* Dhmioyrgeitai h prwth non vaccinated skip list */
			(*nonVaccinatedPeopleSkipLists)[0]=createSkipList(SKIP_LIST_HEIGHT);
						
			/* Ayksanetai o metrhths twn iwn */
			(*numOfViruses)++;
			
			/* Ekxwreitai h dieythynsh toy prwtoy komvoy ston deikth currentNode */
			/* Parakatw (eksw apo thn if) tha akoloythisei eisagwgh toy antistoixoy citizenRecord sthn skip list */
			currentNode=(*citizenRecordList);
					
		}
		else
		{
			
			/* Diavazontai ta pedia ths eggrafhs */			
			sscanf(lineInFile,"%d %s %s %s %d %s %s %s",&citizenID,firstName,lastName,country,&age,virusName,yesOrNo,date);
											
			/* Eksetazetai an yparxei o ios */
			found=0;

			for(i=0;i<*numOfViruses;i++)
			{
				
				if(!strcmp((*viruses)[i],virusName))
				{
					
					found=1;
					break;
					
				}				
				
			}

			/* Ean de yparxei prosti8etai */
			if(!found)
			{
				// Ayksanetai to mege8os toy pinaka iwn me realloc
				(*viruses) = (char**)realloc((*viruses),(*numOfViruses+1)*sizeof(char*));
				
				// Desmeyetai o katallhlos xwros
				(*viruses)[*numOfViruses]=malloc(strlen(virusName)+1);	

				// Ayksanetai to mege8os toy pinaka twn bloom me realloc
				(*blooms) = (char**)realloc((*blooms),(*numOfViruses+1)*sizeof(char*));
				
				/* Dhmiourgeitai to epomeno bloom */
				(*blooms)[*numOfViruses]=BLOOM_create(sizeOfBloom);
				
				/* Desmeyetai xwros gia ton deikth neas vaccinated skip list */
				(*vaccinatedPeopleSkipLists)=(skipList**)realloc((*vaccinatedPeopleSkipLists),(*numOfViruses+1)*sizeof(skipList*));
				
				/* Dhmioyrgeitai h nea vaccinated skip list */
				(*vaccinatedPeopleSkipLists)[*numOfViruses]=createSkipList(SKIP_LIST_HEIGHT);
				
				/* Desmeyetai xwros gia ton deikth neas non vaccinated skip list */
				(*nonVaccinatedPeopleSkipLists)=(skipList**)realloc((*nonVaccinatedPeopleSkipLists),(*numOfViruses+1)*sizeof(skipList*));
			
				/* Dhmioyrgeitai h nea non vaccinated skip list */
				(*nonVaccinatedPeopleSkipLists)[*numOfViruses]=createSkipList(SKIP_LIST_HEIGHT);
							
				/* Antigrafetai o neos ios */
				sprintf((*viruses)[*numOfViruses],"%s",virusName);
				
				(*numOfViruses)++;
				
			}
			
			/* Eksetazetai an yparxei hdh eggrafh toy atomoy */
			currentNode=(*citizenRecordList);
			
			while(currentNode!=NULL)
			{
				
				if(currentNode->rec->citizenID==citizenID)
					break;
				else
					currentNode=currentNode->next;
										
			}
			
			
			/* An o currentNode einai NULL tote dhmioyrgeitai neos komvos kai mpainei sthn lista */
			if(currentNode==NULL)
			{
				
				/* Dhmioyrgeitai neos komvos */				
				newNode=malloc(sizeof(citizenRecordNode));
			
				/* Dhmioyrgeitai se ayton ton komvo eggrafh */
				newNode->rec=malloc(sizeof(citizenRecord));
			
				/* Ekxwreitai h taytothta toy atomoy */
				newNode->rec->citizenID = citizenID;
				
				/* Desmeyetai xwros gia to onoma */
				newNode->rec->firstName=malloc(strlen(firstName)+1);
				
				/* Grafetai to onoma */
				sprintf(newNode->rec->firstName,"%s",firstName);
				
				/* Desmeyetai xwros gia to epwnymo */
				newNode->rec->lastName=malloc(strlen(lastName)+1);
				
				/* Grafetai to onoma */
				sprintf(newNode->rec->lastName,"%s",lastName);
				
				/* Ekxwreitai h xwra */
				newNode->rec->country=countryName;
								
				/* Ekxwreitai h hlikia toy atomoy */
				newNode->rec->age = age;
				
				newNode->next=(*citizenRecordList);
				
				(*citizenRecordList)=newNode;
				
				/* Ekxwreitai h dieythynsh toy neoy komvoy ston deikth currentNode */
				/* Parakatw (eksw apo thn if) tha akoloythisei eisagwgh toy antistoixoy citizenRecord sthn skip list */
				currentNode=newNode;
				
			}
			
		}
		
		
		/* Eisagwgh toy komvoy se skip list kai bloom */
		
		/* Arxika vriskoyme ton io */
		for(i=0;i<(*numOfViruses);i++)
			if(!strcmp((*viruses)[i],virusName))
				break;
		
		/* Eisagwgh emvoliasmenoy atomoy */	
		if(!strcmp(yesOrNo,"YES"))
		{
					
			skNode=insert((*vaccinatedPeopleSkipLists)[i],citizenID,&exists);
			
			/* Ean exei emboliastei hdh to atomo mhnyma sfalmatos */
			if(exists)
			{
				//printf("DOUBLE INSERTION ATTEMPTED: CITIZEN %d OF %s ALREADY VACCINATED FOR %s ON %s.\n",citizenID,countryName,virusName,skNode->date);
			}
			else
			{
			/* Diaforetika ekxwroyntai ta aparaithta stoixeia */
				
				skNode->date=malloc(strlen(date)+1);
				
				sprintf(skNode->date,"%s",date);
				
				skNode->citizenRecordPtr=currentNode->rec;
				
				/* Epishs ginetai eisagwgh kai sto bloom */
				BLOOM_add_key((*blooms)[i],sizeOfBloom,citizenID);
			}
			
		}
		/* Eisagwgh mh emvoliasmenoy atomoy */	
		else
		{
			
			skNode=insert((*nonVaccinatedPeopleSkipLists)[i],citizenID,&exists);
			
			/* Ean to atomo den exei anaferthei hdh san mh emvoliasmeno anaferetai */
			if(!exists)
			{
				
				skNode->date=NULL;
				
				skNode->citizenRecordPtr=currentNode->rec;
				
			}
			
			
		}
		
	}
	
}

/* Emfanizei lista twn citizen */
void CITIZEN_print_list(citizenRecordNode* citizenRecordList)
{
		
	printf("\nList records:");
	while(citizenRecordList)
	{
		printf("%d ",citizenRecordList->rec->citizenID);
		printf("%s ",citizenRecordList->rec->firstName);
		printf("%s ",citizenRecordList->rec->lastName);
		printf("%s ",citizenRecordList->rec->country);
		printf("%d\n",citizenRecordList->rec->age);
	
		citizenRecordList=citizenRecordList->next;
	}
	printf("End of list.\n\n");
	
}

/* Epistrefei thn eggrafh enos citizen me dedomeno id (NULL se apotyxia) */
citizenRecord* CITIZEN_get(citizenRecordNode* citizenRecordList, int citizenID)
{
			
	while(citizenRecordList)
	{
		if(citizenRecordList->rec->citizenID==citizenID)
			return citizenRecordList->rec;
		
		citizenRecordList=citizenRecordList->next;
	}
		
	return NULL;
}









