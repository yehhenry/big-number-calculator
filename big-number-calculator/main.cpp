#include <iostream>
#include "Blamath.h"

int main() {
	Blamath a(25);
	//			   階層↓
	std::cout << a.getFactorial().getIntPart();
}
