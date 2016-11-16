#include <stdlib.h>
#include "URL.h"

URL *initURL() {
	URL *url = (URL *)malloc(sizeof(URL));
	return url;
}