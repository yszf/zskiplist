#include "zskiplist.h"
#include <vld.h>


int main() {
	srand(time(NULL));

	zskiplist* zsl = zslCreate();
	zslInsert(zsl, 2.0, "a");
	zslInsert(zsl, 1.0, "b");
	zslInsert(zsl, 3.0, "c");
	zslInsert(zsl, 2.5, "d");
	zslInsert(zsl, 1.5, "e");
	zslFree(zsl);


	return 0;
}