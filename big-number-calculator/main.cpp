#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include "Blamath.h"
#include "Decimal.h"
#include "Integer.h"
using namespace std;

vector<string> res;

int prec(string c) {
	if (c == "!") {
		return 5;
	}
	else if (c == "^") {
		return 4;
	}
	else if (c == "*") {
		return 3;
	}
	else if (c == "/") {
		return 2;
	}
	else if (c == "+" || c == "-") {
		return 1;
	}
	else {
		return -1;
	}
}

int precc(char c)
{
	if (c == '!') {
		return 5;
	}
	else if (c == '^') {
		return 4;
	}
	else if (c == '*') {
		return 3;
	}
	else if (c == '/') {
		return 2;
	}
	else if (c == '+' || c == '-') {
		return 1;
	}
	else {
		return -1;
	}
}

void infixToPostfix(string str) {
	stack<string> st;
	string result;

	for (int i = 0; i < str.length(); i++) {
		char c = str[i];
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
			while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
				result = result + c;
				i++;
				c = str[i];
			}
			res.push_back(result);
			result.clear();
		}
		if (c == '(') {
			st.push("(");
		}
		else if (c == ')') {
			while (st.top() != "(") {
				res.push_back(st.top());
				st.pop();
			}
			st.pop();
		}
		else {
			while (!st.empty() && precc(str[i]) <= prec(st.top())) {
				if (c == '!' && st.top() == "!") {
					break;
				}
				else {
					res.push_back(st.top());
					st.pop();
				}
			}
			result = c;
			st.push(result);
			result.clear();
		}
	}

	while (!st.empty()) {
		res.push_back(st.top());
		st.pop();
	}
}

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
