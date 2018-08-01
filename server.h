#pragma once

struct Route {
	char *RequestLine,
	bool (*handler)(char *RequestLine,int socket,char* extra),
	char* extra
};