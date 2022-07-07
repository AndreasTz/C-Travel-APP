#ifndef _DATE_
#define _DATE_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Epistrefei 1 an h date einai anamesa stis date1 kai date2 */
int DATE_isBetween(char* date,char* date1,char* date2);

/* Epistrefei 1 an h date einai mesa sto eksamhno poy teleiwnei stnn semDate */
int DATE_isInSemesterOf(char* date, char* semDate);


#endif








