#include "Decimal.h"

Decimal::Decimal(const Blamath& o) :Blamath(o) {
	Blamath::isInteger = false;
}

Decimal::Decimal(const char* num) : Blamath(num) {
	Blamath::isInteger = false;
}

Decimal::Decimal(std::string num) : Blamath(num) {
	Blamath::isInteger = false;
}

Decimal::Decimal(int num) : Blamath(num) {
	Blamath::isInteger = false;
}

Decimal::Decimal(long long num) : Blamath(num) {
	Blamath::isInteger = false;
}

Decimal::Decimal(unsigned int num) : Blamath(num) {
	Blamath::isInteger = false;
}

Decimal::Decimal(unsigned long long num) : Blamath(num) {
	Blamath::isInteger = false;
}

Decimal::Decimal(float num) : Blamath(num) {
	Blamath::isInteger = false;
}

Decimal::Decimal(double num) : Blamath(num) {
	Blamath::isInteger = false;
}

Decimal::Decimal(long double num) : Blamath(num) {
	Blamath::isInteger = false;
}