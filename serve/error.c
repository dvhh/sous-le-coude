#include <stdio.h>
#include <stdlib.h>

struct HttpError {
	int code;
	char* message;
};

#include "httpErrors.h"

static int cmpHttpError ( const void *a, const void *b ) {
	const struct HttpError *pa = (struct HttpError*)a;
	const struct HttpError *pb = (struct HttpError*)b;
	return pa->code - pb->code;
}

void serveError ( const char* verb, const char* path, const char* extra, const int flag, FILE* inpout, FILE* output) {
	const struct HttpError key={flag,NULL};
	const struct HttpError *entry = bsearch( &key, httpErrors, sizeof( httpErrors ) / sizeof( struct HttpError ), sizeof( struct HttpError ), cmpHttpError );
	fprintf( output, "HTTP/1.1 %s\r\n", entry->message );
	fprintf( output, "\r\n");
}
