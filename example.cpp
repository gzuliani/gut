#include "gut.h"
#include <algorithm>
#include <stdexcept>
#include <vector>

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

	// WARN("verify that...");
	// EVAL(anEmptyList.size());
	// INFO("after anEmptyList initialization");
	// CHECK(false);

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

// TEST("Clearing the list makes it empty") {
// 	FAIL("TODO!");
// }
