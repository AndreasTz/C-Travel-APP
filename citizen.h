#ifndef _CITIZEN_
#define _CITIZEN_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "skipList.h"
#include "bloom.h"

/* Struct gia thn kathe eggrafh */
struct citizenRecord{
	
	/* To citizen id */
	int citizenID;
	
	/* Onoma atomou */
	char* firstName;
	
	/* Epwnymo atomou */
	char* lastName;
	
	/* Onoma xwras */
	char* country;
	
	/* H hlikia */
	int age;
	
};

typedef struct citizenRecord citizenRecord;

/* Struct gia komvo listas eggrafwn */
struct citizenRecordNode{
	
	citizenRecord* rec;
	
	struct citizenRecordNode* next;	
	
};

typedef struct citizenRecordNode citizenRecordNode;

/* Eisagei tis eggrafes pou prokyptoyn apo tis grammes enos arxeioy stis domes */
void CITIZEN_get_records_of_file(char* filename,char* countryName,citizenRecordNode** citizenRecordList,char*** viruses,int* numOfViruses,char*** blooms, unsigned long sizeOfBloom,skipList*** vaccinatedPeopleSkipLists,skipList*** nonVaccinatedPeopleSkipLists);

/* Emfanizei lista twn citizen */
void CITIZEN_print_list(citizenRecordNode* citizenRecordList);

/* Epistrefei thn eggrafh enos citizen me dedomeno id (NULL se apotyxia) */
citizenRecord* CITIZEN_get(citizenRecordNode* citizenRecordList, int citizenID);


#endif








