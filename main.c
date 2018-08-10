#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <limits.h>

#include "router.h"
#include "serve/error.h"
#include "serve/static.h"
#include "serve/test.h"
#include "serve/clientAddress.h"

#ifndef LINE_MAX
#define LINE_MAX 2048
#endif

void handleClient ( const int clientFd, const struct Route routes[], const unsigned int routesCount ) {

	FILE *out = fdopen(clientFd, "w");
	FILE *in  = fdopen(clientFd, "r");
	// read first line

	char line[LINE_MAX];
	if( fgets( line, LINE_MAX, in ) == NULL ) {
		perror("fgets");
		exit( EXIT_FAILURE );
	}

	const size_t length = strnlen( line, LINE_MAX );
	if( line[length-1] != '\n' ) {
		serveError(NULL,NULL,NULL,414,in,out);
		exit( EXIT_FAILURE );
	}

	char* verb=line;
	char* path=NULL;
	char* proto=NULL;
	for( unsigned int i=0; i<length; ++i ) {
		if( line[i]==' ' ) {
			line[i] = 0;
			if( path == NULL ) {
				path = line + i + 1;
			}else{
				proto = line + i + 1;
				break;
			}
		}
	}

	// Too strict ?
	if( strcmp( "HTTP/1.1\r\n",proto) != 0 ) {
		serveError(NULL,NULL,NULL,505,in,out);
	}

	const struct Route *route = getRoute( path, routes, routesCount );

	if( route != NULL ) {
		route->handler( verb, path, route->extra, route->flag, in, out);
	} else {
		fprintf(stderr, "[%s] not handled, serving a 404\n", path);
		serveError(NULL,path,NULL,404,in,out);
	}

	fflush(out);
	fclose(in);
	fclose(out);
	exit( EXIT_SUCCESS );
}

void acceptClient ( const int serverSocket , const struct Route routes[], const unsigned int routeCount ) {
	struct sockaddr_in client_addr;
	socklen_t clientaddr_len = sizeof ( client_addr );

	const int client = accept( serverSocket, (struct sockaddr *)&client_addr, &clientaddr_len);

	if( client == -1 ) {
		perror(NULL);
		exit( EXIT_FAILURE );
	}

	union {
		unsigned long int l;
		struct {
			unsigned char b1;
			unsigned char b2;
			unsigned char b3;
			unsigned char b4;
		} b;
	} address;

	address.l = ntohl( client_addr.sin_addr.s_addr );

	fprintf(stderr, "client %d.%d.%d.%d\n", address.b.b4, address.b.b3, address.b.b2, address.b.b1 );

	const pid_t pid = fork();
	printf("%jd\n", (intmax_t) pid);

	if( pid == -1 ) {
		perror("pid");
		exit( EXIT_FAILURE );
	}
	if( pid > 0 ) {
		close( client );
		return;
	}
	return handleClient( client, routes, routeCount );
}

void serve ( const int port, const struct Route routes[], const unsigned int routeCount ) {

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
	fputs( "ready\n", stderr );
	while(1) {
		acceptClient( serverSocket, routes, routeCount );
	}
	close( serverSocket );
}


#include "route.h"

int main (int argc, char **argv) {

	if (argc != 2) {
		printf("%s [port-number]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	const int port = atoi( argv[1] );

	const unsigned int routeCount = sizeof( routes ) / sizeof( struct Route );
	serve( port, routes, routeCount );

	exit(EXIT_SUCCESS);
}
