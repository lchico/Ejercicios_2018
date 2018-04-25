
#include "utils.h"

char * itoa_mp(int i)	// función para convertir de int a array de char
{
	char * res = malloc(8 * sizeof(int));
	sprintf(res, "%d", i);
	return res;
}
