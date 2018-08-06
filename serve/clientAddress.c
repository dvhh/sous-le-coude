#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "clientAddress.h"
#include "error.h"

void serveClientAddress ( const char* verb, const char* path, const char* extra, const int flag, FILE* input,FILE* output) {
	const char* headerName = "X-Forwarded-For:";
	const size_t headerLength = strlen( headerName );
	char line[LINE_MAX];
	char *clientAddress=NULL;
	while(fgets(line, LINE_MAX, input)!=NULL) {
		if( line[0] == 0 ) { break; }
		if( line[0] =='\r' && line[1]=='\n' ) { break; }

		const size_t length = strlen( line );
		if( length <= headerLength ) {
			continue;
		}
		line[headerLength] = 0;
		//fprintf( stderr, "%i\t", strncmp(headerName, line, LINE_MAX) );
		//fputs( line, stderr );
		//fputs( "\n", stderr );
		if( strncmp(headerName, line, headerLength) == 0 ) {
			clientAddress = line + headerLength + 1;
			line[length-2] = 0;
			//fputs( clientAddress, stderr );
			//fputs( "\n", stderr );
			break;
		}
	}
	if( clientAddress == NULL ) {
		return serveError( verb, path, extra, 400, input, output);
	}
	printHead( 200, output );
	fputs( "Content-Type: text/plain\r\n", output );
	fputs( "\r\n", output );
	fputs( clientAddress, output);
}
