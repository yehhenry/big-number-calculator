#include "Integer.h"

Integer::Integer() :Blamath() {
	Blamath::isInteger = true;
}

Integer::Integer(const Blamath& o) : Blamath(o) {
	Blamath::isInteger = true;
}

Integer::Integer(const char* num) : Blamath(num) {
	Blamath::isInteger = true;
}

Integer::Integer(std::string num) : Blamath(num) {
	Blamath::isInteger = true;
}

Integer::Integer(int num) : Blamath(num) {
	Blamath::isInteger = true;
}

Integer::Integer(long long num) : Blamath(num) {
	Blamath::isInteger = true;
}

Integer::Integer(unsigned int num) : Blamath(num) {
	Blamath::isInteger = true;
}

Integer::Integer(unsigned long long num) : Blamath(num) {
	Blamath::isInteger = true;
}

Integer::Integer(float num) : Blamath(num) {
	Blamath::isInteger = true;
}

Integer::Integer(double num) : Blamath(num) {
	Blamath::isInteger = true;
}

Integer::Integer(long double num) : Blamath(num) {
	Blamath::isInteger = true;
}