#include "gut.h"
#include <cmath>

const double pi = 3.141592653;

gut::Boolean AreAlmostEqual(double d1, double d2) {
	double diff = std::fabs(d1 - d2);
	return gut::Boolean(diff < 0.001, gut::TextFlow() << "|" << d1 << " - " << d2 << "| = " << diff << " >= 0.001");
}

TEST {
	CHECK(AreAlmostEqual(pi, 3.14)); // fails
	CHECK(AreAlmostEqual(pi, 3.141)); // succeeds
}
