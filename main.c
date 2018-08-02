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

void serveTest( char* verb, char* path, char* extra,int flag, FILE* input,FILE* output) {
	
}

void serveStatic( char* verb, char* path, char* extra,int flag, FILE* inpout, FILE* output) {

}

struct httpError {
	int code;
	char* message;
};

#include "httpErrors.h"

void serveError( char* verb, char* path, char* extra,int flag, FILE* inpout, FILE* output) {
	
}

struct Route {
	char* verb;
	char* path;
	char* extra;
	void (*handler)(char*,char*,char*,int,FILE*,FILE*);
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
	FILE *out = fdopen(client, "w");
	FILE *in  = fdopen(client, "r");
	// read first line
	
	fprintf(out, "HTTP/1.1 200 OK\r\n");
	fprintf(out, "Content-Type: text/plain\r\n");
	fprintf(out, "\r\n");

	char line[LINE_MAX];
	if( fgets( line, LINE_MAX, in ) == NULL ) {
		perror(NULL);
		goto clientCleanup;
	}

	const size_t length = strnlen( line, LINE_MAX );
	if( line[length-1] != '\n' ) {
		serveError(NULL,NULL,NULL,414,in,out);
	}
clientCleanup:
	fclose(out);
	fclose(in);
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
