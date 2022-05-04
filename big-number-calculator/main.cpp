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
		return 4;
	}
	else if (c == "^") {
		return 3;
	}
	else if (c == "*" || c == "/") {
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
		return 4;
	}
	else if (c == '^') {
		return 3;
	}
	else if (c == '*' || c == '/') {
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

string calculate(Blamath a, Blamath b, const std::string operatorSign)
{
	Blamath temp;
	if (operatorSign == "+") {
		temp = a + b;
		return temp.toString();
	}
	else if (operatorSign == "-") {
		temp = a - b;
		return temp.toString();
	}
	else if (operatorSign == "*") {
		temp = a * b;
		return temp.toString();
	}
	else if (operatorSign == "/") {
		temp = a / b;
		return temp.toString();
	}
	else if (operatorSign == "^") {
		temp = a ^ b;
		return temp.toString();
	}

}
// POSTFIX evaluation to get result
static std::string evaluatePostfixExpression()
{
	stack <string> myStack;
	string str;
	for (int i = 0, j = res.size() - 1,index=0; i < j; i++,index++)
	{
		str = res[index];
		if (index + 1 <= j) {
			if (res[index + 1] == "!") {
				i--;
			}
		}
		
		if (str != "!" && str != "^" && str != "*" && str != "/" && str != "+" && str != "-") {
			myStack.push(res[index]);
		}
		else if (str == "!") {
			Blamath a(myStack.top());
			myStack.pop();
			myStack.push(a.getFactorial().toString());
		}
		else if (str == "^" || str == "*" || str == "/" || str == "+" || str == "-") //found operator
		{

			Blamath b(myStack.top());
			myStack.pop();

			Blamath a(myStack.top());
			myStack.pop();

			myStack.push(calculate(a, b, str));
		}
	}
	return myStack.top();
}


int main() {
	Blamath a(25);
	string ans;
	//			   階層↓
	/*std::cout << a.getFactorial().toString() << endl;

	Integer i("123");
	Decimal d("123.3");

	vector<Blamath*> nums;
	nums.push_back(&i);
	nums.push_back(&d);
	for (const auto& num : nums) {
		cout << *num << endl;
	}*/
	string infix;
	while (getline(cin, infix)) {
		infixToPostfix(infix);

		cout << "Infix Expression: " << infix << endl;
		cout << "Postfix Expression: ";
		//verify result
		for (int i = 0; i < res.size(); i++) {
			cout << res[i];
		}
		cout << endl;
		ans = evaluatePostfixExpression();
		cout << ans << endl;
		res.clear();
	}
}
