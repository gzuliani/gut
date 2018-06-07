#include "gut.h"
#include <stdexcept>

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

class NonCopiableObject {
	int id_;
	NonCopiableObject(const NonCopiableObject& co);
	NonCopiableObject& operator=(const NonCopiableObject& co);
public:
	NonCopiableObject(int id) : id_(id) { }
	bool operator==(const NonCopiableObject& co) const { return id_ == co.id_; }
	int GetId() const { return id_; }
};

std::ostream& operator<<(std::ostream& os, const NonCopiableObject& nco) {
	return os << "NonCopiableObject#" << nco.GetId();
}

class NonSerializableObject {
	int m_id;
public:
	NonSerializableObject(int id) : m_id(id) { }
	bool operator==(const NonSerializableObject& co) const { return m_id == co.m_id; }
};

bool isOdd(int i) {
	return i % 2;
}

void fnThatNotThrows() {
}

int fnThatThrowsARuntimeError() {
  throw std::runtime_error("a runtime error");
}

int fnThatThrowsAnInt() {
  throw 42;
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
	CHECK(b1 == false);
	CHECK(true == b2);
	CHECK(true == false);

	// other int types
	unsigned short us = 3;
	unsigned long ul = 4;
	CHECK(us == ul);
	long long ll1 = 23612343;
	long long ll2 = 876543445676;
	CHECK(ll1 == ll2);

	// characters
	char c1 = 'o';
	unsigned char uc2 = 0x05;
	CHECK(c1 == uc2);

	// short vs. unsigned char
	short ss1 = 6;
	CHECK(ss1 == uc2);
	CHECK(uc2 == ss1);

	// short vs. unsigned short
	CHECK(ss1 == us);
	CHECK(us == ss1);

	// short vs. unsigned int
	unsigned int ui = 7;
	CHECK(ss1 == ui);
	CHECK(ui == ss1);

	// short vs. unsigned long
	CHECK(ss1 == ul);
	CHECK(ul == ss1);

	// int vs. unsigned char
	CHECK(i1 == uc2);
	CHECK(uc2 == i1);

	// int vs. unsigned short
	CHECK(i1 == us);
	CHECK(us == i1);

	// int vs. unsigned int
	CHECK(i1 == ui);
	CHECK(ui == i1);

	// int vs. unsigned long
	CHECK(ul == i1);
	CHECK(i1 == ul);

	// long vs. unsigned char
	long l1 = 8;
	CHECK(l1 == uc2);
	CHECK(uc2 == l1);

	// long vs. unsigned short
	CHECK(l1 == us);
	CHECK(us == l1);

	// long vs. unsigned int
	CHECK(l1 == ui);
	CHECK(ui == l1);

	// long vs. unsigned long
	CHECK(l1 == ul);
	CHECK(ul == l1);

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

	NonCopiableObject nco1(1);
	NonCopiableObject nco2(2);
	CHECK(nco1 == nco2);
	CHECK(nco1 == NonCopiableObject(2));
	CHECK(NonCopiableObject(1) == nco2);
	CHECK(NonCopiableObject(1) == NonCopiableObject(2));

	NonSerializableObject nso1(1);
	NonSerializableObject nso2(2);
	CHECK(nso1 == nso2);
	CHECK(nso1 == NonSerializableObject(2));
	CHECK(NonSerializableObject(1) == nso2);
	CHECK(NonSerializableObject(1) == NonSerializableObject(2));

	// function arguments are not expanded
	CHECK(isOdd(i2));

	// other comparisons
	CHECK(i1 < i1);
	CHECK(i2 < i1);
	CHECK(i1 <= i1); // pass!
	CHECK(i2 <= i1);
	CHECK(i1 > i1);
	CHECK(i1 > i2);
	CHECK(i1 >= i1); // pass!
	CHECK(i1 >= i2);

	// signed/unsigned comparisons
	CHECK(ui < us);
	CHECK(uc2 <= i1);
	CHECK(i1 > 3L);
	CHECK(i1 >= 3UL);

	// comparing pointers
	int a[10];
	int* pa1 = a;
	int* pa2 = a + 3;
	CHECK(pa1 >= pa2);

	// exceptions
	try {
		// THROWS(f(), std::exception);
		THROWS(fnThatNotThrows(), std::runtime_error);
		THROWS(fnThatThrowsARuntimeError(), std::logic_error);
		THROWS(fnThatThrowsAnInt(), std::runtime_error);
		CHECK(fnThatThrowsARuntimeError() == 1);
		CHECK(2 == 1); // won't execute
	} catch(const std::exception& e) {
		std::cout << "unexpected exception \"" << e.what() << "\" caught" << std::endl;
	} catch(...) {
		std::cout << "unexpected unknown exception caught" << std::endl;
	}
	return 0;
}
