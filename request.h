#ifndef _REQUEST_
#define _REQUEST_

typedef struct request{
	
	/* Ios aithshs */
	char virusName[24];	
	
	/* Hmeromhnia aithshs */
	char date[11];
	
	/* Apotelesma ths aithshs */
	int result;
	
	struct request* next;
	
}request;

/* Ka8arismos listas requests */
void REQUEST_clear_requests(request* rec);

#endif 


