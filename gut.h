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
	template <typename T>
	static bool logAndEvaluate(const T& value) {
		if (HasOperatorString<T>::value)
			Expression::last = StringRepr<T, HasOperatorString<T>::value>(value).str();
		else
			Expression::last = gut::toString(static_cast<bool>(value));
		return static_cast<bool>(value);
	}
	bool logAndEvaluate() {
		Expression::last = toString();
		return evaluate();
	}
};

std::string Expression::last;

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
		return Expression::logAndEvaluate(lhs_);
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
	Location(const char* file_, int line_) : file(file_), line(line_) {
	}
};

struct AbortTest { };

struct AbortSuite { };

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

#define GUT_ENABLE_FAILFAST gut::FailFast failFast_;

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
		content() << expression << " threw an unexpected exception \"" << exception.what() << "\"";
	}
};

struct WrongExceptionFailure : public Error {
	WrongExceptionFailure(const char* expression, const char* file, int line) : Error(file, line) {
		content() << expression << " threw an unknown exception";
	}
};

struct UnexpectedExceptionFailure : public Fatal {
	UnexpectedExceptionFailure(const std::exception& exception, const char* file, int line) : Fatal(file, line) {
		content() << "unexpected exception \"" << exception.what() << "\" caught";
	}
};

struct UnknownExceptionFailure : public Fatal {
	UnknownExceptionFailure(const char* file, int line) : Fatal(file, line) {
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
	static std::shared_ptr<Report> report_;
protected:
	Clock clock_;
public:
	static void set(std::shared_ptr<Report> report) {
		report_ = report;
	}
	static void start() {
		if (report_)
			report_->onStart();
	}
	static void end() {
		if (report_)
			report_->onEnd();
	}
	static void startTest(const std::string& name) {
		if (report_)
			report_->onStartTest(name);
	}
	static void endTest() {
		if (report_)
			report_->onEndTest();
	}
	static void failure(const Notice& failure) {
		if (report_)
			report_->onFailure(failure);
		if (FailFast::enabled())
			throw AbortSuite();
	}
	static void eval(const Notice& eval) {
		if (report_)
			report_->onEval(eval);
	}
	static void info(const Notice& info) {
		if (report_)
			report_->onInfo(info);
	}
	static void warn(const Notice& warn) {
		if (report_)
			report_->onWarn(warn);
	}
	static void quit() {
		if (report_)
			report_->onQuit();
	}
protected:
	virtual void onStart() { }
	virtual void onEnd() { }
	virtual void onStartTest(const std::string& /*name*/) { }
	virtual void onEndTest() { }
	virtual void onFailure(const Notice& /*failure*/) { }
	virtual void onEval(const Notice& /*eval*/) { }
	virtual void onInfo(const Notice& /*info*/) { }
	virtual void onWarn(const Notice& /*warn*/) { }
	virtual void onQuit() { }
};

std::shared_ptr<Report> Report::report_;

class DefaultReport : public Report {
	size_t tests_;
	size_t testFailures_;
	size_t totalFailures_;
	size_t failedTests_;
	std::vector<Notice> log_;
public:
	size_t failedTestCount() const {
		return failedTests_;
	}
	DefaultReport() : tests_(0), testFailures_(0), totalFailures_(0), failedTests_(0) { }
protected:
	virtual void onStart() {
		std::cout << "Test suite started..." << std::endl;
	}
	virtual void onEnd() {
		std::cout << "Ran " << tests_ << " test(s) in " << clock_.elapsedTime() << "s." << std::endl;
		if (failedTests_ == 0)
			std::cout << color::lime << "OK - all tests passed." << color::reset << std::endl;
		else
			std::cout << color::red << "FAILED - " << totalFailures_ << " failure(s) in " << failedTests_ << " test(s)." << color::reset << std::endl;
	}
	virtual void onStartTest(const std::string& name) {
		++tests_;
		testFailures_ = 0;
		std::cout << name << ": ";
	}
	virtual void onEndTest() {
		if (testFailures_ == 0) {
			std::cout << "OK" << std::endl;
			flushLog(e_warning);
		}
		else
			testFailed();
		clear();
	}
	virtual void onFailure(const Notice& failure) {
		if (testFailures_ == 0)
			std::cout << "FAILED" << std::endl;
		log_.push_back(failure);
		++testFailures_;
		++totalFailures_;
	}
	virtual void onEval(const Notice& eval) {
		log_.push_back(eval);
	}
	virtual void onInfo(const Notice& info) {
		log_.push_back(info);
	}
	virtual void onWarn(const Notice& warn) {
		log_.push_back(warn);
	}
	virtual void onQuit() {
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

} // namespace gut

#define CHECK(expr_) \
	do { \
		if (!(gut::Capture()->*expr_)) \
			gut::Report::failure(gut::CheckFailure(#expr_, gut::Expression::last, __FILE__, __LINE__)); \
	} while (0)

#define THROWS(expr_, exception_) \
	do { \
		try { \
			(void)(expr_); \
			gut::Report::failure(gut::NoThrowFailure(#expr_, __FILE__, __LINE__)); \
		} catch(const exception_&) { \
		} catch(const std::exception& e_) { \
			gut::Report::failure(gut::WrongTypedExceptionFailure(#expr_, e_, __FILE__, __LINE__)); \
		} catch(...) { \
			gut::Report::failure(gut::WrongExceptionFailure(#expr_, __FILE__, __LINE__)); \
		} \
	} while (0)

#define REQUIRE(expr_) \
		if (!(gut::Capture()->*expr_)) { \
			gut::Report::failure(gut::RequireFailure(#expr_, gut::Expression::last, __FILE__, __LINE__)); \
			throw gut::AbortTest();\
		}

int runTests_() {
	auto report_ = std::make_shared<gut::DefaultReport>();
	gut::Report::set(report_);
	gut::Report::start();
	for (auto test : gut::Suite::tests()) {
		gut::Report::startTest(test.name());
		try {
			test.run();
		} catch(const gut::AbortSuite&) {
			gut::Report::quit();
			break;
		} catch(const gut::AbortTest&) {
		} catch(const std::exception& e_) {
			gut::Report::failure(gut::UnexpectedExceptionFailure(e_, __FILE__, __LINE__));
		} catch(...) {
			gut::Report::failure(gut::UnknownExceptionFailure(__FILE__, __LINE__));
		}
		gut::Report::endTest();
	}
	gut::Report::end();
	return report_->failedTestCount();
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
		gut::Report::eval(gut::Eval(#expr_, expr_, __FILE__, __LINE__)); \
	} while (0)

#define INFO(message_) \
	do { \
		gut::Report::info(gut::Info(message_, __FILE__, __LINE__)); \
	} while (0)

#define WARN(message_) \
	do { \
		gut::Report::warn(gut::Warn(message_, __FILE__, __LINE__)); \
	} while (0)

#define FAIL(message_) \
	do { \
		gut::Report::failure(gut::UserFailure(message_, __FILE__, __LINE__)); \
	} while (0)

#endif // GUT_H
