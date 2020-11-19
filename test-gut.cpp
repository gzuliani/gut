#define GUT_CUSTOM_MAIN
#include "gut.h"

#include <cassert>
#include <stdexcept>

bool f() {
    return false;
}

class Object {
    int id_;
public:
    Object(int id) : id_(id) {}
    Object(const Object& co) : id_(co.id_) {}
    Object& operator=(const Object& co) {
        id_ = co.id_;
        return *this;
    }
    bool operator==(const Object& co) const { return id_ == co.id_; }
    int GetId() const { return id_; }
};

std::ostream& operator<<(std::ostream& os, const Object& co) {
    return os << "Object#" << co.GetId();
}

class NonCopiableObject {
    int id_;
    NonCopiableObject(const NonCopiableObject& co);
    NonCopiableObject& operator=(const NonCopiableObject& co);
public:
    NonCopiableObject(int id) : id_(id) {}
    bool operator==(const NonCopiableObject& co) const { return id_ == co.id_; }
    int GetId() const { return id_; }
};

std::ostream& operator<<(std::ostream& os, const NonCopiableObject& nco) {
    return os << "NonCopiableObject#" << nco.GetId();
}

class NonSerializableObject {
    int id_;
public:
    NonSerializableObject(int id) : id_(id) {}
    bool operator==(const NonSerializableObject& co) const {
        return id_ == co.id_;
    }
};

bool isOdd(int i) {
    return i % 2 == 1;
}

void fnThatNotThrows() {
}

int fnThatThrowsARuntimeError() {
    throw std::runtime_error("a runtime error");
}

int fnThatThrowsAnInt() {
    throw 42;
}

struct Point {
    int x_;
    int y_;
    Point(int x, int y) : x_(x), y_(y) {
        if (x < 0 || y < 0)
            throw std::runtime_error("point out of domain");
    }
    bool operator==(const Point& p) const { return x_ == p.x_ && y_ == p.y_; }
};

std::ostream& operator<<(std::ostream& os, const Point& pt) {
    return os << "(" << pt.x_ << ", " << pt.y_ << ")";
}

class TestReport {
    std::string& failure_;
    std::string& info_;
    // disabled!
    TestReport& operator=(const TestReport&);
public:
    TestReport(std::string& failure, std::string& info)
    : failure_(failure), info_(info) {}
    TestReport(const TestReport&) = default;
    void start() {}
    void end(
        int /*tests*/,
        int /*failedTests*/,
        int /*failures*/,
        const gut::Duration& /*duration*/) {}
    void startTest(const std::string& /*name*/) {}
    void endTest(bool /*failed*/, const gut::Duration& /*duration*/) {}
    void failure(
        const char* /*file*/,
        int /*line*/,
        int /*level*/,
        const std::string& what) { failure_ = what; }
    virtual void info(
        const char* /*file*/,
        int /*line*/,
        int /*level*/,
        const std::string& what) { info_ = what; }
    void quit(const std::string& /*reason*/) {}
};

class ConvertibleToBool {
    bool value_;
public:
    ConvertibleToBool(bool value) : value_(value) {}
    operator bool() const {
        return value_;
    }
};

class ConvertibleToBoolAndString {
    bool value_;
public:
    ConvertibleToBoolAndString(bool value) : value_(value) {}
    operator bool() const {
        return value_;
    }
    operator std::string() const {
        return value_ ? "\"YES\"" : "\"NO\"";
    }
};

class DerivedFromConvertibleToBoolAndString
 : public ConvertibleToBoolAndString {
public:
    DerivedFromConvertibleToBoolAndString(bool value)
     : ConvertibleToBoolAndString(value) {}
};

#define LEVELS(lambda_) \
    lambda_(low) \
    lambda_(medium) \
    lambda_(high)

#define LEVEL_ID(name_) e_ ## name_,
#define LEVEL_ENTRY(name_) { Level::e_ ## name_, "Level::e_" #name_ },

#include <map>

enum class Level {
    LEVELS(LEVEL_ID)
};
static std::map<Level, std::string> level_names = { LEVELS(LEVEL_ENTRY) };

std::ostream& operator<<(std::ostream& os, Level level) {
    os << level_names[level];
    return os;
}

int main() {

    std::string lastInfo;
    std::string lastFailure;
    gut::theListener = gut::Listener(TestReport(lastFailure, lastInfo));

    int i1 = 1;
    int i2 = 2;
    CHECK(i1 == i2);
    assert(lastFailure == "[error] i1 == i2 evaluates to 1 == 2");

    CHECK(i1 != 1);
    assert(lastFailure == "[error] i1 != 1 evaluates to 1 != 1");

    // boolean function
    CHECK(f());
    assert(lastFailure == "[error] f() evaluates to false");

    // strings
    std::string s1 = "1";
    std::string s2 = "2";

    CHECK(s1 == s2);
    assert(lastFailure == "[error] s1 == s2 evaluates to \"1\" == \"2\"");

    CHECK(s1 == "2");
    assert(lastFailure == "[error] s1 == \"2\" evaluates to \"1\" == \"2\"");

    CHECK("1" == s2);
    assert(lastFailure == "[error] \"1\" == s2 evaluates to \"1\" == \"2\"");

    CHECK("1" == "2");
    assert(lastFailure == "[error] \"1\" == \"2\" evaluates to \"1\" == \"2\"");

    // booleans
    bool b1 = true;
    bool b2 = false;

    CHECK(b1 == b2);
    assert(lastFailure == "[error] b1 == b2 evaluates to true == false");

    CHECK(b1 == false);
    assert(lastFailure == "[error] b1 == false evaluates to true == false");

    CHECK(true == b2);
    assert(lastFailure == "[error] true == b2 evaluates to true == false");

    CHECK(true == false);
    assert(lastFailure == "[error] true == false evaluates to true == false");

    // other int types
    unsigned short us = 3;
    unsigned long ul = 4;

    CHECK(us == ul);
    assert(lastFailure == "[error] us == ul evaluates to 3 == 4ul");

    long long ll1 = 23612343;
    long long ll2 = 876543445676;

    CHECK(ll1 == ll2);
    assert(lastFailure == "[error] ll1 == ll2 evaluates to 23612343ll == 876543445676ll");

    // characters
    char c1 = 'o';
    unsigned char uc2 = 0x05;

    CHECK(c1 == uc2);
    assert(lastFailure == "[error] c1 == uc2 evaluates to 'o' 0x6f == 0x05");

    // signed vs. unsigned

    // short vs. unsigned char
    short ss1 = 6;

    CHECK(ss1 == uc2);
    assert(lastFailure == "[error] ss1 == uc2 evaluates to 6 == 0x05");

    CHECK(uc2 == ss1);
    assert(lastFailure == "[error] uc2 == ss1 evaluates to 0x05 == 6");

    // short vs. unsigned short
    CHECK(ss1 == us);
    assert(lastFailure == "[error] ss1 == us evaluates to 6 == 3");

    CHECK(us == ss1);
    assert(lastFailure == "[error] us == ss1 evaluates to 3 == 6");

    // short vs. unsigned int
    unsigned int ui = 7;

    CHECK(ss1 == ui);
    assert(lastFailure == "[error] ss1 == ui evaluates to 6u == 7u");

    CHECK(ui == ss1);
    assert(lastFailure == "[error] ui == ss1 evaluates to 7u == 6u");

    // short vs. unsigned long
    CHECK(ss1 == ul);
    assert(lastFailure == "[error] ss1 == ul evaluates to 6ul == 4ul");

    CHECK(ul == ss1);
    assert(lastFailure == "[error] ul == ss1 evaluates to 4ul == 6ul");

    // int vs. unsigned char
    CHECK(i1 == uc2);
    assert(lastFailure == "[error] i1 == uc2 evaluates to 1 == 0x05");

    CHECK(uc2 == i1);
    assert(lastFailure == "[error] uc2 == i1 evaluates to 0x05 == 1");

    // int vs. unsigned short
    CHECK(i1 == us);
    assert(lastFailure == "[error] i1 == us evaluates to 1 == 3");

    CHECK(us == i1);
    assert(lastFailure == "[error] us == i1 evaluates to 3 == 1");

    // int vs. unsigned int
    CHECK(i1 == ui);
    assert(lastFailure == "[error] i1 == ui evaluates to 1u == 7u");

    CHECK(ui == i1);
    assert(lastFailure == "[error] ui == i1 evaluates to 7u == 1u");

    // int vs. unsigned long
    CHECK(ul == i1);
    assert(lastFailure == "[error] ul == i1 evaluates to 4ul == 1ul");

    CHECK(i1 == ul);
    assert(lastFailure == "[error] i1 == ul evaluates to 1ul == 4ul");

    // long vs. unsigned char
    long l1 = 8;

    CHECK(l1 == uc2);
    assert(lastFailure == "[error] l1 == uc2 evaluates to 8l == 0x05");

    CHECK(uc2 == l1);
    assert(lastFailure == "[error] uc2 == l1 evaluates to 0x05 == 8l");

    // long vs. unsigned short
    CHECK(l1 == us);
    assert(lastFailure == "[error] l1 == us evaluates to 8l == 3");

    CHECK(us == l1);
    assert(lastFailure == "[error] us == l1 evaluates to 3 == 8l");

    // long vs. unsigned int
    CHECK(l1 == ui);
    assert(lastFailure == "[error] l1 == ui evaluates to 8l == 7l");

    CHECK(ui == l1);
    assert(lastFailure == "[error] ui == l1 evaluates to 7l == 8l");

    // long vs. unsigned long
    CHECK(l1 == ul);
    assert(lastFailure == "[error] l1 == ul evaluates to 8ul == 4ul");

    CHECK(ul == l1);
    assert(lastFailure == "[error] ul == l1 evaluates to 4ul == 8ul");

    // pointers - what if the compiler allocates i1 and i2 elsewhere?
    int* pi1 = &i1;
    int* pi2 = &i2;

    CHECK(pi1 == pi2);
    //~ assert(lastFailure == "[error] pi1 == pi2 evaluates to 0012FF24 == 0012FF18");

    CHECK(pi1 == &i2);
    //~ assert(lastFailure == "[error] pi1 == &i2 evaluates to 0012FF24 == 0012FF18");

    CHECK(&i1 == pi2);
    //~ assert(lastFailure == "[error] &i1 == pi2 evaluates to 0012FF24 == 0012FF18");

    CHECK(&i1 == &i2);
    //~ assert(lastFailure == "[error] &i1 == &i2 evaluates to 0012FF24 == 0012FF18");

    const int* cpi1 = &i1;
    const int* cpi2 = &i2;

    CHECK(cpi1 == cpi2);
    //~ assert(lastFailure == "[error] cpi1 == cpi2 evaluates to 0012FF24 == 0012FF18");

    CHECK(cpi1 == &i2);
    //~ assert(lastFailure == "[error] cpi1 == &i2 evaluates to 0012FF24 == 0012FF18");

    CHECK(&i1 == cpi2);
    //~ assert(lastFailure == "[error] &i1 == cpi2 evaluates to 0012FF24 == 0012FF18");

    CHECK(&i1 == &i2);
    //~ assert(lastFailure == "[error] &i1 == &i2 evaluates to 0012FF24 == 0012FF18");

    CHECK(cpi1 == pi2);
    //~ assert(lastFailure == "[error] cpi1 == pi2 evaluates to 0012FF24 == 0012FF18");

    CHECK(pi1 == cpi2);
    //~ assert(lastFailure == "[error] pi1 == cpi2 evaluates to 0012FF24 == 0012FF18");

    CHECK(pi1 == 0);
    //~ assert(lastFailure == "[error] pi1 == 0 evaluates to 0012FF24 == 0");

    CHECK(pi1 == NULL);
    //~ assert(lastFailure == "[error] pi1 == NULL evaluates to 0012FF24 == 0");

    CHECK(&i1 == 0);
    //~ assert(lastFailure == "[error] &i1 == 0 evaluates to 0012FF24 == 0");

    CHECK(&i1 == NULL);
    //~ assert(lastFailure == "[error] &i1 == NULL evaluates to 0012FF24 == 0");

    int* pnull = NULL;

    CHECK(pnull != NULL);
    //~ assert(lastFailure == "[error] pnull != NULL evaluates to 00000000 != 0");

    CHECK(pnull != 0);
    //~ assert(lastFailure == "[error] pnull != 0 evaluates to 00000000 != 0");

    const int* cpnull = NULL;

    CHECK(pnull != cpnull);
    //~ assert(lastFailure == "[error] pnull != cpnull evaluates to 00000000 != 00000000");

    CHECK(cpnull != pnull);
    //~ assert(lastFailure == "[error] cpnull != pnull evaluates to 00000000 != 00000000");

    CHECK(cpi1 == 0);
    //~ assert(lastFailure == "[error] cpi1 == 0 evaluates to 0012FF24 == 0");

    CHECK(cpi1 == NULL);
    //~ assert(lastFailure == "[error] cpi1 == NULL evaluates to 0012FF24 == 0");

    CHECK(0 == pi1);
    //~ assert(lastFailure == "[error] 0 == pi1 evaluates to 0 == 0012FF24");

    CHECK(NULL == &i1);
    //~ assert(lastFailure == "[error] NULL == &i1 evaluates to 0 == 0012FF24");

    CHECK(0 != pnull);
    //~ assert(lastFailure == "[error] 0 != pnull evaluates to 0 != 00000000");

    CHECK(NULL != pnull);
    //~ assert(lastFailure == "[error] NULL != pnull evaluates to 0 != 00000000");

    CHECK(0 == cpi1);
    //~ assert(lastFailure == "[error] 0 == cpi1 evaluates to 0 == 0012FF24");

    CHECK(NULL == cpi1);
    //~ assert(lastFailure == "[error] NULL == cpi1 evaluates to 0 == 0012FF24");

    // nullptr
    CHECK(pi1 == nullptr);
    //~ assert(lastFailure == "[error] pi1 == nullptr evaluates to 0012FF24 == <nullptr>");

    CHECK(&i1 == nullptr);
    //~ assert(lastFailure == "[error] &i1 == nullptr evaluates to 0012FF24 == <nullptr>");

    int* pnullptr = nullptr;

    CHECK(pnullptr != NULL);
    //~ assert(lastFailure == "[error] pnullptr != NULL evaluates to 00000000 != 00000000");

    CHECK(pnullptr != 0);
    //~ assert(lastFailure == "[error] pnullptr != 0 evaluates to 00000000 != 00000000");

    CHECK(cpi1 == nullptr);
    //~ assert(lastFailure == "[error] cpi1 == nullptr evaluates to 0012FF24 == <nullptr>");

    CHECK(nullptr == &i1);
    //~ assert(lastFailure == "[error] nullptr == &i1 evaluates to <nullptr> == 0012FF24");

    CHECK(0 != pnullptr);
    //~ assert(lastFailure == "[error] 0 != pnullptr evaluates to 00000000 != 00000000");

    CHECK(NULL != pnullptr);
    //~ assert(lastFailure == "[error] NULL != pnullptr evaluates to 00000000 != 00000000");

    CHECK(nullptr == cpi1);
    //~ assert(lastFailure == "[error] nullptr == cpi1 evaluates to <nullptr> == 0012FF24");

    // objects
    Object o1(1);
    Object o2(2);

    CHECK(o1 == o2);
    assert(lastFailure == "[error] o1 == o2 evaluates to Object#1 == Object#2");

    CHECK(o1 == Object(2));
    assert(lastFailure == "[error] o1 == Object(2) evaluates to Object#1 == Object#2");

    CHECK(Object(1) == o2);
    assert(lastFailure == "[error] Object(1) == o2 evaluates to Object#1 == Object#2");

    CHECK(Object(1) == Object(2));
    assert(lastFailure == "[error] Object(1) == Object(2) evaluates to Object#1 == Object#2");

    NonCopiableObject nco1(1);
    NonCopiableObject nco2(2);

    CHECK(nco1 == nco2);
    assert(lastFailure == "[error] nco1 == nco2 evaluates to NonCopiableObject#1 == NonCopiableObject#2");

    CHECK(nco1 == NonCopiableObject(2));
    assert(lastFailure == "[error] nco1 == NonCopiableObject(2) evaluates to NonCopiableObject#1 == NonCopiableObject#2");

    CHECK(NonCopiableObject(1) == nco2);
    assert(lastFailure == "[error] NonCopiableObject(1) == nco2 evaluates to NonCopiableObject#1 == NonCopiableObject#2");

    CHECK(NonCopiableObject(1) == NonCopiableObject(2));
    assert(lastFailure == "[error] NonCopiableObject(1) == NonCopiableObject(2) evaluates to NonCopiableObject#1 == NonCopiableObject#2");

    NonSerializableObject nso1(1);
    NonSerializableObject nso2(2);

    CHECK(nso1 == nso2);
    assert(lastFailure == "[error] nso1 == nso2 evaluates to {?} == {?}");

    CHECK(nso1 == NonSerializableObject(2));
    assert(lastFailure == "[error] nso1 == NonSerializableObject(2) evaluates to {?} == {?}");

    CHECK(NonSerializableObject(1) == nso2);
    assert(lastFailure == "[error] NonSerializableObject(1) == nso2 evaluates to {?} == {?}");

    CHECK(NonSerializableObject(1) == NonSerializableObject(2));
    assert(lastFailure == "[error] NonSerializableObject(1) == NonSerializableObject(2) evaluates to {?} == {?}");

    // function arguments are not expanded
    CHECK(isOdd(i2));
    assert(lastFailure == "[error] isOdd(i2) evaluates to false");

    // other comparisons
    CHECK(i1 < i1);
    assert(lastFailure == "[error] i1 < i1 evaluates to 1 < 1");

    CHECK(i2 < i1);
    assert(lastFailure == "[error] i2 < i1 evaluates to 2 < 1");

    CHECK(i1 <= i1); // pass!
    assert(lastFailure == "[error] i2 < i1 evaluates to 2 < 1");

    CHECK(i2 <= i1);
    assert(lastFailure == "[error] i2 <= i1 evaluates to 2 <= 1");

    CHECK(i1 > i1);
    assert(lastFailure == "[error] i1 > i1 evaluates to 1 > 1");

    CHECK(i1 > i2);
    assert(lastFailure == "[error] i1 > i2 evaluates to 1 > 2");

    CHECK(i1 >= i1); // pass!
    assert(lastFailure == "[error] i1 > i2 evaluates to 1 > 2");

    CHECK(i1 >= i2);
    assert(lastFailure == "[error] i1 >= i2 evaluates to 1 >= 2");

    // signed/unsigned comparisons
    CHECK(ui < us);
    assert(lastFailure == "[error] ui < us evaluates to 7u < 3");

    CHECK(uc2 <= i1);
    assert(lastFailure == "[error] uc2 <= i1 evaluates to 0x05 <= 1");

    CHECK(i1 > 3L);
    assert(lastFailure == "[error] i1 > 3L evaluates to 1 > 3l");

    CHECK(i1 >= 3UL);
    assert(lastFailure == "[error] i1 >= 3UL evaluates to 1ul >= 3ul");

    // comparing pointers - what if the compiler allocates the array elsewhere?
    int a[10];
    int* pa1 = a;
    int* pa2 = a + 3;

    CHECK(pa1 >= pa2);
    //~ assert(lastFailure == "[error] pa1 >= pa2 evaluates to 0012FD7C >= 0012FD88");

    // convertible to bool
    CHECK(ConvertibleToBool(false));
    assert(lastFailure == "[error] ConvertibleToBool(false) evaluates to false");

    CHECK(!ConvertibleToBool(true));
    assert(lastFailure == "[error] !ConvertibleToBool(true) evaluates to false");

    // convertible to bool and string - conversion to std::string happens only on positive tests!
    CHECK(ConvertibleToBoolAndString(false));
    assert(lastFailure == "[error] ConvertibleToBoolAndString(false) evaluates to \"NO\"");

    CHECK(!ConvertibleToBoolAndString(true));
    assert(lastFailure == "[error] !ConvertibleToBoolAndString(true) evaluates to false");

    // derived from a convertible to bool and string
    CHECK(DerivedFromConvertibleToBoolAndString(false));
    assert(lastFailure == "[error] DerivedFromConvertibleToBoolAndString(false) evaluates to \"NO\"");

    CHECK(!DerivedFromConvertibleToBoolAndString(true));
    assert(lastFailure == "[error] !DerivedFromConvertibleToBoolAndString(true) evaluates to false");

    // exceptions
    try {
        THROWS(fnThatNotThrows(), std::runtime_error);
        assert(lastFailure == "[error] fnThatNotThrows() did not throw");

        THROWS(fnThatThrowsARuntimeError(), std::logic_error);
        assert(lastFailure == "[error] fnThatThrowsARuntimeError() threw an unexpected exception with message \"a runtime error\"");

        // THROWS(fnThatThrowsAnInt(), std::exception); does not compile!!!
        THROWS(fnThatThrowsAnInt(), std::runtime_error);
        assert(lastFailure == "[error] fnThatThrowsAnInt() threw an unknown exception");

        CHECK(fnThatThrowsARuntimeError() == 1);

        CHECK(2 == 1); // won't be executed
    } catch(const std::exception& e) {
        gut::theListener.failure(gut::UnexpectedExceptionFailure(e, __FILE__, __LINE__));
    } catch(...) {
        gut::theListener.failure(gut::UnknownExceptionFailure(__FILE__, __LINE__));
    }

    assert(lastFailure == "[error] unexpected exception with message \"a runtime error\" caught");

    // test EVAL
    assert(lastInfo == "");
    EVAL(i1);
    assert(lastInfo == "[info] i1 evaluates to 1");
    EVAL((i1 + 3 * i2));
    assert(lastInfo == "[info] (i1 + 3 * i2) evaluates to 7");

    // test INFO
    INFO("message #1");
    assert(lastInfo == "[info] message #1");
    INFO("message #2");
    assert(lastInfo == "[info] message #2");

    // test WARN
    WARN("message #1");
    assert(lastInfo == "[warning] message #1");
    WARN("message #2");
    assert(lastInfo == "[warning] message #2");

    // test FAIL
    FAIL("user failure");
    assert(lastFailure == "[fatal] user failure");

    // test exceptions /2
    THROWS_ANYTHING(fnThatNotThrows());
    assert(lastFailure == "[error] fnThatNotThrows() did not throw");

    THROWS_WITH_MESSAGE(fnThatThrowsARuntimeError(), std::runtime_error, "error");
    assert(lastFailure == "[error] fnThatThrowsARuntimeError() threw an exception with wrong message (expected \"error\", got \"a runtime error\")");

    THROWS_NOTHING(fnThatThrowsARuntimeError());
    assert(lastFailure == "[error] unexpected exception with message \"a runtime error\" caught");

    THROWS_NOTHING(fnThatThrowsAnInt());
    assert(lastFailure == "[error] unknown exception caught");

    Level l = Level::e_low;
    CHECK(l != Level::e_low);
    assert(lastFailure == "[error] l != Level::e_low evaluates to Level::e_low != Level::e_low");
    CHECK(l == Level::e_high);
    assert(lastFailure == "[error] l == Level::e_high evaluates to Level::e_low == Level::e_high");

    // initialization lists
    Point pt(1, 2);

    CHECK(pt == Point(1, 2));
    CHECK(pt == Point{1, 2});

    REQUIRE(pt == Point(1, 2));
    REQUIRE(pt == Point{1, 2});

    THROWS(Point{-1, 2}, std::runtime_error);
    THROWS_ANYTHING(Point{-1, 2});
    THROWS_WITH_MESSAGE(Point{-1, 2}, std::runtime_error, "point out of domain");
    THROWS_NOTHING(Point{1, 2});

    REQUIRE_THROWS(Point{-1, 2}, std::runtime_error);
    REQUIRE_THROWS_ANYTHING(Point{-1, 2});
    REQUIRE_THROWS_WITH_MESSAGE(Point{-1, 2}, std::runtime_error, "point out of domain");
    REQUIRE_THROWS_NOTHING(Point{1, 2});

    // highlighting the first mismatching characters in strings
    GUT_ENABLE_HIGHLIGHTFIRSTDIFFINSTRINGS

    const std::string s3("abcdefghijklmnopqrstuvwxyz");
    const std::string s4("*bcdefghijklXnopqrstuvwxyz");

    CHECK(s3 == s4);
    assert(lastFailure ==
        "[error] s3 == s4 evaluates to \"abcdefghijklmnopqrstuvwxyz\" == \"*bcdefghijklXnopqrstuvwxyz\"\n"
        "first difference found at index 0:\n"
        "abcdefghijklmnopqrstuvwxyz\n"
        "*bcdefghijklXnopqrstuvwxyz\n"
        "^\n");

    CHECK(s3 == "abc*efghijklmnopqrstuvwxyz");
    assert(lastFailure ==
        "[error] s3 == \"abc*efghijklmnopqrstuvwxyz\" evaluates to \"abcdefghijklmnopqrstuvwxyz\" == \"abc*efghijklmnopqrstuvwxyz\"\n"
        "first difference found at index 3:\n"
        "abcdefghijklmnopqrstuvwxyz\n"
        "abc*efghijklmnopqrstuvwxyz\n"
        "---^\n");

    CHECK("*bcdefghijklmnopqrstuvw*yz" == s4);
    assert(lastFailure ==
        "[error] \"*bcdefghijklmnopqrstuvw*yz\" == s4 evaluates to \"*bcdefghijklmnopqrstuvw*yz\" == \"*bcdefghijklXnopqrstuvwxyz\"\n"
        "first difference found at index 12:\n"
        "*bcdefghijklmnopqrstuvw*yz\n"
        "*bcdefghijklXnopqrstuvwxyz\n"
        "------------^\n");

    CHECK("abcdefghijklmnopqrstuvwxy*" == "abcdefghijklmnopqrstuvwxyz");
    assert(lastFailure ==
        "[error] \"abcdefghijklmnopqrstuvwxy*\" == \"abcdefghijklmnopqrstuvwxyz\" evaluates to \"abcdefghijklmnopqrstuvwxy*\" == \"abcdefghijklmnopqrstuvwxyz\"\n"
        "first difference found at index 25:\n"
        "abcdefghijklmnopqrstuvwxy*\n"
        "abcdefghijklmnopqrstuvwxyz\n"
        "-------------------------^\n");

    CHECK(s3 == std::string());
    assert(lastFailure ==
        "[error] s3 == std::string() evaluates to \"abcdefghijklmnopqrstuvwxyz\" == \"\"\n"
        "first difference found at index 0:\n"
        "abcdefghijklmnopqrstuvwxyz\n"
        "\n"
        "^\n");

    CHECK(std::string() == s4);
    assert(lastFailure ==
        "[error] std::string() == s4 evaluates to \"\" == \"*bcdefghijklXnopqrstuvwxyz\"\n"
        "first difference found at index 0:\n"
        "\n"
        "*bcdefghijklXnopqrstuvwxyz\n"
        "^\n");

    CHECK("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" == "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTU*WXYZ");
    assert(lastFailure ==
        "[error] \"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\" == \"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTU*WXYZ\" evaluates to \"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\" == \"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTU*WXYZ\"\n"
        "first difference found at index 47:\n"
        "hijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\n"
        "hijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTU*WXYZ\n"
        "----------------------------------------^\n");

    // non-string types don't get highlighting
    CHECK(i1 == i2);
    assert(lastFailure == "[error] i1 == i2 evaluates to 1 == 2");

    CHECK(b1 == b2);
    assert(lastFailure == "[error] b1 == b2 evaluates to true == false");

    CHECK(c1 == uc2);
    assert(lastFailure == "[error] c1 == uc2 evaluates to 'o' 0x6f == 0x05");

    CHECK(o1 == o2);
    assert(lastFailure == "[error] o1 == o2 evaluates to Object#1 == Object#2");

    return 0;
}
