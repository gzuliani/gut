#ifndef GUT_H
#define GUT_H

#define GUT_HAS_CHRONO

#if (_MSC_VER<=1600) // VC10
#undef GUT_HAS_CHRONO
#endif

#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#ifdef GUT_HAS_CHRONO
#include <chrono>
#else
#include <time.h>
#endif

#include "colors.h"

#define INT_BASE Dec
#define CHAR_BASE Hex

#define CONCAT(a, b) a ## b
#define CONCAT_(a, b) CONCAT(a, b)
#define INT_TO_STRING CONCAT_(as, INT_BASE)
#define CHAR_TO_STRING CONCAT_(as, CHAR_BASE)

namespace gut {

std::string asDec(long long value, const char* suffix) {
    std::ostringstream os;
    os << value << suffix;
    return os.str();
}

template<typename T>
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

std::string asHex(long long value, size_t width) {
    std::ostringstream os;
    os << "0x" << std::hex << std::setw(width) << std::setfill('0') << value;
    return os.str();
}

template<typename T>
std::string asHex(const T& value) {
    return asHex(value, sizeof(T) * 2);
}

std::string asHex(char value) {
    return asHex(static_cast<int>(value), 2);
}

template<typename T>
std::string intToString(const T& value) {
    return INT_TO_STRING(value);
}

template<typename T>
std::string toString(const T& value) {
    std::ostringstream os;
    os << std::boolalpha << value;
    return os.str();
}

std::string toString(const std::string& value) {
    return std::string("\"") + value + "\"";
}

std::string toString(const char* value) {
    return std::string("\"") + value + "\"";
}

std::string toString(char value) {
    std::ostringstream os;
    os << "'" << value << "' " << CHAR_TO_STRING(value);
    return os.str();
}

std::string toString(unsigned char value) {
    std::ostringstream os;
    os << CHAR_TO_STRING(value);
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

template<typename T>
class HasOperatorString {
    typedef char yes[1];
    typedef char no [2];

    struct Base {
        operator std::string() const;
    };

    struct Derived : T, Base { };

    template<typename U, U u>
    struct Check { };

    template<typename U>
    static no& check_(Check<std::string (Base::*)() const, &U::operator std::string>*);

    template<typename U>
    static yes& check_(...);

public:
    static const bool value = sizeof(yes) == sizeof(check_<Derived>(0));
};

template<>
class HasOperatorString<bool> {
public:
    static const bool value = false;
};

template<typename T, bool HasOperatorString = true>
struct StringRepr {
    std::string repr;
    StringRepr(const T& item) : repr(static_cast<std::string>(item)) {
    }
    std::string str() const {
        return repr;
    }
};

template<typename T>
struct StringRepr<T, false> {
    StringRepr(const T& /*item*/) {
    }
    static std::string str() {
        return "<?>";
    }
};

struct Expression {
    static std::string last;
    virtual ~Expression() { }
    virtual bool evaluate() const = 0;
    virtual std::string toString() const = 0;
    bool logAndEvaluate() {
        Expression::last = toString();
        return evaluate();
    }
};

std::string Expression::last;

template<typename T>
class UnaryExpression : public Expression {
protected:
    const T& value_;
public:
    UnaryExpression(const T& value) : value_(value) { }
    virtual bool evaluate() const {
        return static_cast<bool>(value_);
    }
    virtual std::string toString() const {
        if (HasOperatorString<T>::value)
            return StringRepr<T, HasOperatorString<T>::value>(value_).str();
        else
            return gut::toString(static_cast<bool>(value_));
    }
};

template<typename T, typename U>
class BinaryExpression : public Expression {
protected:
    const T& lhs_;
    const U& rhs_;
public:
    BinaryExpression(const T& lhs, const U& rhs) : lhs_(lhs), rhs_(rhs) { }
    virtual std::string toString() const {
        return gut::toString(lhs_) + " " + getOpName() + " " + gut::toString(rhs_);
    }
    virtual std::string getOpName() const = 0;
};

template<typename T, typename U>
struct Equal : public BinaryExpression<T, U> {
    Equal(const T& lhs, const U& rhs) : BinaryExpression<T, U>(lhs, rhs) { }
    virtual bool evaluate() const { return this->lhs_ == this->rhs_; }
    virtual std::string getOpName() const { return "=="; }
};

template<typename T, typename U>
struct NotEqual : public BinaryExpression<T, U> {
    NotEqual(const T& lhs, const U& rhs) : BinaryExpression<T, U>(lhs, rhs) { }
    virtual bool evaluate() const { return this->lhs_ != this->rhs_; }
    virtual std::string getOpName() const { return "!="; }
};

template<typename T, typename U>
struct LessThan : public BinaryExpression<T, U> {
    LessThan(const T& lhs, const U& rhs) : BinaryExpression<T, U>(lhs, rhs) { }
    virtual bool evaluate() const { return this->lhs_ < this->rhs_; }
    virtual std::string getOpName() const { return "<"; }
};

template<typename T, typename U>
struct LessThanOrEqual : public BinaryExpression<T, U> {
    LessThanOrEqual(const T& lhs, const U& rhs) : BinaryExpression<T, U>(lhs, rhs) { }
    virtual bool evaluate() const { return this->lhs_ <= this->rhs_; }
    virtual std::string getOpName() const { return "<="; }
};

template<typename T, typename U>
struct GreaterThan : public BinaryExpression<T, U> {
    GreaterThan(const T& lhs, const U& rhs) : BinaryExpression<T, U>(lhs, rhs) { }
    virtual bool evaluate() const { return this->lhs_ > this->rhs_; }
    virtual std::string getOpName() const { return ">"; }
};

template<typename T, typename U>
struct GreaterThanOrEqual : public BinaryExpression<T, U> {
    GreaterThanOrEqual(const T& lhs, const U& rhs) : BinaryExpression<T, U>(lhs, rhs) { }
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

template<typename T, typename U, Operator op>
struct ExprFactory {
    static bool logAndEvaluate(const T& /*lhs*/, const U& /*rhs*/) {
        OPERATION_NOT_SUPPORTED dummy;
        return false;
    }
};

template<typename T, typename U>
struct ExprFactory<T, U, e_equal> {
    static bool logAndEvaluate(const T& lhs, const U& rhs) {
        return Equal<T, U>(lhs, rhs).logAndEvaluate();
    }
};

template<typename T, typename U>
struct ExprFactory<T, U, e_notEqual> {
    static bool logAndEvaluate(const T& lhs, const U& rhs) {
        return NotEqual<T, U>(lhs, rhs).logAndEvaluate();
    }
};

template<typename T, typename U>
struct ExprFactory<T, U, e_lessThan> {
    static bool logAndEvaluate(const T& lhs, const U& rhs) {
        return LessThan<T, U>(lhs, rhs).logAndEvaluate();
    }
};

template<typename T, typename U>
struct ExprFactory<T, U, e_lessThanOrEqual> {
    static bool logAndEvaluate(const T& lhs, const U& rhs) {
        return LessThanOrEqual<T, U>(lhs, rhs).logAndEvaluate();
    }
};

template<typename T, typename U>
struct ExprFactory<T, U, e_greaterThan> {
    static bool logAndEvaluate(const T& lhs, const U& rhs) {
        return GreaterThan<T, U>(lhs, rhs).logAndEvaluate();
    }
};

template<typename T, typename U>
struct ExprFactory<T, U, e_greaterThanOrEqual> {
    static bool logAndEvaluate(const T& lhs, const U& rhs) {
        return GreaterThanOrEqual<T, U>(lhs, rhs).logAndEvaluate();
    }
};

template<Operator op, typename T, typename U>
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

template<Operator op, typename T>
bool compare(T* lhs, int rhs) {
    return ExprFactory<T*, T*, op>::logAndEvaluate(lhs, reinterpret_cast<T*>(rhs));
}

template<Operator op, typename T>
bool compare(T* lhs, long long rhs) {
    return ExprFactory<T*, T*, op>::logAndEvaluate(lhs, reinterpret_cast<T*>(rhs));
}

template<Operator op, typename T>
bool compare(int lhs, T* rhs) {
    return ExprFactory<T*, T*, op>::logAndEvaluate(reinterpret_cast<T*>(lhs), rhs);
}

template<Operator op, typename T>
bool compare(long long lhs, T* rhs) {
    return ExprFactory<T*, T*, op>::logAndEvaluate(reinterpret_cast<T*>(lhs), rhs);
}

template<Operator op, typename T>
bool compare(T* lhs, std::nullptr_t rhs) {
    return ExprFactory<T*, std::nullptr_t, op>::logAndEvaluate(lhs, rhs);
}

template<Operator op, typename T>
bool compare(std::nullptr_t lhs, T* rhs) {
    return ExprFactory<std::nullptr_t, T*, op>::logAndEvaluate(lhs, rhs);
}

template<typename T>
class Term {
    const T& lhs_;
public:
    Term(const T& lhs) : lhs_(lhs) { }
    template<typename U>
    bool operator==(const U& rhs) const {
        return compare<e_equal>(lhs_, rhs);
    }
    template<typename U>
    bool operator!=(const U& rhs) const {
        return compare<e_notEqual>(lhs_, rhs);
    }
    template<typename U>
    bool operator<(const U& rhs) const {
        return compare<e_lessThan>(lhs_, rhs);
    }
    template<typename U>
    bool operator<=(const U& rhs) const {
        return compare<e_lessThanOrEqual>(lhs_, rhs);
    }
    template<typename U>
    bool operator>(const U& rhs) const {
        return compare<e_greaterThan>(lhs_, rhs);
    }
    template<typename U>
    bool operator>=(const U& rhs) const {
        return compare<e_greaterThanOrEqual>(lhs_, rhs);
    }
    operator bool() const {
        return UnaryExpression<T>(lhs_).logAndEvaluate();
    }
    template<typename U>
    UNEXPECTED_ASSIGNMENT operator=(const U& value) const;
    OPERATION_NOT_SUPPORTED operator&&(const Term<T>& term) const;
    OPERATION_NOT_SUPPORTED operator||(const Term<T>& term) const;
    OPERATION_NOT_SUPPORTED operator+ (const Term<T>& term) const;
    OPERATION_NOT_SUPPORTED operator- (const Term<T>& term) const;
    OPERATION_NOT_SUPPORTED operator/ (const Term<T>& term) const;
    OPERATION_NOT_SUPPORTED operator* (const Term<T>& term) const;
    OPERATION_NOT_SUPPORTED operator% (const Term<T>& term) const;
    OPERATION_NOT_SUPPORTED operator& (const Term<T>& term) const;
    OPERATION_NOT_SUPPORTED operator| (const Term<T>& term) const;
    OPERATION_NOT_SUPPORTED operator^ (const Term<T>& term) const;
    OPERATION_NOT_SUPPORTED operator>>(int term) const;
    OPERATION_NOT_SUPPORTED operator<<(int term) const;
};

class Boolean {
    bool value_;
    std::string repr_;
public:
    Boolean(bool value) : value_(value), repr_(gut::toString(value)) { }
    Boolean(bool value, const std::string& repr) : value_(value), repr_(repr) { }
    operator bool() const {
        return value_;
    }
    operator std::string() const {
        return repr_;
    }
};

class TextFlow {
    std::ostringstream oss_;
public:
    template <typename T>
    TextFlow& operator<<(const T& item) {
        oss_ << item;
        return *this;
    }
    operator std::string() const {
        return oss_.str();
    }
};

struct NonStreamableTerm {
    template<typename T>
    NonStreamableTerm(const T&) { }
};

std::ostream& operator<<(std::ostream& os, const NonStreamableTerm&) {
    return os << "{?}";
}

class Capture {
public:
    template<typename T>
    Term<T> operator->*(const T& term) {
        return Term<T>(term);
    }
};

struct Location {
    const char* file;
    int line;
    Location(const char* file_, int line_) : file(file_), line(line_) { }
};

struct AbortTest { };

class AbortSuite {
  std::string reason_;
public:
    AbortSuite() { }
    AbortSuite(const std::string& reason) : reason_(reason) { }
    const std::string& reason() const { return reason_; }
};

class FailFast {
    static bool enabled_;
public:
    FailFast() {
        enabled_ = true;
    }
    static bool enabled() {
        return enabled_;
    }
};

bool FailFast::enabled_ = false;

#define ENABLE_FAILFAST gut::FailFast failFast_;

#define PICK_NAME(id_) e_ ## id_,
#define PICK_LABEL(id_) #id_,

#define LEVELS(lambda_) \
  lambda_(info) \
  lambda_(warning) \
  lambda_(error) \
  lambda_(fatal) \

enum Level { LEVELS(PICK_NAME) };
static std::string level_name[] = { LEVELS(PICK_LABEL) };

// no virtual methods here - see DefaultReport.log_
class Notice {
    Level level_;
    Location location_;
    std::ostringstream content_;
public:
    Notice(Level level, const char* file, int line) : level_(level), location_(file, line) {
        content_ << "[" << level_name[level] << "] ";
    }
    Notice(const Notice& notice) : level_(notice.level_), location_(notice.location_) {
        content_ << notice.content_.str();
    }
    Level level() const {
        return level_;
    }
    std::string what() const {
        return content_.str();
    }
    std::string toString() const {
        std::ostringstream ss;
        ss << location_.file << "(" << location_.line << ") : " << what();
        return ss.str();
    }
protected:
    std::ostream& content() {
        return content_;
    }
};

struct Error : public Notice {
    Error(const char* file, int line) : Notice(e_error, file, line) { }
};

struct Fatal : public Notice {
    Fatal(const char* file, int line) : Notice(e_fatal, file, line) { }
};

struct CheckFailure : public Error {
    CheckFailure(const char* expression, const std::string& expansion, const char* file, int line) : Error(file, line) {
        content() << expression << " evaluates to " << expansion;
    }
};

struct RequireFailure : public Fatal {
    RequireFailure(const char* expression, const std::string& expansion, const char* file, int line) : Fatal(file, line) {
        content() << expression << " evaluates to " << expansion;
    }
};

struct NoThrowFailure : public Error {
    NoThrowFailure(const char* expression, const char* file, int line) : Error(file, line) {
        content() << expression << " did not throw";
    }
};

struct WrongTypedExceptionFailure : public Error {
    WrongTypedExceptionFailure(const char* expression, const std::exception& exception, const char* file, int line) : Error(file, line) {
        content() << expression << " threw an unexpected exception with message \"" << exception.what() << "\"";
    }
};

struct WrongExceptionMessageFailure : public Error {
    WrongExceptionMessageFailure(const char* expression, const char* message, const char* expected, const char* file, int line) : Error(file, line) {
        content() << expression << " threw an exception with wrong message (expected \"" << expected << "\", got \"" << message << "\")";
    }
};

struct WrongExceptionFailure : public Error {
    WrongExceptionFailure(const char* expression, const char* file, int line) : Error(file, line) {
        content() << expression << " threw an unknown exception";
    }
};

struct UnexpectedExceptionFailure : public Error {
    UnexpectedExceptionFailure(const std::exception& exception, const char* file, int line) : Error(file, line) {
        content() << "unexpected exception with message \"" << exception.what() << "\" caught";
    }
};

struct UnknownExceptionFailure : public Error {
    UnknownExceptionFailure(const char* file, int line) : Error(file, line) {
        content() << "unknown exception caught";
    }
};

struct FatalNoThrowFailure : public Fatal {
    FatalNoThrowFailure(const char* expression, const char* file, int line) : Fatal(file, line) {
        content() << expression << " did not throw";
    }
};

struct FatalWrongTypedExceptionFailure : public Fatal {
    FatalWrongTypedExceptionFailure(const char* expression, const std::exception& exception, const char* file, int line) : Fatal(file, line) {
        content() << expression << " threw an unexpected exception with message \"" << exception.what() << "\"";
    }
};

struct FatalWrongExceptionMessageFailure : public Fatal {
    FatalWrongExceptionMessageFailure(const char* expression, const char* message, const char* expected, const char* file, int line) : Fatal(file, line) {
        content() << expression << " threw an exception with wrong message (expected \"" << expected << "\", got \"" << message << "\")";
    }
};

struct FatalWrongExceptionFailure : public Fatal {
    FatalWrongExceptionFailure(const char* expression, const char* file, int line) : Fatal(file, line) {
        content() << expression << " threw an unknown exception";
    }
};

struct FatalUnexpectedExceptionFailure : public Fatal {
    FatalUnexpectedExceptionFailure(const std::exception& exception, const char* file, int line) : Fatal(file, line) {
        content() << "unexpected exception with message \"" << exception.what() << "\" caught";
    }
};

struct FatalUnknownExceptionFailure : public Fatal {
    FatalUnknownExceptionFailure(const char* file, int line) : Fatal(file, line) {
        content() << "unknown exception caught";
    }
};

struct Eval : public Notice {
    template <typename T>
    Eval(const char* expression, const T& value, const char* file, int line) : Notice(e_info, file, line) {
        content() << expression << " evaluates to " << value;
    }
};

struct Info : public Notice {
    Info(const char* message, const char* file, int line) : Notice(e_info, file, line) {
        content() << message;
    }
};

struct Warn : public Notice {
    Warn(const char* message, const char* file, int line) : Notice(e_warning, file, line) {
        content() << message;
    }
};

struct UserFailure : public Fatal {
    UserFailure(const char* message, const char* file, int line) : Fatal(file, line) {
        content() << message;
    }
};

typedef void (*TestFn)();

class Test {
    std::string name_;
    TestFn test_;
public:
    Test(const std::string& name, TestFn test) : name_(name), test_(test) {
    }
    const std::string& name() const {
        return name_;
    }
    void run() {
        test_();
    }
};

class Suite {
    static std::vector<Test> tests_;
public:
    struct add {
        add(const std::string& name, TestFn test) {
            tests_.push_back(Test(name, test));
        }
    };
    static const std::vector<Test>& tests() {
        return tests_;
    }
};

std::vector<Test> Suite::tests_;

#ifdef GUT_HAS_CHRONO
struct Clock {
    std::chrono::steady_clock::time_point start_;
    Clock() : start_(std::chrono::steady_clock::now()) { }
    double elapsedTime() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start_).count() / 1000.;
    }
};
#else
struct Clock {
    clock_t start_;
    Clock() { start_ = clock(); }
    double elapsedTime() {
        return static_cast<double>(clock() - start_) / CLOCKS_PER_SEC;
    }
};
#endif

class Report {
public:
    template<class T>
    Report(T report) : report_ (std::make_shared<Model<T>>(std::move(report))) { }
    void start() { report_->onStart(); }
    void end() { report_->onEnd(); }
    void startTest(const std::string& name) { report_->onStartTest(name); }
    void endTest() { report_->onEndTest(); }
    void failure(const Notice& failure) {
        report_->onFailure(failure);
        if (FailFast::enabled())
            throw AbortSuite();
    }
    void eval(const Notice& eval) { report_->onEval(eval); }
    void info(const Notice& info) { report_->onInfo(info); }
    void warn(const Notice& warn) { report_->onWarn(warn); }
    void quit(const std::string& reason) { report_->onQuit(reason); }
    int failedTestCount() { return report_->failedTestCount(); }
private:
    struct Concept {
        virtual ~Concept() { }
        virtual void onStart() = 0;
        virtual void onEnd() = 0;
        virtual void onStartTest(const std::string& /*name*/) = 0;
        virtual void onEndTest() = 0;
        virtual void onFailure(const Notice& /*failure*/) = 0;
        virtual void onEval(const Notice& /*eval*/) = 0;
        virtual void onInfo(const Notice& /*info*/) = 0;
        virtual void onWarn(const Notice& /*warn*/) = 0;
        virtual void onQuit(const std::string& /*reason*/) = 0;
        virtual int failedTestCount() = 0;
    };

    template<class T>
    struct Model : public Concept {
        T report_;
        Model(T report) : report_(std::move(report)) { }
        virtual void onStart() { report_.onStart(); }
        virtual void onEnd() { report_.onEnd(); }
        virtual void onStartTest(const std::string& name) { report_.onStartTest(name); }
        virtual void onEndTest() { report_.onEndTest(); }
        virtual void onFailure(const Notice& failure) { report_.onFailure(failure); }
        virtual void onEval(const Notice& eval) { report_.onEval(eval); }
        virtual void onInfo(const Notice& info) { report_.onInfo(info); }
        virtual void onWarn(const Notice& warn) { report_.onWarn(warn); }
        virtual void onQuit(const std::string& reason) { report_.onQuit(reason); }
        virtual int failedTestCount() { return report_.failedTestCount(); }
    };

  std::shared_ptr<Concept> report_;
};

class theReport {
    static Report report_;
public:
    static void set(Report report) { report_ = report; }
    static void start() { report_.start(); }
    static void end() { report_.end(); }
    static void startTest(const std::string& name) { report_.startTest(name); }
    static void endTest() { report_.endTest(); }
    static void failure(const Notice& failure) { report_.failure(failure); }
    static void eval(const Notice& eval) { report_.eval(eval); }
    static void info(const Notice& info) { report_.info(info); }
    static void warn(const Notice& warn) { report_.warn(warn); }
    static void quit(const std::string& report) { report_.quit(report); }
    static int failedTestCount() { return report_.failedTestCount(); }
};

class DefaultReport {
    size_t tests_;
    bool testFailed_;
    size_t totalFailures_;
    size_t failedTests_;
    std::vector<Notice> log_;
    Clock clock_;
public:
    int failedTestCount() const {
        return static_cast<int>(failedTests_);
    }
    void onStart() {
        tests_ = 0;
        failedTests_ = 0;
        std::cout << "Test suite started..." << std::endl;
    }
    void onEnd() {
        std::cout << "Ran " << tests_ << " test(s) in " << clock_.elapsedTime() << "s." << std::endl;
        if (failedTests_ == 0)
            std::cout << color::lime << "OK - all tests passed." << color::reset << std::endl;
        else
            std::cout << color::red << "FAILED - " << totalFailures_ << " failure(s) in " << failedTests_ << " test(s)." << color::reset << std::endl;
    }
    void onStartTest(const std::string& name) {
        ++tests_;
        testFailed_ = false;;
        std::cout << name << ": ";
    }
    void onEndTest() {
        if (!testFailed_) {
            std::cout << "OK" << std::endl;
            flushLog(e_warning);
        }
        else
            testFailed();
        clear();
    }
    void onFailure(const Notice& failure) {
        if (!testFailed_)
            std::cout << "FAILED" << std::endl;
        log_.push_back(failure);
        ++totalFailures_;
        testFailed_ = true;
    }
    void onEval(const Notice& eval) {
        log_.push_back(eval);
    }
    void onInfo(const Notice& info) {
        log_.push_back(info);
    }
    void onWarn(const Notice& warn) {
        log_.push_back(warn);
    }
    void onQuit(const std::string& /*reason*/) {
        testFailed();
    }
    void testFailed() {
        ++failedTests_;
        flushLog(e_info);
    }
    void flushLog(Level minLevel) {
        for (auto notice : log_)
            if (notice.level() >= minLevel)
                std::cout << " " << notice.toString() << std::endl;
    }
    void clear() {
        log_.clear();
    }
};

Report theReport::report_ = Report(DefaultReport());

} // namespace gut

#define GUT_ENABLE_REPORT(name_) \
    static struct CustomReport { \
        CustomReport() { gut::theReport::set(name_()); } \
    } customReport_;

#define CHECK(expr_) \
    do { \
        if (!(gut::Capture()->*expr_)) \
            gut::theReport::failure(gut::CheckFailure(#expr_, gut::Expression::last, __FILE__, __LINE__)); \
    } while (0)

#define THROWS_(expr_, exception_, prefix_, abort_) \
    do { \
        bool catched_ = false; \
        try { \
            (void)(expr_); \
            gut::theReport::failure(gut::prefix_ ## NoThrowFailure(#expr_, __FILE__, __LINE__)); \
        } catch(const exception_&) { \
            catched_ = true; \
        } catch(const std::exception& e_) { \
            gut::theReport::failure(gut::prefix_ ## WrongTypedExceptionFailure(#expr_, e_, __FILE__, __LINE__)); \
        } catch(...) { \
            gut::theReport::failure(gut::prefix_ ## WrongExceptionFailure(#expr_, __FILE__, __LINE__)); \
        } \
        if (!catched_ && abort_) \
            throw gut::AbortTest(); \
    } while (0)

#define THROWS(expr_, exception_) \
    THROWS_(expr_, exception_, , false)

#define REQUIRE_THROWS(expr_, exception_) \
    THROWS_(expr_, exception_, Fatal, true)

#define REQUIRE(expr_) \
    do { \
        if (!(gut::Capture()->*expr_)) { \
            gut::theReport::failure(gut::RequireFailure(#expr_, gut::Expression::last, __FILE__, __LINE__)); \
            throw gut::AbortTest();\
        } \
    } while (0)

#define THROWS_ANYTHING(expr_) \
    do { \
        try { \
            (void)(expr_); \
            gut::theReport::failure(gut::NoThrowFailure(#expr_, __FILE__, __LINE__)); \
        } catch(...) { \
        } \
    } while (0)

#define REQUIRE_THROWS_ANYTHING(expr_) \
    do { \
        bool threw_ = false; \
        try { \
            (void)(expr_); \
        } catch(...) { \
            threw_ = true; \
        } \
        if (!threw_) { \
            gut::theReport::failure(gut::FatalNoThrowFailure(#expr_, __FILE__, __LINE__)); \
            throw gut::AbortTest(); \
        } \
    } while (0)

#define THROWS_WITH_MESSAGE_(expr_, exception_, what_, prefix_, abort_) \
    do { \
        bool catched_ = false; \
        try { \
            (void)(expr_); \
            gut::theReport::failure(gut::prefix_ ## NoThrowFailure(#expr_, __FILE__, __LINE__)); \
        } catch(const exception_& e_) { \
            if (strcmp(e_.what(), static_cast<const char*>(what_)) != 0) \
                gut::theReport::failure(gut::prefix_ ## WrongExceptionMessageFailure(#expr_, e_.what(), what_, __FILE__, __LINE__)); \
            else \
                catched_ = true; \
        } catch(const std::exception& e_) { \
            gut::theReport::failure(gut::prefix_ ## WrongTypedExceptionFailure(#expr_, e_, __FILE__, __LINE__)); \
        } catch(...) { \
            gut::theReport::failure(gut::prefix_ ## WrongExceptionFailure(#expr_, __FILE__, __LINE__)); \
        } \
        if (!catched_ && abort_) \
            throw gut::AbortTest(); \
    } while (0)

#define THROWS_WITH_MESSAGE(expr_, exception_, what_) \
    THROWS_WITH_MESSAGE_(expr_, exception_, what_, , false)

#define REQUIRE_THROWS_WITH_MESSAGE(expr_, exception_, what_) \
    THROWS_WITH_MESSAGE_(expr_, exception_, what_, Fatal, true)

#define THROWS_NOTHING(expr_) \
    do { \
        try { \
            (void)(expr_); \
        } catch(const std::exception& e_) { \
            gut::theReport::failure(gut::UnexpectedExceptionFailure(e_, __FILE__, __LINE__)); \
        } catch(...) { \
            gut::theReport::failure(gut::UnknownExceptionFailure(__FILE__, __LINE__)); \
        } \
    } while (0)

#define REQUIRE_THROWS_NOTHING(expr_) \
    do { \
        bool threw_ = true; \
        try { \
            (void)(expr_); \
            threw_ = false; \
        } catch(const std::exception& e_) { \
            gut::theReport::failure(gut::FatalUnexpectedExceptionFailure(e_, __FILE__, __LINE__)); \
        } catch(...) { \
            gut::theReport::failure(gut::FatalUnknownExceptionFailure(__FILE__, __LINE__)); \
        } \
        if (threw_) \
            throw gut::AbortTest(); \
    } while (0)

int runTests_() {
    gut::theReport::start();
    for (auto test : gut::Suite::tests()) {
        gut::theReport::startTest(test.name());
        try {
            test.run();
        } catch(const gut::AbortSuite& e) {
            gut::theReport::quit(e.reason());
            break;
        } catch(const gut::AbortTest&) {
        } catch(const std::exception& e_) {
            gut::theReport::failure(gut::FatalUnexpectedExceptionFailure(e_, __FILE__, __LINE__));
        } catch(...) {
            gut::theReport::failure(gut::FatalUnknownExceptionFailure(__FILE__, __LINE__));
        }
        gut::theReport::endTest();
    }
    gut::theReport::end();
    return gut::theReport::failedTestCount();
}

#ifndef GUT_CUSTOM_MAIN
int main() {
    return runTests_();
}
#endif

#define MAKE_UNIQUE(name_) CONCAT_(name_, __LINE__)

#define TEST(name_) \
    static void MAKE_UNIQUE(test_)(); \
    gut::Suite::add MAKE_UNIQUE(testAddition_)(name_, &CONCAT_(test_, __LINE__)); \
    static void MAKE_UNIQUE(test_)()

#define EVAL(expr_) \
    do { \
        gut::theReport::eval(gut::Eval(#expr_, expr_, __FILE__, __LINE__)); \
    } while (0)

#define INFO(message_) \
    do { \
        gut::theReport::info(gut::Info(message_, __FILE__, __LINE__)); \
    } while (0)

#define WARN(message_) \
    do { \
        gut::theReport::warn(gut::Warn(message_, __FILE__, __LINE__)); \
    } while (0)

#define FAIL(message_) \
    do { \
        gut::theReport::failure(gut::UserFailure(message_, __FILE__, __LINE__)); \
    } while (0)

#endif // GUT_H
