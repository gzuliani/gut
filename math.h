#ifndef GUT_MATH_H
#define GUT_MATH_H

#include "gut.h"
#include <cmath>

namespace gut {

namespace num {

gut::Boolean areAlmostEqual(double d1, double d2) {
    double diff = std::fabs(d1 - d2);
    return gut::Boolean(diff < 0.001, gut::TextFlow() << "|" << d1 << " - " << d2 << "| = " << diff << " < 0.001");
}

} // namespace num

} // namespace gut

#endif // GUT_MATH_H
