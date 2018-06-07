#include "gut.h"
#include "gut-tap.h"
#include <algorithm>
#include <stdexcept>
#include <vector>

GUT_ENABLE_REPORT(gut::TapReport)

class RecentlyUsedList {
	std::vector<std::string> items_;
public:
	bool empty() const {
		return items_.empty();
	}
	size_t size() const {
		return items_.size();
	}
	std::string operator[](size_t index) const {
		if (index >= size())
			throw std::out_of_range("invalid subscript");
		return items_[size() - index - 1];
	}
	void insert(const std::string& item) {
		auto found = std::find(begin(items_), end(items_), item);
		if (found != items_.end())
			items_.erase(found);
		items_.push_back(item);
	}
};

TEST("Initial list is empty") {
	RecentlyUsedList anEmptyList;

	CHECK(anEmptyList.empty());
	CHECK(anEmptyList.size() == 0);
}

TEST("Insertion to empty list is retained") {
	RecentlyUsedList aListWithOneElement;
	aListWithOneElement.insert("one");

	CHECK(!aListWithOneElement.empty());
	CHECK(aListWithOneElement.size() == 1);
	CHECK(aListWithOneElement[0] == "one");
}

TEST("Distinct insertions are retained in stack order") {
	RecentlyUsedList aListWithManyElements;
	aListWithManyElements.insert("one");
	aListWithManyElements.insert("two");
	aListWithManyElements.insert("three");

	CHECK(!aListWithManyElements.empty());
	CHECK(aListWithManyElements.size() == 3);
	CHECK(aListWithManyElements[0] == "three");
	REQUIRE(aListWithManyElements[1] == "two");
	CHECK(aListWithManyElements[2] == "one");
}

TEST("Duplicate insertions are moved to the front but not inserted") {
	RecentlyUsedList aListWithDuplicatedElements;
	aListWithDuplicatedElements.insert("one");
	aListWithDuplicatedElements.insert("two");
	aListWithDuplicatedElements.insert("three");
	aListWithDuplicatedElements.insert("two");

	CHECK(!aListWithDuplicatedElements.empty());
	CHECK(aListWithDuplicatedElements.size() == 3);
	CHECK(aListWithDuplicatedElements[0] == "two");
	CHECK(aListWithDuplicatedElements[1] == "three");
	CHECK(aListWithDuplicatedElements[2] == "one");
}

TEST("Out of range indexing throws exception") {
	RecentlyUsedList aListWithOneElement;
	aListWithOneElement.insert("one");

	THROWS(aListWithOneElement[1], std::out_of_range);
}

TODO("Empty strings are ignored", "not yet implemented") {
	RecentlyUsedList aList;
	aList.insert("");

	CHECK(aList.empty());
}

SKIP("One trillion insertions are ok", "too slow!") {
	RecentlyUsedList aList;
	for (int i = 0; i < 1000000; ++i)
		for (int j = 0; j < 1000000; ++j)
			for (int k = 0; k < 1000000; ++k)
				aList.insert("one");

	CHECK(aList.size() == 1);
	CHECK(aList[0] == "one");
}

TEST("Write to a file") {
	BAIL_OUT("File system is read only.");
}