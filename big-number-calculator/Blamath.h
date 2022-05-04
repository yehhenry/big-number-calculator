#pragma once
#pragma warning(disable : 4996)

#include<iostream>
#include<sstream>
#include <algorithm>
#include <string>
#include <stdlib.h>
#include <stack>
#include <vector>

using namespace std;

class Blamath {

protected:
	bool isInteger = true;
	string value = "0";

public:
	Blamath();
	Blamath(const Blamath& bla);
	Blamath(const char* num);
	Blamath(std::string num);
	Blamath(int num);
	Blamath(long long num);
	Blamath(unsigned int num);
	Blamath(unsigned long long num);
	Blamath(float num);
	Blamath(double num);
	Blamath(long double num);

	Blamath operator+(const Blamath& bla);
	Blamath operator-(const Blamath& bla);
	Blamath operator*(const Blamath& bla);
	Blamath operator/(const Blamath& bla);
	Blamath operator%(const Blamath& bla);
	Blamath operator^(const Blamath& bla);

	void operator+=(const Blamath& bla);
	void operator-=(const Blamath& bla);
	void operator*=(const Blamath& bla);
	void operator/=(const Blamath& bla);
	void operator^=(const Blamath& bla);

	bool operator > (const Blamath& bla);
	bool operator >= (const Blamath& bla);
	bool operator == (const Blamath& bla);
	bool operator < (const Blamath& bla);
	bool operator <= (const Blamath& bla);

	friend std::istream& operator>>(std::istream& is, Blamath& bla);
	friend std::ostream& operator<<(std::ostream& os, const Blamath& bla);

	// 取得完整數字字串
	std::string toString() const;
	// 四捨五入到指定小數點後位數
	void round(int scale);
	// 取得階層
	Blamath getFactorial();
	// 取得整數部分的字串
	std::string getIntPart() const;
	// 取得小數點後部分的字串
	std::string getDecPart() const;

	// 設定保留的小數後位數
	static void blaScale(int scale);
	// 取得兩個數字相除
	static std::string blaDiv(const std::string& lhs, const std::string& rhs, int scale = INT_MIN);
	// 取得兩個數字相除的餘數
	static std::string blaMod(const std::string& lhs, const std::string& rhs);
	// 取得數字的次方
	static std::string blaPow(const std::string& lhs, const std::string& rhs);
	// 取得兩個數字的相加
	static std::string blaAdd(const std::string& lhs, const std::string& rhs, int scale = INT_MIN);
	// 取得兩個數字的相減
	static std::string blaSub(const std::string& lhs, const std::string& rhs, int scale = INT_MIN);
	// 取得兩個數字的相乘
	static std::string blaMul(const std::string& lhs, const std::string& rhs, int scale = INT_MIN);
	// 取得某數四捨五入後的值，並輸入另一個數指定小數後幾位
	static std::string blaRound(const std::string& lhs, int scale = INT_MIN);
	// 比較兩個數的大小
	static int blaCompare(const std::string& lhs, const std::string& rhs, int scale = INT_MIN);

};