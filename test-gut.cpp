#include "gut.h"
#include <cassert>
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

class LastFailure : public gut::Report {
	std::string& what_;
	// disabled!
	LastFailure& operator=(const LastFailure&);
public:
	LastFailure(std::string& what) : what_(what) {
	}
protected:
	virtual void onFailure(const gut::Failure& failure) {
		what_ = failure.what();
	}
};

int main() {

	std::string lastFailure;
	gut::Report::set(std::make_shared<LastFailure>(lastFailure));

	int i1 = 1;
	int i2 = 2;
	CHECK(i1 == i2);
	assert(lastFailure == "i1 == i2 evaluates to 1 == 2");

	CHECK(i1 != 1);
	assert(lastFailure == "i1 != 1 evaluates to 1 != 1");

	// boolean function
	CHECK(f());
	assert(lastFailure == "f() evaluates to false");

	// strings
	std::string s1 = "1";
	std::string s2 = "2";

	CHECK(s1 == s2);
	assert(lastFailure == "s1 == s2 evaluates to \"1\" == \"2\"");

	CHECK(s1 == "2");
	assert(lastFailure == "s1 == \"2\" evaluates to \"1\" == \"2\"");

	CHECK("1" == s2);
	assert(lastFailure == "\"1\" == s2 evaluates to \"1\" == \"2\"");

	CHECK("1" == "2");
	assert(lastFailure == "\"1\" == \"2\" evaluates to \"1\" == \"2\"");

	// booleans
	bool b1 = true;
	bool b2 = false;

	CHECK(b1 == b2);
	assert(lastFailure == "b1 == b2 evaluates to true == false");

	CHECK(b1 == false);
	assert(lastFailure == "b1 == false evaluates to true == false");

	CHECK(true == b2);
	assert(lastFailure == "true == b2 evaluates to true == false");

	CHECK(true == false);
	assert(lastFailure == "true == false evaluates to true == false");

	// other int types
	unsigned short us = 3;
	unsigned long ul = 4;

	CHECK(us == ul);
	assert(lastFailure == "us == ul evaluates to 3 == 4ul");

	long long ll1 = 23612343;
	long long ll2 = 876543445676;

	CHECK(ll1 == ll2);
	assert(lastFailure == "ll1 == ll2 evaluates to 23612343ll == 876543445676ll");

	// characters
	char c1 = 'o';
	unsigned char uc2 = 0x05;

	CHECK(c1 == uc2);
	assert(lastFailure == "c1 == uc2 evaluates to 'o' 0x6f == 0x05");

	// signed vs. unsigned

	// short vs. unsigned char
	short ss1 = 6;

	CHECK(ss1 == uc2);
	assert(lastFailure == "ss1 == uc2 evaluates to 6 == 0x05");

	CHECK(uc2 == ss1);
	assert(lastFailure == "uc2 == ss1 evaluates to 0x05 == 6");

	// short vs. unsigned short
	CHECK(ss1 == us);
	assert(lastFailure == "ss1 == us evaluates to 6 == 3");

	CHECK(us == ss1);
	assert(lastFailure == "us == ss1 evaluates to 3 == 6");

	// short vs. unsigned int
	unsigned int ui = 7;

	CHECK(ss1 == ui);
	assert(lastFailure == "ss1 == ui evaluates to 6u == 7u");

	CHECK(ui == ss1);
	assert(lastFailure == "ui == ss1 evaluates to 7u == 6u");

	// short vs. unsigned long
	CHECK(ss1 == ul);
	assert(lastFailure == "ss1 == ul evaluates to 6ul == 4ul");

	CHECK(ul == ss1);
	assert(lastFailure == "ul == ss1 evaluates to 4ul == 6ul");

	// int vs. unsigned char
	CHECK(i1 == uc2);
	assert(lastFailure == "i1 == uc2 evaluates to 1 == 0x05");

	CHECK(uc2 == i1);
	assert(lastFailure == "uc2 == i1 evaluates to 0x05 == 1");

	// int vs. unsigned short
	CHECK(i1 == us);
	assert(lastFailure == "i1 == us evaluates to 1 == 3");

	CHECK(us == i1);
	assert(lastFailure == "us == i1 evaluates to 3 == 1");

	// int vs. unsigned int
	CHECK(i1 == ui);
	assert(lastFailure == "i1 == ui evaluates to 1u == 7u");

	CHECK(ui == i1);
	assert(lastFailure == "ui == i1 evaluates to 7u == 1u");

	// int vs. unsigned long
	CHECK(ul == i1);
	assert(lastFailure == "ul == i1 evaluates to 4ul == 1ul");

	CHECK(i1 == ul);
	assert(lastFailure == "i1 == ul evaluates to 1ul == 4ul");

	// long vs. unsigned char
	long l1 = 8;

	CHECK(l1 == uc2);
	assert(lastFailure == "l1 == uc2 evaluates to 8l == 0x05");

	CHECK(uc2 == l1);
	assert(lastFailure == "uc2 == l1 evaluates to 0x05 == 8l");

	// long vs. unsigned short
	CHECK(l1 == us);
	assert(lastFailure == "l1 == us evaluates to 8l == 3");

	CHECK(us == l1);
	assert(lastFailure == "us == l1 evaluates to 3 == 8l");

	// long vs. unsigned int
	CHECK(l1 == ui);
	assert(lastFailure == "l1 == ui evaluates to 8l == 7l");

	CHECK(ui == l1);
	assert(lastFailure == "ui == l1 evaluates to 7l == 8l");

	// long vs. unsigned long
	CHECK(l1 == ul);
	assert(lastFailure == "l1 == ul evaluates to 8ul == 4ul");

	CHECK(ul == l1);
	assert(lastFailure == "ul == l1 evaluates to 4ul == 8ul");

	// pointers - what if the compiler allocates i1 and i2 elsewhere?
	int* pi1 = &i1;
	int* pi2 = &i2;

	CHECK(pi1 == pi2);
	//~ assert(lastFailure == "pi1 == pi2 evaluates to 0012FF24 == 0012FF18");

	CHECK(pi1 == &i2);
	//~ assert(lastFailure == "pi1 == &i2 evaluates to 0012FF24 == 0012FF18");

	CHECK(&i1 == pi2);
	//~ assert(lastFailure == "&i1 == pi2 evaluates to 0012FF24 == 0012FF18");

	CHECK(&i1 == &i2);
	//~ assert(lastFailure == "&i1 == &i2 evaluates to 0012FF24 == 0012FF18");

	const int* cpi1 = &i1;
	const int* cpi2 = &i2;

	CHECK(cpi1 == cpi2);
	//~ assert(lastFailure == "cpi1 == cpi2 evaluates to 0012FF24 == 0012FF18");

	CHECK(cpi1 == &i2);
	//~ assert(lastFailure == "cpi1 == &i2 evaluates to 0012FF24 == 0012FF18");

	CHECK(&i1 == cpi2);
	//~ assert(lastFailure == "&i1 == cpi2 evaluates to 0012FF24 == 0012FF18");

	CHECK(&i1 == &i2);
	//~ assert(lastFailure == "&i1 == &i2 evaluates to 0012FF24 == 0012FF18");

	CHECK(cpi1 == pi2);
	//~ assert(lastFailure == "cpi1 == pi2 evaluates to 0012FF24 == 0012FF18");

	CHECK(pi1 == cpi2);
	//~ assert(lastFailure == "pi1 == cpi2 evaluates to 0012FF24 == 0012FF18");

	CHECK(pi1 == 0);
	//~ assert(lastFailure == "pi1 == 0 evaluates to 0012FF24 == 0");

	CHECK(pi1 == NULL);
	//~ assert(lastFailure == "pi1 == NULL evaluates to 0012FF24 == 0");

	CHECK(&i1 == 0);
	//~ assert(lastFailure == "&i1 == 0 evaluates to 0012FF24 == 0");

	CHECK(&i1 == NULL);
	//~ assert(lastFailure == "&i1 == NULL evaluates to 0012FF24 == 0");

	int* pnull = NULL;

	CHECK(pnull != NULL);
	//~ assert(lastFailure == "pnull != NULL evaluates to 00000000 != 0");

	CHECK(pnull != 0);
	//~ assert(lastFailure == "pnull != 0 evaluates to 00000000 != 0");

	const int* cpnull = NULL;

	CHECK(pnull != cpnull);
	//~ assert(lastFailure == "pnull != cpnull evaluates to 00000000 != 00000000");

	CHECK(cpnull != pnull);
	//~ assert(lastFailure == "cpnull != pnull evaluates to 00000000 != 00000000");

	CHECK(cpi1 == 0);
	//~ assert(lastFailure == "cpi1 == 0 evaluates to 0012FF24 == 0");

	CHECK(cpi1 == NULL);
	//~ assert(lastFailure == "cpi1 == NULL evaluates to 0012FF24 == 0");

	CHECK(0 == pi1);
	//~ assert(lastFailure == "0 == pi1 evaluates to 0 == 0012FF24");

	CHECK(NULL == &i1);
	//~ assert(lastFailure == "NULL == &i1 evaluates to 0 == 0012FF24");

	CHECK(0 != pnull);
	//~ assert(lastFailure == "0 != pnull evaluates to 0 != 00000000");

	CHECK(NULL != pnull);
	//~ assert(lastFailure == "NULL != pnull evaluates to 0 != 00000000");

	CHECK(0 == cpi1);
	//~ assert(lastFailure == "0 == cpi1 evaluates to 0 == 0012FF24");

	CHECK(NULL == cpi1);
	//~ assert(lastFailure == "NULL == cpi1 evaluates to 0 == 0012FF24");

	// objects
	Object o1(1);
	Object o2(2);

	CHECK(o1 == o2);
	assert(lastFailure == "o1 == o2 evaluates to Object#1 == Object#2");

	CHECK(o1 == Object(2));
	assert(lastFailure == "o1 == Object(2) evaluates to Object#1 == Object#2");

	CHECK(Object(1) == o2);
	assert(lastFailure == "Object(1) == o2 evaluates to Object#1 == Object#2");

	CHECK(Object(1) == Object(2));
	assert(lastFailure == "Object(1) == Object(2) evaluates to Object#1 == Object#2");

	NonCopiableObject nco1(1);
	NonCopiableObject nco2(2);

	CHECK(nco1 == nco2);
	assert(lastFailure == "nco1 == nco2 evaluates to NonCopiableObject#1 == NonCopiableObject#2");

	CHECK(nco1 == NonCopiableObject(2));
	assert(lastFailure == "nco1 == NonCopiableObject(2) evaluates to NonCopiableObject#1 == NonCopiableObject#2");

	CHECK(NonCopiableObject(1) == nco2);
	assert(lastFailure == "NonCopiableObject(1) == nco2 evaluates to NonCopiableObject#1 == NonCopiableObject#2");

	CHECK(NonCopiableObject(1) == NonCopiableObject(2));
	assert(lastFailure == "NonCopiableObject(1) == NonCopiableObject(2) evaluates to NonCopiableObject#1 == NonCopiableObject#2");

	NonSerializableObject nso1(1);
	NonSerializableObject nso2(2);

	CHECK(nso1 == nso2);
	assert(lastFailure == "nso1 == nso2 evaluates to {?} == {?}");

	CHECK(nso1 == NonSerializableObject(2));
	assert(lastFailure == "nso1 == NonSerializableObject(2) evaluates to {?} == {?}");

	CHECK(NonSerializableObject(1) == nso2);
	assert(lastFailure == "NonSerializableObject(1) == nso2 evaluates to {?} == {?}");

	CHECK(NonSerializableObject(1) == NonSerializableObject(2));
	assert(lastFailure == "NonSerializableObject(1) == NonSerializableObject(2) evaluates to {?} == {?}");

	// function arguments are not expanded
	CHECK(isOdd(i2));
	assert(lastFailure == "isOdd(i2) evaluates to false");

	// other comparisons
	CHECK(i1 < i1);
	assert(lastFailure == "i1 < i1 evaluates to 1 < 1");

	CHECK(i2 < i1);
	assert(lastFailure == "i2 < i1 evaluates to 2 < 1");

	CHECK(i1 <= i1); // pass!
	assert(lastFailure == "i2 < i1 evaluates to 2 < 1");

	CHECK(i2 <= i1);
	assert(lastFailure == "i2 <= i1 evaluates to 2 <= 1");

	CHECK(i1 > i1);
	assert(lastFailure == "i1 > i1 evaluates to 1 > 1");

	CHECK(i1 > i2);
	assert(lastFailure == "i1 > i2 evaluates to 1 > 2");

	CHECK(i1 >= i1); // pass!
	assert(lastFailure == "i1 > i2 evaluates to 1 > 2");

	CHECK(i1 >= i2);
	assert(lastFailure == "i1 >= i2 evaluates to 1 >= 2");

	// signed/unsigned comparisons
	CHECK(ui < us);
	assert(lastFailure == "ui < us evaluates to 7u < 3");

	CHECK(uc2 <= i1);
	assert(lastFailure == "uc2 <= i1 evaluates to 0x05 <= 1");

	CHECK(i1 > 3L);
	assert(lastFailure == "i1 > 3L evaluates to 1 > 3l");

	CHECK(i1 >= 3UL);
	assert(lastFailure == "i1 >= 3UL evaluates to 1ul >= 3ul");

	// comparing pointers - what if the compiler allocates the array elsewhere?
	int a[10];
	int* pa1 = a;
	int* pa2 = a + 3;

	CHECK(pa1 >= pa2);
	//~ assert(lastFailure == "pa1 >= pa2 evaluates to 0012FD7C >= 0012FD88");

	// exceptions
	try {
		THROWS(fnThatNotThrows(), std::runtime_error);
		assert(lastFailure == "fnThatNotThrows() did not throw");

		THROWS(fnThatThrowsARuntimeError(), std::logic_error);
		assert(lastFailure == "fnThatThrowsARuntimeError() threw an unexpected exception \"a runtime error\"");

		// THROWS(fnThatThrowsAnInt(), std::exception); does not compile!!!
		THROWS(fnThatThrowsAnInt(), std::runtime_error);
		assert(lastFailure == "fnThatThrowsAnInt() threw an unknown exception");

		CHECK(fnThatThrowsARuntimeError() == 1);

		CHECK(2 == 1); // won't be executed
	} catch(const std::exception& e) {
		gut::Report::failure(gut::UnexpectedExceptionFailure(e, __FILE__, __LINE__));
	} catch(...) {
		gut::Report::failure(gut::UnknownExceptionFailure(__FILE__, __LINE__));
	}

	assert(lastFailure == "unexpected exception \"a runtime error\" caught");
	return 0;
}
