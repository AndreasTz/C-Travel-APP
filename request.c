#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "request.h"

/* Ka8arismos listas requests */
void REQUEST_clear_requests(request* rec){
	
	request* delRec=rec;
	
	while(delRec)
	{
		rec=delRec->next;
		
		free(delRec);
		
		delRec=rec;
		
	}
	
	
	
}


