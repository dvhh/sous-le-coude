#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <fnmatch.h>

void serveTest( const char* verb, const char* path, const char* extra, const int flag, FILE* input,FILE* output) {
	
}

void serveStatic( const char* verb, const char* path, const char* extra, const int flag, FILE* inpout, FILE* output) {

}

struct HttpError {
	int code;
	char* message;
};

#include "httpErrors.h"

int cmpHttpError( const void *a, const void *b ) {
	const struct HttpError *pa = (struct HttpError*)a;
	const struct HttpError *pb = (struct HttpError*)b;
	return pa->code - pb->code;
}

void serveError( const char* verb, const char* path, const char* extra, const int flag, FILE* inpout, FILE* output) {
	const struct HttpError key={flag,NULL};
	const struct HttpError *entry = bsearch( &key, httpErrors, sizeof( httpErrors ) / sizeof( struct HttpError ), sizeof( struct HttpError ), cmpHttpError );
	fprintf( output, "HTTP/1.1 %s\r\n", entry->message );
	fprintf( output, "\r\n");
}

struct Route {
	char* verb;
	char* path;
	char* extra;
	void (*handler)(const char*,const char*,const char*,const int,FILE*,FILE*);
};

#include "route.h"

void handleClient(const int serverSocket) {

	struct sockaddr_in client_addr;
	socklen_t clientaddr_len = sizeof ( client_addr );

	const int client = accept( serverSocket, (struct sockaddr *)&client_addr, &clientaddr_len);

	if( client == -1 ) {
		perror(NULL);
		exit( EXIT_FAILURE );
	}
	const pid_t pid = fork();
	if( pid == -1 ) {
		perror(NULL);
		exit( EXIT_FAILURE );
	}
	if( pid > 0 ) {
		close(client);
		return;
	}
	FILE *out = fdopen(client, "w");
	FILE *in  = fdopen(client, "r");
	// read first line

	char line[LINE_MAX];
	if( fgets( line, LINE_MAX, in ) == NULL ) {
		perror(NULL);
		exit( EXIT_FAILURE );
	}

	const size_t length = strnlen( line, LINE_MAX );
	if( line[length-1] != '\n' ) {
		serveError(NULL,NULL,NULL,414,in,out);
		exit( EXIT_FAILURE );
	}


	fprintf( out, "HTTP/1.1 200 OK\r\n");
	fprintf( out, "Content-Type: text/plain\r\n");
	fprintf( out, "\r\n");
	fprintf( out, line);
	fprintf( out, "\r\n");

	char* verb=line;
	char* path=NULL;
	char* proto=NULL;
	for( int i=0; i<length; ++i ) {
		if(line[i]==' ') {
			line[i] = 0;
			if(path==NULL) {
				path=line + i + 1;
			}else{
				proto=line + i + 1;
				break;
			}
		}
	}
	fprintf( out, verb);
	fprintf( out, "\r\n");
	fprintf( out, path);
	fprintf( out, "\r\n");
	fprintf( out, proto);
	
	//fprintf( out, "\r\n");
	exit( EXIT_SUCCESS );
}

void serve(const int port) {
	// open connection
	const int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if ( serverSocket == -1 ) {
		perror( NULL );
		exit( EXIT_FAILURE );
	}

	struct sockaddr_in addr_in;
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons( port );
	addr_in.sin_addr.s_addr = INADDR_ANY;

	if ( bind( serverSocket, ( struct sockaddr * ) &addr_in, sizeof( struct sockaddr_in ) ) == -1 ) {
		perror( NULL );
		exit( EXIT_FAILURE );
	}

	if ( listen( serverSocket,32 ) == -1 ) {
		perror( NULL );
		exit( EXIT_FAILURE );
	}
	// service client connection
	while(1) {
		handleClient(serverSocket);
	}
	close(serverSocket);
}

int main(int argc, char **argv) {

	if (argc != 2) {
		printf("%s [port-number]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	const int port = atoi(argv[1]);
	serve(port);

	exit(EXIT_SUCCESS);
}
