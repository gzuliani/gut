#include "gut.h"

GUT_ENABLE_FAILFAST

TEST("fail-fast /1") {
	CHECK(1 == 2);
	CHECK(2 == 3);
}

TEST("fail-fast /2") {
	CHECK(1 == 2);
	CHECK(2 == 3);
}
