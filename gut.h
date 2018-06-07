#ifndef GUT_H
#define GUT_H

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#define INT_BASE Dec
#define CHAR_BASE Hex

#define CONCAT(a, b) a ## b
#define CONCAT_(a, b) CONCAT(a, b)
#define INT_TO_STRING CONCAT_(as, INT_BASE)
#define CHAR_TO_STRING CONCAT_(as, CHAR_BASE)

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

std::string toString(const char* value) {
	std::ostringstream os;
	os << "\"" << value << "\"";
	return os.str();
}

std::string toString(const std::string& value) {
	std::ostringstream stream;
	stream << "{" << value << "}";
	return stream.str();
}

std::string toString(bool value) {
	std::ostringstream stream;
	stream << std::boolalpha << value;
	return stream.str();
}

template<typename T>
std::string toString(const T& value) {
	std::ostringstream os;
	os << value;
	return os.str();
}

struct Expression {
	virtual bool evaluate() const = 0;
	virtual std::string toString() const = 0;

	static std::string last;

	// unary expressions support
	static bool logAndEvaluate(bool value) {
		Expression::last = value ? "true" : "false";
		return value;
	}
	// n-ary expressions support
	static bool logAndEvaluate(const Expression& expression) {
		Expression::last = expression.toString();
		return expression.evaluate();
	}
};

std::string Expression::last;

template<typename T, typename U>
struct BinaryExpression : public Expression {
	const T& lhs_;
	const U& rhs_;
	BinaryExpression(const T& lhs, const U& rhs) : lhs_(lhs), rhs_(rhs) { }
	virtual std::string toString() const {
		return ::toString(lhs_) + " " + getOpName() + " " + ::toString(rhs_);
	}
	virtual std::string getOpName() const = 0;
};

template<typename T, typename U>
struct Equal_ : public BinaryExpression<T, U> {
	Equal_(const T& lhs, const U& rhs) : BinaryExpression<T, U>(lhs, rhs) { }
	virtual std::string getOpName() const { return "=="; }
};

template<typename T, typename U>
struct Equal : public Equal_<T, U> {
	Equal(const T& lhs, const U& rhs) : Equal_<T, U>(lhs, rhs) { }
	virtual bool evaluate() const { return this->lhs_ == this->rhs_; }
};

template<typename T>
struct Equal<T*, int> : public Equal_<T*, int> {
	Equal(T* const& lhs, int rhs) : Equal_<T*, int>(lhs, rhs) { }
	virtual bool evaluate() const { return this->lhs_ == reinterpret_cast<T*>(this->rhs_); }
};

template<typename T, typename U>
struct NotEqual_ : public BinaryExpression<T, U> {
	NotEqual_(const T& lhs, const U& rhs) : BinaryExpression<T, U>(lhs, rhs) { }
	virtual std::string getOpName() const { return "!="; }
};

template<typename T, typename U>
struct NotEqual : public NotEqual_<T, U> {
	NotEqual(const T& lhs, const U& rhs) : NotEqual_<T, U>(lhs, rhs) { }
	virtual bool evaluate() const { return this->lhs_ != this->rhs_; }
};

template<typename T>
struct NotEqual<T*, int> : public NotEqual_<T*, int> {
	NotEqual(T* const& lhs, int rhs) : NotEqual_<T*, int>(lhs, rhs) { }
	virtual bool evaluate() const { return this->lhs_ != reinterpret_cast<T*>(this->rhs_); }
};

template<typename T>
struct Equal<int, T*> : public Equal_<int, T*> {
	Equal(int lhs, T* const& rhs) : Equal_<int, T*>(lhs, rhs) { }
	virtual bool evaluate() const { return reinterpret_cast<T*>(this->lhs_) == this->rhs_; }
};

template<typename T>
struct NotEqual<int, T*> : public NotEqual_<int, T*> {
	NotEqual(int lhs, T* const& rhs) : NotEqual_<int, T*>(lhs, rhs) { }
	virtual bool evaluate() const { return reinterpret_cast<T*>(this->lhs_) != this->rhs_; }
};

struct UNEXPECTED_ASSIGNMENT;
struct OPERATION_NOT_SUPPORTED;

template<typename T>
class Term {
	const T& lhs_;
public:
	Term(const T& lhs) : lhs_(lhs) { }
	template<typename U>
	bool operator==(const U& rhs) const {
		return Expression::logAndEvaluate(Equal<T, U>(lhs_, rhs));
	}
	template<typename U>
	bool operator!=(const U& rhs) const {
		return Expression::logAndEvaluate(NotEqual<T, U>(lhs_, rhs));
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

#define CHECK(expr_) \
	do { \
		if (!(Capture()->*expr_)) \
			std::cout << __FILE__ << "(" << __LINE__ << "): " << #expr_ << " evaluates to " << Expression::last << std::endl; \
	} while (0)

#endif // GUT_H
