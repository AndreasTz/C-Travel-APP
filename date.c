#include "date.h"

/* Epistrefei 1 an h date einai anamesa stis date1 kai date2 */
int DATE_isBetween(char* date,char* date1,char* date2)
{
	int j;
	
	char date_copy[24];
	char date1_copy[24];
	char date2_copy[24];
	
	char day[4],day1[4],day2[4];
	char month[4],month1[4],month2[4];
	char year[8],year1[8],year2[8];
	
	/* Antigrafontai oi hmeromhnies */
	sprintf(date_copy,"%s",date);
	sprintf(date1_copy,"%s",date1);
	sprintf(date2_copy,"%s",date2);
	
	/* Afairoyntai oi payles */
	for(j=0;j<strlen(date_copy);j++)
		if(date_copy[j]=='-')
			date_copy[j]=' ';
	
	for(j=0;j<strlen(date1_copy);j++)
		if(date1_copy[j]=='-')
			date1_copy[j]=' ';
	
	for(j=0;j<strlen(date2_copy);j++)
		if(date2_copy[j]=='-')
			date2_copy[j]=' ';
		
		
	/* Oi hmeromhnies spane se mera mhna etos*/
	sscanf(date_copy,"%s %s %s",day,month,year);
	sscanf(date1_copy,"%s %s %s",day1,month1,year1);
	sscanf(date2_copy,"%s %s %s",day2,month2,year2);
		
	if(atoi(year)>atoi(year1) || atoi(year)==atoi(year1) && atoi(month)>atoi(month1) || atoi(year)==atoi(year1) && atoi(month)==atoi(month1) && atoi(day)>=atoi(day1))
		if(atoi(year)<atoi(year2) || atoi(year)==atoi(year2) && atoi(month) <atoi(month2) || atoi(year)==atoi(year2) && atoi(month)==atoi(month2) && atoi(day)<=atoi(day2))
		{
				
			return 1;
			
		}
		
	return 0;
	
}



/* Epistrefei 1 an h date einai mesa sto eksamhno poy teleiwnei stnn semDate */
int DATE_isInSemesterOf(char* date, char* semDate)
{
	int j;

	char date_copy[24];
	char semDate_copy[24];
	char sixMothsBack_copy[24];
	
	char day[4],semDay[4],sixMothsBackDay[4];
	char month[4],semMonth[4],sixMothsBackMonth[4];
	char year[8],semYear[8],sixMothsBackYear[8];
	
	int sixMothsBackMonthInt;
	int sixMothsBackYearInt;
	
	/* Antigrafontai oi hmeromhnies */
	sprintf(date_copy,"%s",date);
	sprintf(sixMothsBack_copy,"%s",semDate);
	sprintf(semDate_copy,"%s",semDate);
	
	/* Afairoyntai oi payles */
	for(j=0;j<strlen(date_copy);j++)
		if(date_copy[j]=='-')
			date_copy[j]=' ';
	
	for(j=0;j<strlen(sixMothsBack_copy);j++)
		if(sixMothsBack_copy[j]=='-')
			sixMothsBack_copy[j]=' ';
	
	for(j=0;j<strlen(semDate_copy);j++)
		if(semDate_copy[j]=='-')
			semDate_copy[j]=' ';
		
		
	/* Oi hmeromhnies spane se mera mhna etos*/
	sscanf(date_copy,"%s %s %s",day,month,year);
	sscanf(sixMothsBack_copy,"%s %s %s",sixMothsBackDay,sixMothsBackMonth,sixMothsBackYear);
	sscanf(semDate_copy,"%s %s %s",semDay,semMonth,semYear);
	
	sixMothsBackYearInt=atoi(sixMothsBackYear);
	sixMothsBackMonthInt=atoi(sixMothsBackMonth);

	sixMothsBackMonthInt-=6;
	if(sixMothsBackMonthInt<1)
	{
		sixMothsBackMonthInt+=12;
		sixMothsBackYearInt-=1;
	}
			
	if(atoi(year)>sixMothsBackYearInt || atoi(year)==sixMothsBackYearInt && atoi(month)>sixMothsBackMonthInt || atoi(year)==sixMothsBackYearInt && atoi(month)==sixMothsBackMonthInt && atoi(day)>=atoi(sixMothsBackDay))
		if(atoi(year)<atoi(semYear) || atoi(year)==atoi(semYear) && atoi(month) <atoi(semMonth) || atoi(year)==atoi(semYear) && atoi(month)==atoi(semMonth) && atoi(day)<=atoi(semDay))
		{
			return 1;			
		}

	return 0;
	
}







