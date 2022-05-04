#pragma once

#include "Blamath.h"

class Integer :public Blamath {
public:
	Integer(const Blamath& o);
	Integer(const char* num);
	Integer(std::string num);
	Integer(int num);
	Integer(long long num);
	Integer(unsigned int num);
	Integer(unsigned long long num);
	Integer(float num);
	Integer(double num);
	Integer(long double num);
};