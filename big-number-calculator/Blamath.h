#pragma once
#pragma warning(disable : 4996)

#include<string>
#include<iostream>
#include<sstream>

class Blamath {

public:
	bool isInteger = true;

	Blamath() { }
	Blamath(const Blamath& o) : value(o.value) { }
	Blamath(const char* num) : value(num) {  }
	Blamath(std::string num) : value(num) {  }
	Blamath(int num) : value() { std::stringstream ss; ss << num; value = ss.str(); }
	Blamath(long long num) : value() { std::stringstream ss; ss << num; value = ss.str(); }
	Blamath(unsigned int num) : value() { std::stringstream ss; ss << num; value = ss.str(); }
	Blamath(unsigned long long num) : value() { std::stringstream ss; ss << num; value = ss.str(); }
	Blamath(float num) : value() { std::stringstream ss; ss << num; value = ss.str(); }
	Blamath(double num) : value() { std::stringstream ss; ss << num; value = ss.str(); }
	Blamath(long double num) { std::stringstream ss; ss << num; value = ss.str(); }

	Blamath operator+(const Blamath& o) {
		return Blamath::blaAdd(value, o.value);
	}

	Blamath operator-(const Blamath& o) {
		return Blamath::blaSub(value, o.value);
	}

	Blamath operator*(const Blamath& o) {
		return Blamath::blaMul(value, o.value);
	}

	Blamath operator/(const Blamath& o) {
		return Blamath::blaDiv(value, o.value);
	}

	Blamath operator%(const Blamath& o) {
		return Blamath::blaMod(value, o.value);
	}

	Blamath operator^(const Blamath& o) {
		return Blamath::blaPow(value, o.value);
	}

	Blamath operator=(const std::string& o) {
		//TODO =
	}

	void operator+=(const Blamath& o) {
		value = Blamath::blaAdd(value, o.value);
	}
	void operator-=(const Blamath& o) {
		value = Blamath::blaSub(value, o.value);
	}
	void operator*=(const Blamath& o) {
		value = Blamath::blaMul(value, o.value);
	}
	void operator/=(const Blamath& o) {
		value = Blamath::blaDiv(value, o.value);
	}
	void operator^=(const Blamath& o) {
		value = Blamath::blaPow(value, o.value);
	}

	bool operator > (const Blamath& o) {
		return Blamath::blaCompare(value, o.value) > 0;
	}
	bool operator >= (const Blamath& o) {
		return Blamath::blaCompare(value, o.value) >= 0;
	}
	bool operator == (const Blamath& o) {
		return Blamath::blaCompare(value, o.value) == 0;
	}
	bool operator < (const Blamath& o) {
		return Blamath::blaCompare(value, o.value) < 0;
	}
	bool operator <= (const Blamath& o) {
		return Blamath::blaCompare(value, o.value) <= 0;
	}

	friend std::istream& operator>>(std::istream& is, Blamath& o) {
		//TODO >>
		is >> o;

		return is;
	}

	friend std::ostream& operator<<(std::ostream& os, Blamath& o) {
		if (o.isInteger) {
			os << o.getIntPart();
		}
		else {
			os << o.toString();
		}

		return os;
	}

	std::string toString() {
		return value;
	}

	void round(int scale) {
		if (scale >= 1)
			value = Blamath::blaRound(value, scale);
	}

	Blamath getFactorial();

	std::string getIntPart() {
		std::size_t dot = value.find('.');
		if (dot != std::string::npos) {
			if (dot == 0)
				return std::string("0");
			if (dot == 1 && value[0] == '-')
				return std::string("-0");
			return value.substr(0, dot);
		}
		else {
			return value;
		}
	}

	std::string getDecPart() {
		std::size_t dot = value.find('.');
		if (dot != std::string::npos)
			return value.length() > dot + 1 ? value.substr(dot + 1) : std::string("0");
		else
			return std::string("0");
	}

private:
	std::string value;

public:
	static void blaScale(int scale);

	static std::string blaDiv(const std::string& lhs, const std::string& rhs, int scale = INT_MIN);

	static std::string blaMod(const std::string& lhs, const std::string& rhs);

	static std::string blaPow(const std::string& lhs, const std::string& rhs);

	static std::string blaAdd(const std::string& lhs, const std::string& rhs, int scale = INT_MIN);

	static std::string blaSub(const std::string& lhs, const std::string& rhs, int scale = INT_MIN);

	static std::string blaMul(const std::string& lhs, const std::string& rhs, int scale = INT_MIN);

	static std::string blaRound(const std::string& lhs, int scale = INT_MIN);

	static int blaCompare(const std::string& lhs, const std::string& rhs, int scale = INT_MIN);
};