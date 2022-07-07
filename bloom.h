#ifndef _BLOOM_
#define _BLOOM_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define KEY_SIZE 32

/* Dhmioyrgia filtroy bloom */
char* BLOOM_create(unsigned long bloomSize);

/* Prosthhkh kleidioy sto filtro bloom */
void BLOOM_add_key(char* bloom,unsigned long bloomSize,int key);

/* Elegxei thn kataxwrish enos kleidioy sto boom. Epistrefei 0 sto "oxi" kai 1 sto "isws" */
int BLOOM_check_key(char* bloom,unsigned long bloomSize,int key);

/* Apostolh bloom */
void BLOOM_send(int fdSend,char* buffer,int bufferSize,char* bloomSource,unsigned long bloomSize);

/* Paralabh bloom */
void BLOOM_receive(int fdReceive,char* buffer,int bufferSize,char* bloomDestination,unsigned long bloomSize);

/* Ypologismos checksum (gia dikh mas xrhsh epivevaiwshs orthothtas apostolhs paralabhs) */
int BLOOM_check_sum(char* bloom,unsigned long bloomSize);

#endif








