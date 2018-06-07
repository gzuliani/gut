#include "gut.h"

bool f() {
	return false;
}

class Object {
	int m_id;
public:
	Object(int id) : m_id(id) { }
	Object(const Object& co) : m_id(co.m_id) { }
	Object& operator=(const Object& co) {
		m_id = co.m_id;
		return *this;
	}
	bool operator==(const Object& co) const { return m_id == co.m_id; }
	int GetId() const { return m_id; }
};

std::ostream& operator<<(std::ostream& os, const Object& co) {
	return os << "Object#" << co.GetId(); }

class NonSerializableObject {
	int m_id;
public:
	NonSerializableObject(int id) : m_id(id) { }
	bool operator==(const NonSerializableObject& co) const { return m_id == co.m_id; }
};

bool isOdd(int i) {
	return i % 2;
}

int main() {
	int i1 = 1;
	int i2 = 2;
	CHECK(i1 == i2);
	CHECK(i1 != 1);

	// boolean function
	CHECK(f());

	// strings
	std::string s1 = "1";
	std::string s2 = "2";
	CHECK(s1 == s2);
	CHECK(s1 == "2");
	CHECK("1" == s2);
	CHECK("1" == "2");

	// booleans
	bool b1 = true;
	bool b2 = false;
	CHECK(b1 == b2);

	// other int types
	unsigned short us = 7;
	unsigned long ul = 8;
	CHECK(us == ul);
	long long ll1 = 23612343;
	long long ll2 = 876543445676;
	CHECK(ll1 == ll2);

	// characters
	char c1 = 'o';
	unsigned char uc2 = 0x05;
	CHECK(c1 == uc2);

	// pointers
	int* pi1 = &i1;
	int* pi2 = &i2;
	CHECK(pi1 == pi2);
	CHECK(pi1 == &i2);
	CHECK(&i1 == pi2);
	CHECK(&i1 == &i2);
	const int* cpi1 = &i1;
	const int* cpi2 = &i2;
	CHECK(cpi1 == cpi2);
	CHECK(cpi1 == &i2);
	CHECK(&i1 == cpi2);
	CHECK(&i1 == &i2);
	CHECK(cpi1 == pi2);
	CHECK(pi1 == cpi2);
	CHECK(pi1 == 0);
	CHECK(pi1 == NULL);
	CHECK(&i1 == 0);
	CHECK(&i1 == NULL);
	int* pnull = NULL;
	CHECK(pnull != NULL);
	CHECK(pnull != 0);
	const int* cpnull = NULL;
	CHECK(pnull != cpnull);
	CHECK(cpnull != pnull);
	CHECK(cpi1 == 0);
	CHECK(cpi1 == NULL);
	CHECK(0 == pi1);
	CHECK(NULL == &i1);
	CHECK(0 != pnull);
	CHECK(NULL != pnull);
	CHECK(0 == cpi1);
	CHECK(NULL == cpi1);

	// objects
	Object o1(1);
	Object o2(2);
	CHECK(o1 == o2);
	CHECK(o1 == Object(2));
	CHECK(Object(1) == o2);
	CHECK(Object(1) == Object(2));

	NonSerializableObject nso1(1);
	NonSerializableObject nso2(2);
	CHECK(nso1 == nso2);
	CHECK(nso1 == NonSerializableObject(2));
	CHECK(NonSerializableObject(1) == nso2);
	CHECK(NonSerializableObject(1) == NonSerializableObject(2));

//	CHECK(i1 = i2); // should have been i1 == i2
//	CHECK(i1 + 1 == 1);

	// function arguments are not expanded
	int even = 2;
	CHECK(isOdd(even));
	return 0;
}
