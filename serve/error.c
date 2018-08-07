#include <stdio.h>
#include <stdlib.h>

#include "error.h"

struct HttpError {
	int code;
	char* message;
};

#include "httpErrors.h"

static int cmpHttpError ( const void *pKey, const void *pData ) {
	const int *key = pKey;
	const struct HttpError *data = (struct HttpError*)pData;
	return (*key) - data->code;
}

void printHead( const int code, FILE* output ) {
	const struct HttpError *entry = bsearch( &code, httpErrors, sizeof( httpErrors ) / sizeof( struct HttpError ), sizeof( struct HttpError ), cmpHttpError );
	fprintf( output, "HTTP/1.1 %s\r\n", entry->message );
}

void serveError ( const char* verb, const char* path, const char* extra, const int flag, FILE* inpout, FILE* output) {
	printHead( flag, output );
	fputs( "\r\n", output );
}

