#include <iostream>
#include <vector>
#include "Blamath.h"
#include "Decimal.h"
#include "Integer.h"
using namespace std;

int main() {
	Blamath a(25);
	//			   階層↓
	std::cout << a.getFactorial().toString() << endl;

	Integer i("123");
	Decimal d("123.3");

	vector<Blamath*> nums;
	nums.push_back(&i);
	nums.push_back(&d);
	for (const auto& num : nums) {
		cout << *num << endl;
	}
}
