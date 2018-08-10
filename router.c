#include <stdio.h>

#include <fnmatch.h>

#include "router.h"

const struct Route* getRoute( const char* path, const struct Route routes[], const unsigned int routesCount ) {

	for( unsigned int i=0; i < routesCount; ++i ) {
		if( fnmatch( routes[i].pattern, path, 0 ) == 0 ) {
			fprintf(stderr, "route [%i] used [%s]\n", i, path);
			return &routes[i];//( verb, path, routes[i].extra, routes[i].flag, in, out);
		}
	}

	return NULL;
}
