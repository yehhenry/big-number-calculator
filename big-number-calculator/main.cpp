#include "Blamath.h"
#include "Decimal.h"
#include "Integer.h"

int main() {

	Blamath bla;
	while (cin >> bla) {
		cout << bla << endl;
	}

	// 第一題已修正
	//Blamath bla = "-((-5.0+1*50)*5/(5*3*-1))";
	//cout << bla.originalEquation << endl << bla << endl;

	//*********
	// API 1
	//*********
	//Integer i;
	//Decimal d;
	//cin >> i >> d;
	//cout << i + d;

	//*********
	// API 2
	//*********
	//Integer i = "123";
	//Decimal d = "0.3 * 3";

	//*********
	// API 3
	//*********
	//Integer i("123");
	//Decimal d("123.3");

	//vector<Blamath*> nums;
	//nums.push_back(&i);
	//nums.push_back(&d);
	//for (const auto& num : nums) {
	//	cout << *num << endl;
	//}

}
