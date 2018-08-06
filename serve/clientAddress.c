#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "clientAddress.h"

void serveClientAdress ( const char* verb, const char* path, const char* extra, const int flag, FILE* input,FILE* output) {
	const char* headerName = "X-Forwarded-For:";
	char line[LINE_MAX];
	char *clientAddress=NULL;
	while(fgets(line, LINE_MAX, input)!=NULL) {
		if( strncmp(headerName, line, LINE_MAX) == 0 ) {

		}
		fprintf( output, line );
		// end condition
		if( line[0] == 0 ) { break; }
		if( line[0] =='\r' && line[1]=='\n' ) { break; }
	}	
}
