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

#ifdef GUT_HAS_CHRONO
#include <chrono>
#else
#include <time.h>
#endif


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

struct Expression {
	static std::string last;
	virtual ~Expression() { }
	virtual bool evaluate() const = 0;
	virtual std::string toString() const = 0;
	static bool logAndEvaluate(bool value) {
		Expression::last = value ? "true" : "false";
		return value;
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
bool compare(int lhs, T* rhs) {
	return ExprFactory<T*, T*, op>::logAndEvaluate(reinterpret_cast<T*>(lhs), rhs);
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

struct Failure {
protected:
	std::ostringstream description;
public:
	Location location;
	Failure(const char* file, int line) : location(file, line) {
	}
	std::string what() const {
		return description.str();
	}
};

struct CheckFailure : public Failure {
	CheckFailure(const char* expression, const std::string& expansion, const char* file, int line) : Failure(file, line) {
		description << expression << " evaluates to " << expansion;
	}
};

struct NoThrowFailure : public Failure {
	NoThrowFailure(const char* expression, const char* file, int line) : Failure(file, line) {
		description << expression << " did not throw";
	}
};

struct WrongTypedExceptionFailure : public Failure {
	WrongTypedExceptionFailure(const char* expression, const std::exception& exception, const char* file, int line) : Failure(file, line) {
		description << expression << " threw an unexpected exception \"" << exception.what() << "\"";
	}
};

struct WrongExceptionFailure : public Failure {
	WrongExceptionFailure(const char* expression, const char* file, int line) : Failure(file, line) {
		description << expression << " threw an unknown exception";
	}
};

struct UnexpectedExceptionFailure : public Failure {
	UnexpectedExceptionFailure(const std::exception& exception, const char* file, int line) : Failure(file, line) {
		description << "unexpected exception \"" << exception.what() << "\" caught";
	}
};

struct UnknownExceptionFailure : public Failure {
	UnknownExceptionFailure(const char* file, int line) : Failure(file, line) {
		description << "unknown exception caught";
	}
};

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
	static void failure(const Failure& failure) {
		if (report_)
			report_->onFailure(failure);
	}
protected:
	virtual void onStart() { }
	virtual void onEnd() { }
	virtual void onFailure(const Failure& /*failure*/) { }
};

std::shared_ptr<Report> Report::report_;

class DefaultReport : public gut::Report {
	size_t count_;
public:
	size_t count() const {
		return count_;
	}
	DefaultReport() : count_(0) { }
protected:
	virtual void onFailure(const gut::Failure& failure) {
		std::cout << failure.location.file << "(" << failure.location.line << ") : " << failure.what() << std::endl;
		++count_;
	}
	virtual void onEnd() {
		std::cout << "Test suite ran in " << clock_.elapsedTime() << "s." << std::endl;
		if (count_ == 0)
			std::cout << "All tests passed." << std::endl;
		else
			std::cout << count_ << " test(s) failed." << std::endl;
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

#define TEST \
	void runTest(); \
	int main() { \
		auto report = std::make_shared<gut::DefaultReport>(); \
		gut::Report::set(report); \
		gut::Report::start(); \
		try { \
			runTest(); \
		} catch(const std::exception& e) { \
			gut::Report::failure(gut::UnexpectedExceptionFailure(e, __FILE__, __LINE__)); \
		} catch(...) { \
			gut::Report::failure(gut::UnknownExceptionFailure(__FILE__, __LINE__)); \
		} \
		gut::Report::end(); \
		return report->count(); \
	} \
	void runTest()


#endif // GUT_H
