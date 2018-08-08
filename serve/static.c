#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "static.h"
#include "error.h"

static char* readFileToBuffer(const char* inputPath, const off_t size) {
	FILE* inputFile = fopen( inputPath, "r" );

	if( inputFile == NULL ) {
		perror( inputPath );
		return NULL;
	}

	char *buffer =malloc( size );
	if( buffer == NULL ) { return NULL; }
	if( fread( buffer, size, 1, inputFile ) != 1 ) {
		free(buffer);
		perror( inputPath );
		return NULL;
	}
	fclose(inputFile);

	return buffer;
}

static char* getRFC2822Time(char* output,const size_t max, const time_t t) {
	//const time_t t = time(NULL);
	if ( strftime( output, max, "%a, %d %b %Y %T %z", localtime( &t ) ) == 0 ) {
		return NULL;
	}
	return output;
}

static void getStatic( const char* contentType, const char* inputPath, FILE* input, FILE* output ) {
	fprintf(stderr, "getStatic [%s] [%s]\n", contentType, inputPath);

	struct stat statInput;
	if( stat( inputPath, &statInput ) != 0 ) {
		perror( inputPath );
		return serveError( NULL, NULL, inputPath, 500, input, output );
	}

	char* buffer = readFileToBuffer( inputPath, statInput.st_size );
	if( buffer == NULL ) {
		return serveError( NULL, NULL, inputPath, 404, input, output );
	}

	printHead( 200, output );
	fprintf( output, "Content-Type: %s\r\n", contentType );
	fprintf( output, "Allow: GET, HEAD\r\n" );
	fprintf( output, "Cache-Control: max-age=3600\r\n" );
	fprintf( output, "Content-Length: %ld\r\n", statInput.st_size );
	char timeBuffer[1024];
	if( getRFC2822Time(timeBuffer, 1024, time(NULL) ) == NULL ) { return serveError( NULL, NULL, "time", 500, input, output ); }
	fprintf( output, "Date: %s\r\n", timeBuffer );
	fprintf( output, "Connection: close\r\n" );
	if( getRFC2822Time(timeBuffer, 1024, statInput.st_mtime ) == NULL ) { return serveError( NULL, NULL, "time", 500, input, output ); }
	fprintf( output, "Last-Modified: %s\r\n", timeBuffer );

	fprintf( output, "\r\n");

	if( fwrite(buffer, statInput.st_size, 1, output ) != 1 ) {
		perror(NULL);
		return;
	}

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

