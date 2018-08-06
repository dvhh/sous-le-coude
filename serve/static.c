#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "static.h"
#include "error.h"

static void getStatic( const char* contentType, const char* inputPath, FILE* input, FILE* output ) {
	fprintf(stderr, "getStatic [%s] [%s]\n", contentType, inputPath);

	FILE* inputFile = fopen( inputPath, "r" );
	if( inputFile == NULL ) {
		perror( inputPath );
		return serveError( NULL, NULL, inputPath, 404, input, output );
	}

	fprintf( output, "HTTP/1.1 200 OK\r\n");
	fprintf( output, "Content-Type: %s\r\n", contentType);
	fprintf( output, "Connection: close\r\n");

	fprintf( output, "\r\n");

	uint8_t *buffer = malloc(8192*sizeof(uint8_t));

	while( 1 ) {
		const size_t readCount  = fread( buffer, sizeof(uint8_t), 8192, inputFile );
		const size_t wroteCount = fwrite( buffer, sizeof(uint8_t), readCount, output );
		if( readCount != wroteCount ) {
			fprintf(stderr, "r:%zu w:%zu\n", readCount, wroteCount);
			perror(NULL);
			return;
		}
		if( readCount != 8192 ) {
			break;
		}
	}

	fclose( inputFile );
	free(buffer);
}

void serveStatic ( const char* verb, const char* path, const char* extra, const int flag, FILE* input, FILE* output) {
	fprintf(stderr, "serveStatic [%s] [%s] [%s]\n", verb, path, extra);

	if( extra == NULL ) {
		return serveError( verb, path, extra, 500, input, output );
	}
	char *copy = strdup( extra );
	if( strcmp( verb, "GET" ) == 0 ) {
		const char *contentType = copy;
		char *inputPath = NULL;
		for(unsigned int i=0; i < 1024; ++i ) {
			if( copy[i] == 0 ) {
				break;
			}
			if( copy[i] == ';' ) {
				copy[i] = 0;
				inputPath = copy + i + 1;
				break;
			}
		}

		if( inputPath == NULL ) {
			return serveError( verb, path, inputPath, 404, input, output );
		}

		getStatic( contentType, inputPath, input, output);
		return;
	}

	return serveError( verb, path, extra, 405, input, output );
}
