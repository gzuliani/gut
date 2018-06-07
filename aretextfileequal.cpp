#include "gut.h"

#include "InputTextFile.h"

// tuples not available in VC10
template<class T, class U>
struct first_diff {
  T iter1;
  U iter2;
  size_t count;
  first_diff(const T& i1, const U& i2, size_t c) : iter1(i1), iter2(i2), count(c) { }
};

template<class T, class U>
first_diff<T, U> mismatch(T first1, T last1, U first2, U last2) {
  size_t count = 0;
  while ((first1 != last1) && (first2 != last2) && (*first1==*first2)) {
    ++count;
    ++first1;
    ++first2;
  }
  return first_diff<T, U>(first1, first2, count);
}

gut::Boolean AreTextFileEqual(const std::string& path1, const std::string& path2) {
  static const std::string fileIsExausted("<past-the-end-of-file>");

  InputTextFile f1(path1);

  if (!f1)
    return gut::Boolean(
      false,
      gut::TextFlow() << "false, \"" << path1 << "\" not found");

  InputTextFile f2(path2);

  if (!f2)
    return gut::Boolean(
      false,
      gut::TextFlow() << "false, \"" << path2 << "\" not found");

  const auto diff = mismatch(f1.begin(), f1.end(), f2.begin(), f2.end());
  const bool isFile1Exausted = (diff.iter1 == f1.end());
  const bool isFile2Exausted = (diff.iter2 == f2.end());

  if (isFile1Exausted && isFile2Exausted)
    return true;
  else
    return gut::Boolean(
      false,
      gut::TextFlow()
        << "false, files are different:\n"
        << " " << path1 << "(" << diff.count + 1 << ") : "
        << (isFile1Exausted ? fileIsExausted : *diff.iter1)
        << "\n"
        << " " << path2 << "(" << diff.count + 1 << ") : "
        << (isFile2Exausted ? fileIsExausted : *diff.iter2));
}

TEST {
	// comparing identical files
	CHECK(AreTextFileEqual("file1.txt", "file1.txt"));
	CHECK(AreTextFileEqual("file2.txt", "file2.txt"));

	// comparing different files
	CHECK(!AreTextFileEqual("file1.txt", "file1-partial.txt"));
	CHECK(!AreTextFileEqual("file1-partial.txt", "file1.txt"));
	CHECK(!AreTextFileEqual("file1.txt", "file1-without-trailing-empty-line.txt"));
	CHECK(!AreTextFileEqual("file1-without-trailing-empty-line.txt", "file1.txt"));
	CHECK(!AreTextFileEqual("file1.txt", "file2.txt"));
	CHECK(!AreTextFileEqual("file2.txt", "file1.txt"));

	// comparing an empty file against a non-empty one
	CHECK(!AreTextFileEqual("file1.txt", "empty.txt"));
	CHECK(!AreTextFileEqual("empty.txt", "file1.txt"));

	// comparing a file against a non-existent one
	CHECK(!AreTextFileEqual("file1.txt", "file_.txt"));
	CHECK(!AreTextFileEqual("file_.txt", "file1.txt"));
}
