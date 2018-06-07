#ifndef GUT_H
#define GUT_H

#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#if defined __GNUC__ && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 6))
#pragma GCC system_header
#endif

// Visual Studio 2012 support
#if defined _MSC_VER && (_MSC_VER <= 1700)

    #define GUT_DELETE
    #define GUT_EXPLICIT

#else

    #define GUT_DELETE = delete
    #define GUT_EXPLICIT explicit

#endif

#include "colors.h"
#include "debugger.h"
#include "timing.h"

#define GUT_INT_BASE Dec
#define GUT_CHAR_BASE Hex

#define GUT_CONCAT_(a, b) a ## b
#define GUT_CONCAT(a, b)  GUT_CONCAT_(a, b)

#define GUT_INT_TO_STRING  GUT_CONCAT(as, GUT_INT_BASE)
#define GUT_CHAR_TO_STRING GUT_CONCAT(as, GUT_CHAR_BASE)

namespace gut {

std::string asDec(long long value, const char* suffix) {
    std::ostringstream os;
    os << value << suffix;
    return os.str();
}

template<class T>
std::string asDec(const T& value) {
    return asDec(value, "");
}

std::string asDec(unsigned value) {
    return asDec(value, "u");
}

std::string asDec(long value) {
    return asDec(value, "l");
}

std::string asDec(unsigned long value) {
    return asDec(value, "ul");
}

std::string asDec(long long value) {
    return asDec(value, "ll");
}

std::string asDec(unsigned long long value) {
    return asDec(value, "ull");
}

std::string asHex(long long value, int width) {
    std::ostringstream os;
    os << "0x" << std::hex << std::setw(width) << std::setfill('0') << value;
    return os.str();
}

std::string asHex(char value) {
    return asHex(static_cast<int>(value), 2);
}

template<class T>
std::string asHex(const T& value) {
    return asHex(value, sizeof(T) * 2);
}

template<class T>
std::string intToString(const T& value) {
    return GUT_INT_TO_STRING(value);
}

template<class T>
std::string toString(const T& value);

std::string toString(const std::string& value) {
    return std::string("\"") + value + "\"";
}

std::string toString(const char* value) {
    return std::string("\"") + value + "\"";
}

std::string toString(char value) {
    std::ostringstream os;
    os << "'" << value << "' " << GUT_CHAR_TO_STRING(value);
    return os.str();
}

std::string toString(unsigned char value) {
    std::ostringstream os;
    os << GUT_CHAR_TO_STRING(value);
    return os.str();
}

std::string toString(short value) {
    return intToString(value);
}

std::string toString(int value) {
    return intToString(value);
}

std::string toString(long value) {
    return intToString(value);
}

std::string toString(long long value) {
    return intToString(value);
}

std::string toString(unsigned short value) {
    return intToString(value);
}

std::string toString(unsigned int value) {
    return intToString(value);
}

std::string toString(unsigned long value) {
    return intToString(value);
}

std::string toString(unsigned long long value) {
    return intToString(value);
}

std::string toString(std::nullptr_t) {
    return "<nullptr>";
}

template<class T>
class HasOperatorString {
    typedef char yes[1];
    typedef char no [2];

    struct Base {
        operator std::string() const;
    };

    struct Derived : T, Base {};

    template<class U, U u>
    struct Check {};

    template<class U>
    static no& check_(
        Check<std::string (Base::*)() const, &U::operator std::string>*);

    template<class U>
    static yes& check_(...);

public:
    static const bool value = sizeof(yes) == sizeof(check_<Derived>(0));
};

template<>
class HasOperatorString<bool> {
public:
    static const bool value = false;
};

template<class T, bool HasOperatorString = true>
struct StringRepr {
    std::string repr;
    StringRepr(const T& item) : repr(static_cast<std::string>(item)) {}
    std::string str() const { return repr; }
};

template<class T>
struct StringRepr<T, false> {
    StringRepr(const T& /*item*/) {}
    static std::string str() { return "<?>"; }
};

struct Expression {
    static std::string last;
    virtual ~Expression() {}
    Expression& operator=(const Expression&) GUT_DELETE;
    virtual bool evaluate() const = 0;
    virtual std::string toString() const = 0;
    bool logAndEvaluate() {
        Expression::last = toString();
        return evaluate();
    }
};

std::string Expression::last;

template<class T>
class UnaryExpression : public Expression {
protected:
    const T& value_;
public:
    UnaryExpression(const T& value) : value_(value) {}
    virtual bool evaluate() const { return static_cast<bool>(value_); }
    virtual std::string toString() const {
#ifdef _MSC_VER
__pragma(warning(push))
__pragma(warning(disable:4127))
#endif
        if (HasOperatorString<T>::value)
#ifdef _MSC_VER
__pragma(warning(pop))
#endif
            return StringRepr<T, HasOperatorString<T>::value>(value_).str();
        else
            return gut::toString(static_cast<bool>(value_));
    }
};

template<class T, class U>
class BinaryExpression : public Expression {
protected:
    const T& lhs_;
    const U& rhs_;
public:
    BinaryExpression(const T& lhs, const U& rhs) : lhs_(lhs), rhs_(rhs) {}
    virtual std::string toString() const {
        using gut::toString;
        return toString(lhs_)
            + " "
            + getOpName()
            + " "
            + toString(rhs_);
    }
    virtual std::string getOpName() const = 0;
};

template<class T, class U>
struct Equal : public BinaryExpression<T, U> {
    Equal(const T& lhs, const U& rhs) : BinaryExpression<T, U>(lhs, rhs) {}
    virtual bool evaluate() const { return this->lhs_ == this->rhs_; }
    virtual std::string getOpName() const { return "=="; }
};

template<class T, class U>
struct NotEqual : public BinaryExpression<T, U> {
    NotEqual(const T& lhs, const U& rhs)
        : BinaryExpression<T, U>(lhs, rhs) {}
    virtual bool evaluate() const { return this->lhs_ != this->rhs_; }
    virtual std::string getOpName() const { return "!="; }
};

template<class T, class U>
struct LessThan : public BinaryExpression<T, U> {
    LessThan(const T& lhs, const U& rhs)
        : BinaryExpression<T, U>(lhs, rhs) {}
    virtual bool evaluate() const { return this->lhs_ < this->rhs_; }
    virtual std::string getOpName() const { return "<"; }
};

template<class T, class U>
struct LessThanOrEqual : public BinaryExpression<T, U> {
    LessThanOrEqual(const T& lhs, const U& rhs)
        : BinaryExpression<T, U>(lhs, rhs) {}
    virtual bool evaluate() const { return this->lhs_ <= this->rhs_; }
    virtual std::string getOpName() const { return "<="; }
};

template<class T, class U>
struct GreaterThan : public BinaryExpression<T, U> {
    GreaterThan(const T& lhs, const U& rhs)
        : BinaryExpression<T, U>(lhs, rhs) {}
    virtual bool evaluate() const { return this->lhs_ > this->rhs_; }
    virtual std::string getOpName() const { return ">"; }
};

template<class T, class U>
struct GreaterThanOrEqual : public BinaryExpression<T, U> {
    GreaterThanOrEqual(const T& lhs, const U& rhs)
        : BinaryExpression<T, U>(lhs, rhs) {}
    virtual bool evaluate() const { return this->lhs_ >= this->rhs_; }
    virtual std::string getOpName() const { return ">="; }
};

enum Operator {
    e_equal,
    e_notEqual,
    e_lessThan,
    e_lessThanOrEqual,
    e_greaterThan,
    e_greaterThanOrEqual,
};

struct UNEXPECTED_ASSIGNMENT;
struct OPERATION_NOT_SUPPORTED;

template<class T, class U, Operator op>
struct ExprFactory {
    static OPERATION_NOT_SUPPORTED logAndEvaluate(const T&, const U&);
};

template<class T, class U>
struct ExprFactory<T, U, e_equal> {
    static bool logAndEvaluate(const T& lhs, const U& rhs) {
        return Equal<T, U>(lhs, rhs).logAndEvaluate();
    }
};

template<class T, class U>
struct ExprFactory<T, U, e_notEqual> {
    static bool logAndEvaluate(const T& lhs, const U& rhs) {
        return NotEqual<T, U>(lhs, rhs).logAndEvaluate();
    }
};

template<class T, class U>
struct ExprFactory<T, U, e_lessThan> {
    static bool logAndEvaluate(const T& lhs, const U& rhs) {
        return LessThan<T, U>(lhs, rhs).logAndEvaluate();
    }
};

template<class T, class U>
struct ExprFactory<T, U, e_lessThanOrEqual> {
    static bool logAndEvaluate(const T& lhs, const U& rhs) {
        return LessThanOrEqual<T, U>(lhs, rhs).logAndEvaluate();
    }
};

template<class T, class U>
struct ExprFactory<T, U, e_greaterThan> {
    static bool logAndEvaluate(const T& lhs, const U& rhs) {
        return GreaterThan<T, U>(lhs, rhs).logAndEvaluate();
    }
};

template<class T, class U>
struct ExprFactory<T, U, e_greaterThanOrEqual> {
    static bool logAndEvaluate(const T& lhs, const U& rhs) {
        return GreaterThanOrEqual<T, U>(lhs, rhs).logAndEvaluate();
    }
};

template<Operator op, class T, class U>
bool compare(const T& lhs, const U& rhs) {
    return ExprFactory<T, U, op>::logAndEvaluate(lhs, rhs);
}

template<Operator op>
bool compare(short lhs, unsigned int rhs) {
    return compare<op>(static_cast<unsigned int>(lhs), rhs);
}

template<Operator op>
bool compare(unsigned int lhs, short rhs) {
    return compare<op>(lhs, static_cast<unsigned int>(rhs));
}

template<Operator op>
bool compare(short lhs, unsigned long rhs) {
    return compare<op>(static_cast<unsigned long>(lhs), rhs);
}

template<Operator op>
bool compare(unsigned long lhs, short rhs) {
    return compare<op>(lhs, static_cast<unsigned long>(rhs));
}

template<Operator op>
bool compare(unsigned int lhs, int rhs) {
    return compare<op>(lhs, static_cast<unsigned int>(rhs));
}

template<Operator op>
bool compare(int lhs, unsigned int rhs) {
    return compare<op>(static_cast<unsigned int>(lhs), rhs);
}

template<Operator op>
bool compare(unsigned long lhs, int rhs) {
    return compare<op>(lhs, static_cast<unsigned long>(rhs));
}

template<Operator op>
bool compare(int lhs, unsigned long rhs) {
    return compare<op>(static_cast<unsigned long>(lhs), rhs);
}

template<Operator op>
bool compare(long lhs, unsigned int rhs) {
    return compare<op>(lhs, static_cast<long>(rhs));
}

template<Operator op>
bool compare(unsigned int lhs, long rhs) {
    return compare<op>(static_cast<long>(lhs), rhs);
}

template<Operator op>
bool compare(long lhs, unsigned long rhs) {
    return compare<op>(static_cast<unsigned long>(lhs), rhs);
}

template<Operator op>
bool compare(unsigned long lhs, long rhs) {
    return compare<op>(lhs, static_cast<unsigned long>(rhs));
}

template<Operator op>
bool compare(int lhs, unsigned long long rhs) {
    return compare<op>(static_cast<unsigned long long>(lhs), rhs);
}

template<Operator op>
bool compare(unsigned long long lhs, int rhs) {
    return compare<op>(lhs, static_cast<unsigned long long>(rhs));
}

template<Operator op, class T>
bool compare(T* lhs, int rhs) {
    return ExprFactory<T*, T*, op>::logAndEvaluate(
        lhs, reinterpret_cast<T*>(rhs));
}

template<Operator op, class T>
bool compare(T* lhs, long rhs) {
    return ExprFactory<T*, T*, op>::logAndEvaluate(
        lhs, reinterpret_cast<T*>(rhs));
}

template<Operator op, class T>
bool compare(T* lhs, long long rhs) {
    return ExprFactory<T*, T*, op>::logAndEvaluate(
        lhs, reinterpret_cast<T*>(rhs));
}

template<Operator op, class T>
bool compare(int lhs, T* rhs) {
    return ExprFactory<T*, T*, op>::logAndEvaluate(
        reinterpret_cast<T*>(lhs), rhs);
}

template<Operator op, class T>
bool compare(long lhs, T* rhs) {
    return ExprFactory<T*, T*, op>::logAndEvaluate(
        reinterpret_cast<T*>(lhs), rhs);
}

template<Operator op, class T>
bool compare(long long lhs, T* rhs) {
    return ExprFactory<T*, T*, op>::logAndEvaluate(
        reinterpret_cast<T*>(lhs), rhs);
}

template<Operator op, class T>
bool compare(T* lhs, std::nullptr_t rhs) {
    return ExprFactory<T*, std::nullptr_t, op>::logAndEvaluate(lhs, rhs);
}

template<Operator op, class T>
bool compare(std::nullptr_t lhs, T* rhs) {
    return ExprFactory<std::nullptr_t, T*, op>::logAndEvaluate(lhs, rhs);
}

template<class T>
class Term {
    const T& lhs_;
public:
    Term(const T& lhs) : lhs_(lhs) {}
    template<class U>
    bool operator==(const U& rhs) const {
        return compare<e_equal>(lhs_, rhs);
    }
    template<class U>
    bool operator!=(const U& rhs) const {
        return compare<e_notEqual>(lhs_, rhs);
    }
    template<class U>
    bool operator<(const U& rhs) const {
        return compare<e_lessThan>(lhs_, rhs);
    }
    template<class U>
    bool operator<=(const U& rhs) const {
        return compare<e_lessThanOrEqual>(lhs_, rhs);
    }
    template<class U>
    bool operator>(const U& rhs) const {
        return compare<e_greaterThan>(lhs_, rhs);
    }
    template<class U>
    bool operator>=(const U& rhs) const {
        return compare<e_greaterThanOrEqual>(lhs_, rhs);
    }
    operator bool() const {
        return UnaryExpression<T>(lhs_).logAndEvaluate();
    }
    Term& operator=(const Term&) GUT_DELETE;
    template<class U>
    UNEXPECTED_ASSIGNMENT operator=(const U&) const;
    OPERATION_NOT_SUPPORTED operator&&(const Term<T>&) const;
    OPERATION_NOT_SUPPORTED operator||(const Term<T>&) const;
    OPERATION_NOT_SUPPORTED operator+ (const Term<T>&) const;
    OPERATION_NOT_SUPPORTED operator- (const Term<T>&) const;
    OPERATION_NOT_SUPPORTED operator/ (const Term<T>&) const;
    OPERATION_NOT_SUPPORTED operator* (const Term<T>&) const;
    OPERATION_NOT_SUPPORTED operator% (const Term<T>&) const;
    OPERATION_NOT_SUPPORTED operator& (const Term<T>&) const;
    OPERATION_NOT_SUPPORTED operator| (const Term<T>&) const;
    OPERATION_NOT_SUPPORTED operator^ (const Term<T>&) const;
    OPERATION_NOT_SUPPORTED operator>>(int) const;
    OPERATION_NOT_SUPPORTED operator<<(int) const;
};

class Boolean {
    bool value_;
    mutable std::stringstream repr_;
public:
    Boolean(bool value) : value_(value) {}
    Boolean(const Boolean& other) : value_(other.value_) {
        repr_ << other.repr_.rdbuf();
    }
    GUT_EXPLICIT operator bool() const { return value_; }
    operator std::string() const {
        if (!repr_.rdbuf()->in_avail())
            repr_ << std::boolalpha << value_;
        return repr_.str();
    }
    template<class T>
    Boolean& operator<<(const T& item) {
        repr_ << item;
        return *this;
    }
};

struct NonStreamableTerm {
    template<class T>
    NonStreamableTerm(const T&) {}
};

std::ostream& operator<<(std::ostream& os, const NonStreamableTerm&) {
    return os << "{?}";
}

template<class T>
std::string toString(const T& value) {
    std::ostringstream os;
#if defined __GNUC__ && ((__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __GNUC__ > 4)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-promo"
#endif
    os << std::boolalpha << value;
#if defined __GNUC__ && ((__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __GNUC__ > 4)
#pragma GCC diagnostic pop
#endif
    return os.str();
}

class Capture {
public:
    template<class T>
    Term<T> operator->*(const T& term) { return Term<T>(term); }
};

struct AbortTest {};

class AbortSuite {
  std::string reason_;
public:
    AbortSuite() {}
    AbortSuite(const std::string& reason) : reason_(reason) {}
    const std::string& reason() const { return reason_; }
};

struct FailFast_ {};
typedef StaticFlag<FailFast_> FailFast;

#define GUT_ENABLE_FAILFAST gut::FailFast failFast_;

struct Location {
    const char* file;
    int line;
    Location(const char* file_, int line_) : file(file_), line(line_) {}
};

#define GUT_PICK_NAME(id_) e_ ## id_,
#define GUT_PICK_LABEL(id_) #id_,

#define GUT_LEVELS(lambda_) \
  lambda_(info) \
  lambda_(warning) \
  lambda_(error) \
  lambda_(fatal) \

enum Level { GUT_LEVELS(GUT_PICK_NAME) };
static std::string level_name[] = { GUT_LEVELS(GUT_PICK_LABEL) };

// no virtual methods here - see DefaultReport
class Notice {
    Level level_;
    Location location_;
    std::ostringstream content_;
public:
    Notice(Level level, const char* file, int line)
        : level_(level), location_(file, line) {
        content_ << "[" << level_name[level] << "] ";
    }
    Notice(const Notice& notice)
        : level_(notice.level_), location_(notice.location_) {
        content_ << notice.content_.str();
    }
    Location location() const {
        return location_;
    }
    Level level() const {
        return level_;
    }
    std::string what() const {
        return content_.str();
    }
protected:
    std::ostream& content() {
        return content_;
    }
};

struct Error : public Notice {
    Error(const char* file, int line) : Notice(e_error, file, line) {}
};

struct Fatal : public Notice {
    Fatal(const char* file, int line) : Notice(e_fatal, file, line) {}
};

struct CheckFailure : public Error {
    CheckFailure(
        const char* expression,
        const std::string& expansion,
        const char* file,
        int line) : Error(file, line) {
        content() << expression << " evaluates to " << expansion;
    }
};

struct RequireFailure : public Fatal {
    RequireFailure(
        const char* expression,
        const std::string& expansion,
        const char* file,
        int line) : Fatal(file, line) {
        content() << expression << " evaluates to " << expansion;
    }
};

struct NoThrowFailure : public Error {
    NoThrowFailure(
        const char* expression,
        const char* file,
        int line) : Error(file, line) {
        content() << expression << " did not throw";
    }
};

struct WrongTypedExceptionFailure : public Error {
    WrongTypedExceptionFailure(
        const char* expression,
        const std::exception& exception,
        const char* file,
        int line) : Error(file, line) {
        content()
            << expression
            << " threw an unexpected exception with message \""
            << exception.what()
            << "\"";
    }
};

struct WrongExceptionMessageFailure : public Error {
    WrongExceptionMessageFailure(
        const char* expression,
        const char* message,
        const char* expected,
        const char* file,
        int line) : Error(file, line) {
        content()
            << expression
            << " threw an exception with wrong message (expected \""
            << expected
            << "\", got \""
            << message
            << "\")";
    }
};

struct WrongExceptionFailure : public Error {
    WrongExceptionFailure(
        const char* expression,
        const char* file,
        int line) : Error(file, line) {
        content() << expression << " threw an unknown exception";
    }
};

struct UnexpectedExceptionFailure : public Error {
    UnexpectedExceptionFailure(
        const std::exception& exception,
        const char* file,
        int line) : Error(file, line) {
        content()
            << "unexpected exception with message \""
            << exception.what()
            << "\" caught";
    }
};

struct UnknownExceptionFailure : public Error {
    UnknownExceptionFailure(const char* file, int line) : Error(file, line) {
        content() << "unknown exception caught";
    }
};

struct FatalNoThrowFailure : public Fatal {
    FatalNoThrowFailure(const char* expression, const char* file, int line)
        : Fatal(file, line) {
        content() << expression << " did not throw";
    }
};

struct FatalWrongTypedExceptionFailure : public Fatal {
    FatalWrongTypedExceptionFailure(
        const char* expression,
        const std::exception& exception,
        const char* file,
        int line) : Fatal(file, line) {
        content()
            << expression
            << " threw an unexpected exception with message \""
            << exception.what()
            << "\"";
    }
};

struct FatalWrongExceptionMessageFailure : public Fatal {
    FatalWrongExceptionMessageFailure(
        const char* expression,
        const char* message,
        const char* expected,
        const char* file,
        int line) : Fatal(file, line) {
        content()
            << expression
            << " threw an exception with wrong message (expected \""
            << expected
            << "\", got \""
            << message
            << "\")";
    }
};

struct FatalWrongExceptionFailure : public Fatal {
    FatalWrongExceptionFailure(
        const char* expression,
        const char* file,
        int line) : Fatal(file, line) {
        content() << expression << " threw an unknown exception";
    }
};

struct FatalUnexpectedExceptionFailure : public Fatal {
    FatalUnexpectedExceptionFailure(
        const std::exception& exception,
        const char* file,
        int line) : Fatal(file, line) {
        content()
            << "unexpected exception with message \""
            << exception.what()
            << "\" caught";
    }
};

struct FatalUnknownExceptionFailure : public Fatal {
    FatalUnknownExceptionFailure(
        const char* file,
        int line) : Fatal(file, line) {
        content() << "unknown exception caught";
    }
};

struct DurationFailure : public Error {
    DurationFailure(
        const char* expression,
        const Duration& measured,
        const Duration& limit,
        const char* file,
        int line) : Error(file, line) {
        content()
            << expression
            << " took "
            << gut::toString(measured)
            << ", expected less than "
            << gut::toString(limit);
    }
};

struct FatalDurationFailure : public Fatal {
    FatalDurationFailure(
        const char* expression,
        const Duration& measured,
        const Duration& limit,
        const char* file,
        int line) : Fatal(file, line) {
        content()
            << expression
            << " took "
            << gut::toString(measured)
            << ", expected less than "
            << gut::toString(limit);
    }
};

struct Eval : public Notice {
    template<class T>
    Eval(const char* expr, const T& value, const char* file, int line)
        : Notice(e_info, file, line) {
        content() << expr << " evaluates to " << value;
    }
};

struct Info : public Notice {
    template<class T>
    Info(T message, const char* file, int line)
        : Notice(e_info, file, line) {
        content() << message;
    }
};

struct Warn : public Notice {
    template<class T>
    Warn(T message, const char* file, int line)
        : Notice(e_warning, file, line) {
        content() << message;
    }
};

struct UserFailure : public Fatal {
    template<class T>
    UserFailure(T message, const char* file, int line)
        : Fatal(file, line) {
        content() << message;
    }
};

typedef void (*TestFn)();

class Test {
    std::string name_;
    TestFn test_;
public:
    Test(const std::string& name, TestFn test) : name_(name), test_(test) {}
    const std::string& name() const { return name_; }
    void run() { test_(); }
};

class Suite {
    static std::vector<Test> tests_;
public:
    struct add {
        add(const std::string& name, TestFn test) {
            tests_.push_back(Test(name, test));
        }
    };
    static const std::vector<Test>& tests() { return tests_; }
};

std::vector<Test> Suite::tests_;

class Listener {
    int testCount_;
    int failedTestCount_;
    int totalFailureCount_;
    bool didTestFail_;
    Timer testTimer_;
    Timer globalTimer_;
public:
    template<class T>
    Listener(T report)
        : report_(std::make_shared<Model<T>>(std::move(report))) {}
    int failedTestCount() const { return failedTestCount_; }
    void start() {
        testCount_ = 0;
        failedTestCount_ = 0;
        totalFailureCount_ = 0;
        globalTimer_.reset();
        report_->start();
    }
    void end() {
        report_->end(
            testCount_,
            failedTestCount_,
            totalFailureCount_,
            globalTimer_.elapsedTime());
    }
    void startTest(const std::string& name) {
        ++testCount_;
        didTestFail_ = false;
        testTimer_.reset();
        report_->startTest(name);
    }
    void endTest() {
        if (didTestFail_)
            ++failedTestCount_;
        report_->endTest(didTestFail_, globalTimer_.elapsedTime());
    }
    void failure(const Notice& failure) {
        didTestFail_ = true;
        ++totalFailureCount_;
        report_->failure(
            failure.location().file,
            failure.location().line,
            failure.level(),
            failure.what());
        if (FailFast::enabled())
            throw AbortSuite();
    }
    void info(const Notice& info) {
        report_->info(
            info.location().file,
            info.location().line,
            info.level(),
            info.what());
    }
    void quit(const std::string& reason) {
        ++failedTestCount_;
        report_->quit(reason);
    }
private:
    struct Concept {
        virtual ~Concept() {}
        virtual void start() = 0;
        virtual void end(
            int /*tests*/,
            int /*failedTests*/,
            int /*failures*/,
            const Duration& /*duration*/) = 0;
        virtual void startTest(
            const std::string& /*name*/) = 0;
        virtual void endTest(
            bool /*failed*/,
            const Duration& /*duration*/) = 0;
        virtual void failure(
            const char* /*file*/,
            int /*line*/,
            int /*level*/,
            const std::string& /*what*/) = 0;
        virtual void info(
            const char* /*file*/,
            int /*line*/,
            int /*level*/,
            const std::string& /*what*/) = 0;
        virtual void quit(
            const std::string& /*reason*/) = 0;
    };

    template<class T>
    struct Model : public Concept {
        T report_;
        Model(T report) : report_(std::move(report)) {}
        virtual void start() override { report_.start(); }
        virtual void end(
            int tests,
            int failedTests,
            int failures,
            const Duration& duration) override {
            report_.end(tests, failedTests, failures, duration);
        }
        virtual void startTest(
            const std::string& name) override {
            report_.startTest(name);
        }
        virtual void endTest(
            bool failed,
            const Duration& duration) override {
            report_.endTest(failed, duration);
        }
        virtual void failure(
            const char* file,
            int line,
            int level,
            const std::string& what) override {
            report_.failure(file, line, level, what);
        }
        virtual void info(
            const char* file,
            int line,
            int level,
            const std::string& what) override {
            report_.info(file, line, level, what);
        }
        virtual void quit(const std::string& reason) override {
            report_.quit(reason);
        }
    };

  std::shared_ptr<Concept> report_;
};

class DefaultReport {
    std::ostream& os_;
    bool testAlreadyFailed_;
    std::vector<std::pair<int, std::string>> log_;
public:
    DefaultReport(std::ostream& os = std::cout)
     : os_(os), testAlreadyFailed_(false) {}
    DefaultReport& operator=(const DefaultReport&) GUT_DELETE;
    void start() { os_ << "Test suite started..." << std::endl; }
    void end(
        int tests,
        int failedTests,
        int failures,
        const Duration& duration) {
        os_ << "Ran "
            << tests
            << " test(s) in "
            << std::fixed << std::setprecision(0)
            << duration.seconds()
            << "s."
            << std::endl;
        if (failedTests == 0)
            os_ << color::lime
                << "OK - all tests passed."
                << color::reset
                << std::endl;
        else
            os_ << color::red
                << "FAILED - "
                << failures
                << " failure(s) in "
                << failedTests
                << " test(s)."
                << color::reset
                << std::endl;
    }
    void startTest(const std::string& name) {
        testAlreadyFailed_ = false;
        os_ << name << ": ";
    }
    void endTest(bool failed, const Duration& /*duration*/) {
        if (!failed) {
            os_ << "OK" << std::endl;
            flush(e_warning);
        }
        else
            flush(e_info);
        clear();
    }
    void failure(
        const char* file,
        int line,
        int level,
        const std::string& what) {
        if (!testAlreadyFailed_) {
            testAlreadyFailed_ = true;
            os_ << "FAILED" << std::endl;
        }
        append(file, line, level, what);
    }
    void info(
        const char* file,
        int line,
        int level,
        const std::string& what) {
        append(file, line, level, what);
    }
    void quit(const std::string& /*reason*/) {
        flush(e_info);
    }
protected:
    void append(
        const char* file,
        int line,
        int level,
        const std::string& what) {
        std::ostringstream oss;
        oss << file << "(" << line << ") : " << what;
        log_.push_back(std::make_pair(level, oss.str()));
    }
    void flush(Level minLevel) {
        for (auto entry : log_)
            if (entry.first >= minLevel)
                os_ << " " << entry.second << std::endl;
    }
    void clear() {
        log_.clear();
    }
};

Listener theListener = Listener(DefaultReport());

} // namespace gut

#define GUT_CUSTOM_REPORT(report_) \
    static struct ListenerWithCustomReport { \
        ListenerWithCustomReport() { \
            gut::theListener = gut::Listener(report_); \
        } \
    } aCustomListener_;

#define GUT_BEGIN \
    do {

#ifdef _MSC_VER

    #define GUT_END \
    __pragma(pack(push)) \
    __pragma(warning(disable:4127)) \
        } while (0) \
    __pragma(pack(pop))

#else

    #define GUT_END \
        } while (0)

#endif

#define CHECK(expr_) \
    GUT_BEGIN \
        if (!(gut::Capture()->*expr_)) { \
            GUT_DEBUG_BREAK \
            gut::theListener.failure( \
                gut::CheckFailure( \
                    #expr_, gut::Expression::last, __FILE__, __LINE__)); \
        } \
    GUT_END

#define THROWS_(expr_, exception_, prefix_, abort_) \
    GUT_BEGIN \
        bool catched_ = false; \
        try { \
            (void)(expr_); \
            GUT_DEBUG_BREAK \
            gut::theListener.failure( \
                gut::prefix_ ## NoThrowFailure( \
                    #expr_, __FILE__, __LINE__)); \
        } catch(const exception_&) { \
            catched_ = true; \
        } catch(const std::exception& e_) { \
            GUT_DEBUG_BREAK \
            gut::theListener.failure( \
                gut::prefix_ ## WrongTypedExceptionFailure( \
                    #expr_, e_, __FILE__, __LINE__)); \
        } catch(...) { \
            GUT_DEBUG_BREAK \
            gut::theListener.failure( \
                gut::prefix_ ## WrongExceptionFailure( \
                    #expr_, __FILE__, __LINE__)); \
        } \
        if (!catched_ && abort_) \
            throw gut::AbortTest(); \
    GUT_END

#define THROWS(expr_, exception_) \
    THROWS_(expr_, exception_, , false)

#define REQUIRE_THROWS(expr_, exception_) \
    THROWS_(expr_, exception_, Fatal, true)

#define REQUIRE(expr_) \
    GUT_BEGIN \
        if (!(gut::Capture()->*expr_)) { \
            GUT_DEBUG_BREAK \
            gut::theListener.failure( \
                gut::RequireFailure( \
                    #expr_, gut::Expression::last, __FILE__, __LINE__)); \
            throw gut::AbortTest(); \
        } \
    GUT_END

#define THROWS_ANYTHING(expr_) \
    GUT_BEGIN \
        try { \
            (void)(expr_); \
            GUT_DEBUG_BREAK \
            gut::theListener.failure( \
                gut::NoThrowFailure( \
                    #expr_, __FILE__, __LINE__)); \
        } catch(...) { \
        } \
    GUT_END

#define REQUIRE_THROWS_ANYTHING(expr_) \
    GUT_BEGIN \
        bool threw_ = false; \
        try { \
            (void)(expr_); \
        } catch(...) { \
            threw_ = true; \
        } \
        if (!threw_) { \
            GUT_DEBUG_BREAK \
            gut::theListener.failure( \
                gut::FatalNoThrowFailure( \
                    #expr_, __FILE__, __LINE__)); \
            throw gut::AbortTest(); \
        } \
    GUT_END

#define THROWS_WITH_MESSAGE_(expr_, exception_, what_, prefix_, abort_) \
    GUT_BEGIN \
        bool catched_ = false; \
        try { \
            (void)(expr_); \
            GUT_DEBUG_BREAK \
            gut::theListener.failure( \
                gut::prefix_ ## NoThrowFailure( \
                    #expr_, __FILE__, __LINE__)); \
        } catch(const exception_& e_) { \
            if (strcmp(e_.what(), static_cast<const char*>(what_)) != 0) { \
                GUT_DEBUG_BREAK \
                gut::theListener.failure( \
                    gut::prefix_ ## WrongExceptionMessageFailure( \
                        #expr_, e_.what(), what_, __FILE__, __LINE__)); \
            } else \
                catched_ = true; \
        } catch(const std::exception& e_) { \
            GUT_DEBUG_BREAK \
            gut::theListener.failure( \
                gut::prefix_ ## WrongTypedExceptionFailure( \
                    #expr_, e_, __FILE__, __LINE__)); \
        } catch(...) { \
            GUT_DEBUG_BREAK \
            gut::theListener.failure( \
                gut::prefix_ ## WrongExceptionFailure( \
                    #expr_, __FILE__, __LINE__)); \
        } \
        if (!catched_ && abort_) \
            throw gut::AbortTest(); \
    GUT_END

#define THROWS_WITH_MESSAGE(expr_, exception_, what_) \
    THROWS_WITH_MESSAGE_(expr_, exception_, what_, , false)

#define REQUIRE_THROWS_WITH_MESSAGE(expr_, exception_, what_) \
    THROWS_WITH_MESSAGE_(expr_, exception_, what_, Fatal, true)

#define THROWS_NOTHING(expr_) \
    GUT_BEGIN \
        try { \
            (void)(expr_); \
        } catch(const std::exception& e_) { \
            GUT_DEBUG_BREAK \
            gut::theListener.failure( \
                gut::UnexpectedExceptionFailure( \
                    e_, __FILE__, __LINE__)); \
        } catch(...) { \
            GUT_DEBUG_BREAK \
            gut::theListener.failure( \
                gut::UnknownExceptionFailure( \
                    __FILE__, __LINE__)); \
        } \
    GUT_END

#define REQUIRE_THROWS_NOTHING(expr_) \
    GUT_BEGIN \
        bool threw_ = true; \
        try { \
            (void)(expr_); \
            threw_ = false; \
        } catch(const std::exception& e_) { \
            GUT_DEBUG_BREAK \
            gut::theListener.failure( \
                gut::FatalUnexpectedExceptionFailure( \
                    e_, __FILE__, __LINE__)); \
        } catch(...) { \
            GUT_DEBUG_BREAK \
            gut::theListener.failure( \
                gut::FatalUnknownExceptionFailure( \
                    __FILE__, __LINE__)); \
        } \
        if (threw_) \
            throw gut::AbortTest(); \
    GUT_END

int runTests_() {
    gut::theListener.start();
    for (auto test : gut::Suite::tests()) {
        gut::theListener.startTest(test.name());
        try {
            test.run();
        } catch(const gut::AbortSuite& e) {
            gut::theListener.quit(e.reason());
            break;
        } catch(const gut::AbortTest&) {
        } catch(const std::exception& e_) {
            GUT_DEBUG_BREAK
            gut::theListener.failure(
                gut::FatalUnexpectedExceptionFailure(
                    e_, __FILE__, __LINE__));
        } catch(...) {
            GUT_DEBUG_BREAK
            gut::theListener.failure(
                gut::FatalUnknownExceptionFailure(
                    __FILE__, __LINE__));
        }
        gut::theListener.endTest();
    }
    gut::theListener.end();
    return gut::theListener.failedTestCount();
}

#ifndef GUT_CUSTOM_MAIN
int main() {
    return runTests_();
}
#endif

#define GUT_ID(prefix_) GUT_CONCAT(prefix_, __LINE__)

#define TEST(name_) \
    static void GUT_ID(test_)(); \
    gut::Suite::add GUT_ID(testAddition_)( \
        name_, &GUT_CONCAT(test_, __LINE__)); \
    static void GUT_ID(test_)()

#define EVAL(expr_) \
    GUT_BEGIN \
        gut::theListener.info( \
            gut::Eval(#expr_, expr_, __FILE__, __LINE__)); \
    GUT_END

#define INFO(message_) \
    GUT_BEGIN \
        gut::theListener.info( \
            gut::Info(message_, __FILE__, __LINE__)); \
    GUT_END

#define WARN(message_) \
    GUT_BEGIN \
        gut::theListener.info( \
            gut::Warn(message_, __FILE__, __LINE__)); \
    GUT_END

#define FAIL(message_) \
    GUT_BEGIN \
		GUT_DEBUG_BREAK \
        gut::theListener.failure( \
            gut::UserFailure(message_, __FILE__, __LINE__)); \
    GUT_END

#endif // GUT_H
