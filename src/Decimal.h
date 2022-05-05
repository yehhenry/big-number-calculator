#pragma once

#include "Blamath.h"

class Decimal :public Blamath {

public:
	Decimal();
	Decimal(const Blamath& o);
	Decimal(const char* num);
	Decimal(std::string num);
	Decimal(int num);
	Decimal(long long num);
	Decimal(unsigned int num);
	Decimal(unsigned long long num);
	Decimal(float num);
	Decimal(double num);
	Decimal(long double num);

	Decimal operator+(const Blamath& bla);
	Decimal operator-(const Blamath& bla);
	Decimal operator*(const Blamath& bla);
	Decimal operator/(const Blamath& bla);
	Decimal operator%(const Blamath& bla);
	Decimal operator^(const Blamath& bla);

};