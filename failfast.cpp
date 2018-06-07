#define GUT_FAILFAST
#include "gut.h"

TEST("fail fast /1") {
	CHECK(1 == 2);
	CHECK(2 == 3);
}

TEST("fail fast /2") {
	CHECK(1 == 2);
	CHECK(2 == 3);
}
