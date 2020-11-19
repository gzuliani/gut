#ifndef GUT_HIGHLIGHT_H
#define GUT_HIGHLIGHT_H

#include "utils.h"

#include <algorithm>

namespace gut {

namespace highlight {

// enable highlighting the first mismatching characters in
// trings that do not compare equal in a `==` test expression
struct FirstDiffInStrings_ {};
typedef StaticFlag<FirstDiffInStrings_> FirstDiffInStrings;

#define GUT_ENABLE_HIGHLIGHTFIRSTDIFFINSTRINGS \
    gut::highlight::FirstDiffInStrings firstDiffInStrings_;

std::string firstDiffInStrings(
    const std::string& lhs, const std::string& rhs)
{
    if (!highlight::FirstDiffInStrings::enabled())
        return "";

    static const int prefixLength = 40;
    static const int suffixLength = 32;
    const auto diff = std::mismatch(
        lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    if (diff.first == lhs.end() && diff.second == rhs.end())
        return "";
    const int diffPos = static_cast<int>(
        (diff.first != lhs.end())
            ? diff.first - lhs.begin()
            : diff.second - rhs.begin());
    const auto fragmentStartPos = std::max(0, diffPos - prefixLength);
    const auto markerPos = std::min(diffPos, prefixLength);
    const auto fragmentLength = markerPos + suffixLength;

    std::ostringstream os;
    os
        << "\n"
        << "first difference found at index "
        << diffPos
        << ":\n"
        << lhs.substr(fragmentStartPos, fragmentLength)
        << "\n"
        << rhs.substr(fragmentStartPos, fragmentLength)
        << "\n"
        << std::string(markerPos, '-')
        << "^\n";
    return os.str();
}

} // namespace highlight

} // namespace gut

#endif // GUT_HIGHLIGHT_H
