#include <stdio.h>
#include <limits.h>

#include "test.h"

void serveTest ( const char* verb, const char* path, const char* extra, const int flag, FILE* input,FILE* output) {
	fprintf( output, "HTTP/1.1 200 OK\r\n");
	fprintf( output, "Content-Type: text/plain\r\n");
	fprintf( output, "\r\n");

	fprintf( output, verb);
	fprintf( output, "\r\n");
	fprintf( output, path);
	fprintf( output, "\r\n");

	char line[LINE_MAX];
	while(fgets(line, LINE_MAX, input)!=NULL) {
		fprintf( output, line );
		if( line[0] == 0 ) { break; }
		if( line[0] =='\r' && line[1]=='\n' ) { break; }
	}
}
