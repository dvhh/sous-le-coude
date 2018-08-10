#pragma once

typedef void (handler_t)(const char*,const char*,const char*,const int,FILE*,FILE*);

struct Route {
	char* pattern;
	char* extra;
	int flag;
	handler_t *handler;
};

const struct Route* getRoute( const char* path, const struct Route routes[], const unsigned int routeCount );
