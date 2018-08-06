#include <stdio.h>
#include <limits.h>

#include "test.h"
#include "error.h"

void serveTest ( const char* verb, const char* path, const char* extra, const int flag, FILE* input,FILE* output) {
	printHead(200,output);
	//fputs( "HTTP/1.1 200 OK\r\n", output );
	fputs( "Content-Type: text/plain\r\n", output );
	fputs( "\r\n", output );

	fputs( verb, output );
	fputs( "\r\n", output );
	fputs( path, output);
	fputs( "\r\n", output);

	char line[LINE_MAX];
	while(fgets(line, LINE_MAX, input)!=NULL) {
		fputs( line, output );
		if( line[0] == 0 ) { break; }
		if( line[0] =='\r' && line[1]=='\n' ) { break; }
	}
}
