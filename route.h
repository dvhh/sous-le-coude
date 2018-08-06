#pragma once

static const struct Route routes[] = {
	{ "/", "text/html;static/index.html", 0, serveStatic },
	{ "/test", NULL, 0, serveTest },
	{ "/ip", NULL, 0, serveClientAddress },
	{ "*", NULL, 404, serveError }
};
