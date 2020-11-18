#ifndef GUT_HIGHLIGHT_H
#define GUT_HIGHLIGHT_H

#include "utils.h"

namespace gut {

namespace highlight {

// enable/disable first diff highlighting in strings
struct HighlightFirstDiff_ {};
typedef StaticFlag<HighlightFirstDiff_> HighlightFirstDiff;

#define GUT_ENABLE_HIGHLIGHTFIRSTDIFF \
    gut::highlight::HighlightFirstDiff highlightFirstDiff_;

} // namespace highlight

} // namespace gut

#endif // GUT_HIGHLIGHT_H
