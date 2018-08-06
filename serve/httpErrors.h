#pragma once

static const struct HttpError httpErrors[] = {
	{ 200, "200 OK" },
	{ 400, "400 Bad Request"},
	{ 404, "404 Not Found" },
	{ 405, "405 Method Not Allowed" },
	{ 414, "414 URI Too Long" },
	{ 500, "500 Internal Server Error" },
	{ 505, "505 HTTP Version Not Supported" }
};
