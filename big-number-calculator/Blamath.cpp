#define BLA_ASSERT(cond) ((!(cond)) ? blaAssert(#cond,__FILE__,__LINE__) : blaNoassert())

#include "Blamath.h"

#pragma region Internal

static int _blaScale = 100;

static const std::string ONE("1");
static const std::string ZERO("0");

// 透過字串取得符號階層
int precedenceByString(string c) {
	if (c == "!") {
		return 5;
	}
	else if (c == "^") {
		return 4;
	}
	else if (c == "*" || c == "/") {
		return 3;
	}
	else if (c == "+" || c == "-") {
		return 2;
	}
	else if (c == ",") {
		return 1;
	}
	else {
		return -1;
	}
}

// 透過字元取得符號階層
int precedenceByChar(char c)
{
	if (c == '!') {
		return 5;
	}
	else if (c == '^') {
		return 4;
	}
	else if (c == '*' || c == '/') {
		return 3;
	}
	else if (c == '+' || c == '-') {
		return 2;
	}
	else if (c == ',') {
		return 1;
	}
	else {
		return -1;
	}
}

// 將所有指定的字串取代為新的字串
void replaceAll(string& s, string const& toReplace, string const& replaceWith) {
	ostringstream oss;
	size_t pos = 0;
	size_t prevPos = pos;

	while (true) {
		prevPos = pos;
		pos = s.find(toReplace, pos);
		if (pos == string::npos)
			break;
		oss << s.substr(prevPos, pos - prevPos);
		oss << replaceWith;
		pos += toReplace.size();
	}

	oss << s.substr(prevPos);
	s = oss.str();
}

// 將算式轉換為後序式
vector<string> infixToPostfix(string str) {
	stack<string> st;
	string result;
	vector<string> res;

	str.erase(remove(str.begin(), str.end(), ' '), str.end()); // 移除空白
	replaceAll(str, "Power", ""); // 清除 Power

	for (int i = 0; i < str.length(); i++) {
		char c = str[i];
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9' ||
			(c == '-' && i == 0) || (c == '-' && (str[i - 1] == '(' || str[i - 1] == '+' || str[i - 1] == '-' || str[i - 1] == '*' || str[i - 1] == '/')))) {
			while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '.' ||
				(c == '-' && i == 0) || (c == '-' && (str[i - 1] == '(' || str[i - 1] == '+' || str[i - 1] == '-' || str[i - 1] == '*' || str[i - 1] == '/'))) {
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
			while (!st.empty() && precedenceByChar(str[i]) <= precedenceByString(st.top())) {
				if (c == '!' && st.top() == "!")
					break;
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
		if (st.top()[0] == '\0') {
			st.pop();
			continue;
		}
		res.push_back(st.top());
		st.pop();
	}
	return res;
}

// 透過指定的符號取得兩數計算的結果
string calculate(Blamath a, Blamath b, const std::string operatorSign) {
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
	else if (operatorSign == ",") {
		temp = a ^ b;
		return temp.toString();
	}
}

// 取得後序式運算後的結果
string evaluatePostfixExpression(vector<string> res) {
	stack <string> myStack;
	string str;
	for (int i = 0; i < res.size(); i++) {
		str = res[i];

		if (str != "!" && str != "^" && str != "*" && str != "/" && str != "+" && str != "-" && str != ",") {
			myStack.push(res[i]);
		}
		else if (str == "!") {
			Blamath a(myStack.top());
			myStack.pop();
			myStack.push(a.getFactorial().toString());
		}
		else if (str == "^" || str == "*" || str == "/" || str == "+" || str == "-" || str == ",") { //found operator

			Blamath b(myStack.top());
			myStack.pop();

			Blamath a(myStack.top());
			myStack.pop();

			myStack.push(calculate(a, b, str));
		}
	}
	return myStack.top();
}

static void blaNoassert() { }
static void blaAssert(const char* assertion, const char* file, int line) {
	std::cerr << "Critical Error in: " << assertion << ", File '" << file << "' in line " << line << "." << std::endl;
	exit(-1);
}

//parse a number into parts, returns scale on success and -1 on error
static int _blaParseNumber(const std::string& s, int& lsign, int& lint, int& ldot, int& lfrac, int& lscale) {
	int i = 0;
	lsign = 1;
	if (s[i] == '-' || s[i] == '+') {
		if (s[i] == '-') {
			lsign = -1;
		}
		i++;
	}
	int len = s.length();
	if (i >= len) {
		return -1;
	}
	lint = i;

	while (i < len && '0' <= s[i] && s[i] <= '9') {
		i++;
	}
	ldot = i;

	lscale = 0;
	if (i < len && s[i] == '.') {
		lscale = (int)s.length() - i - 1;
		i++;
	}
	lfrac = i;

	while (i < len && '0' <= s[i] && s[i] <= '9') {
		i++;
	}

	if (i < len) {
		return -1;
	}

	while (s[lint] == '0' && lint + 1 < ldot) {
		lint++;
	}
	//  while (lscale > 0 && s[lfrac + lscale - 1] == '0') {
	//    lscale--;
	//  }
	if (lscale == 0 && lfrac > ldot) {
		lfrac--;
		BLA_ASSERT(lfrac == ldot);
	}

	if (lsign < 0 && (lscale == 0 && s[lint] == '0')) {
		lsign = 1;
	}
	return lscale;
}

static std::string _blaZero(int scale) {
	if (scale == 0) {
		return ZERO;
	}
	std::string result(scale + 2, '0');
	result[1] = '.';
	return result;
}

static int _blaComp(const char* lhs, int lint, int ldot, int lfrac, int lscale, const char* rhs, int rint, int rdot, int rfrac, int rscale, int scale) {
	int llen = ldot - lint;
	int rlen = rdot - rint;

	if (llen != rlen) {
		return (llen < rlen ? -1 : 1);
	}

	for (int i = 0; i < llen; i++) {
		if (lhs[lint + i] != rhs[rint + i]) {
			return (lhs[lint + i] < rhs[rint + i] ? -1 : 1);
		}
	}

	int i;
	for (i = 0; (i < lscale || i < rscale) && i < scale; i++) {
		int lchar = (i < lscale ? lhs[lfrac + i] : '0');
		int rchar = (i < rscale ? rhs[rfrac + i] : '0');
		if (lchar != rchar) {
			return (lchar < rchar ? -1 : 1);
		}
	}

	return 0;
}

static std::string _blaRound(char* lhs, int lint, int ldot, int lfrac, int lscale, int scale, int sign, bool addTrailingZeroes, bool roundLast = false) {
	while (lhs[lint] == '0' && lint + 1 < ldot) {
		lint++;
	}

	BLA_ASSERT(lint > 0 && lscale >= 0 && scale >= 0);

	if (sign < 0 && lhs[lint] == '0') {
		sign = 1;
		for (int i = 0; i < lscale; i++) {
			if (lhs[lfrac + i] != '0') {
				sign = -1;
				break;
			}
		}
	}

	if (roundLast) {
		if (lscale > scale) {
			while (scale > 0 && lhs[lfrac + scale - 1] == '9' && lhs[lfrac + scale] >= '5') {
				scale--;
			}
			lscale = scale;
			if (lhs[lfrac + scale] >= '5') {
				if (scale > 0) {
					lhs[lfrac + scale - 1]++;
				}
				else {
					lfrac--;
					BLA_ASSERT(lfrac == ldot);

					int i;
					lhs[lint - 1] = '0';
					for (i = 0; lhs[ldot - i - 1] == '9'; i++) {
						lhs[ldot - i - 1] = '0';
					}
					lhs[ldot - i - 1]++;
					if (ldot - i - 1 < lint) {
						lint = ldot - i - 1;
					}
				}
			}
		}

		while (lscale > 0 && lhs[lfrac + lscale - 1] == '0') {
			lscale--;
		}
	}
	else {
		if (lscale > scale) {
			lscale = scale;
		}
	}

	if (lscale == 0 && lfrac > ldot) {
		lfrac--;
		BLA_ASSERT(lfrac == ldot);
	}

	if (sign < 0) {
		lhs[--lint] = '-';
	}

	if (lscale == scale || !addTrailingZeroes) {
		return std::string(std::string(lhs + lint).substr(0, lfrac + lscale - lint));
	}
	else {
		std::string result(std::string(lhs + lint).substr(0, lfrac + lscale - lint));
		if (lscale == 0) {
			result += '.';
		}
		for (int kI = 0; kI < scale - lscale; ++kI)
			result += '0';
		return result;
	}
}

static std::string _blaAddPositive(const char* lhs, int lint, int ldot, int lfrac, int lscale, const char* rhs, int rint, int rdot, int rfrac, int rscale, int scale, int sign) {
	int llen = ldot - lint;
	int rlen = rdot - rint;

	int resint, resdot, resfrac, resscale;

	int resultLen = std::max(llen, rlen) + 1;
	int resultScale = std::max(lscale, rscale);
	int resultSize = resultLen + resultScale + 3;
	std::string result(resultSize, '0');

	int i, um = 0;
	int curPos = resultSize;
	int wasFrac = 0;
	for (i = resultScale - 1; i >= 0; i--) {
		if (i < lscale) {
			um += lhs[lfrac + i] - '0';
		}
		if (i < rscale) {
			um += rhs[rfrac + i] - '0';
		}

		if (um != 0 || wasFrac) {
			result[--curPos] = (char)(um % 10 + '0');
			um /= 10;
			wasFrac = 1;
		}
	}
	resscale = resultSize - curPos;
	resfrac = curPos;
	if (wasFrac) {
		result[--curPos] = '.';
	}
	resdot = curPos;

	for (int i = 0; i < resultLen; i++) {
		if (i < llen) {
			um += lhs[ldot - i - 1] - '0';
		}
		if (i < rlen) {
			um += rhs[rdot - i - 1] - '0';
		}

		result[--curPos] = (char)(um % 10 + '0');
		um /= 10;
	}
	resint = curPos;
	BLA_ASSERT(curPos > 0);

	return _blaRound((char*)result.data(), resint, resdot, resfrac, resscale, scale, sign, 1);
}

static std::string _blaSubPositive(const char* lhs, int lint, int ldot, int lfrac, int lscale, const char* rhs, int rint, int rdot, int rfrac, int rscale, int scale, int sign) {
	int llen = ldot - lint;
	int rlen = rdot - rint;

	int resint, resdot, resfrac, resscale;

	int resultLen = llen;
	int resultScale = std::max(lscale, rscale);
	int resultSize = resultLen + resultScale + 3;
	std::string result(resultSize, '0');

	int i, um = 0, nextUm = 0;
	int curPos = resultSize;
	int wasFrac = 0;
	for (i = resultScale - 1; i >= 0; i--) {
		um = nextUm;
		if (i < lscale) {
			um += lhs[lfrac + i] - '0';
		}
		if (i < rscale) {
			um -= rhs[rfrac + i] - '0';
		}
		if (um < 0) {
			nextUm = -1;
			um += 10;
		}
		else {
			nextUm = 0;
		}

		if (um != 0 || wasFrac) {
			result[--curPos] = (char)(um + '0');
			wasFrac = 1;
		}
	}
	resscale = resultSize - curPos;
	resfrac = curPos;
	if (wasFrac) {
		result[--curPos] = '.';
	}
	resdot = curPos;

	for (int i = 0; i < resultLen; i++) {
		um = nextUm;
		um += lhs[ldot - i - 1] - '0';
		if (i < rlen) {
			um -= rhs[rdot - i - 1] - '0';
		}
		if (um < 0) {
			nextUm = -1;
			um += 10;
		}
		else {
			nextUm = 0;
		}

		result[--curPos] = (char)(um + '0');
	}
	resint = curPos;
	BLA_ASSERT(curPos > 0);

	return _blaRound((char*)result.data(), resint, resdot, resfrac, resscale, scale, sign, 1);
}

static std::string _blaMulPositive(const char* lhs, int lint, int ldot, int lfrac, int lscale, const char* rhs, int rint, int rdot, int rfrac, int rscale, int scale, int sign) {
	int llen = ldot - lint;
	int rlen = rdot - rint;

	int resint, resdot, resfrac, resscale;

	int resultLen = llen + rlen;
	int resultScale = lscale + rscale;
	int resultSize = resultLen + resultScale + 3;
	std::string result(resultSize, '0');

	int* res = (int*)malloc(sizeof(int) * resultSize);
	memset(res, 0, sizeof(int) * resultSize);
	for (int i = -lscale; i < llen; i++) {
		int x = (i < 0 ? lhs[lfrac - i - 1] : lhs[ldot - i - 1]) - '0';
		for (int j = -rscale; j < rlen; j++) {
			int y = (j < 0 ? rhs[rfrac - j - 1] : rhs[rdot - j - 1]) - '0';
			res[i + j + resultScale] += x * y;
		}
	}
	for (int i = 0; i + 1 < resultSize; i++) {
		res[i + 1] += res[i] / 10;
		res[i] %= 10;
	}

	int curPos = resultSize;
	for (int i = 0; i < resultScale; i++) {
		result[--curPos] = (char)(res[i] + '0');
	}
	resscale = resultSize - curPos;
	resfrac = curPos;
	if (resultScale > 0) {
		result[--curPos] = '.';
	}
	resdot = curPos;

	for (int i = resultScale; i < resultLen + resultScale; i++) {
		result[--curPos] = (char)(res[i] + '0');
	}
	resint = curPos;
	BLA_ASSERT(curPos > 0);

	free(res);

	char* data = (char*)malloc((result.length() + 1) * sizeof(char));
	sprintf(data, result.c_str());
	std::string ret = _blaRound(data, resint, resdot, resfrac, resscale, scale, sign, 0);
	free(data);

	return ret;
}

static std::string _blaDivPositive(const char* lhs, int lint, int ldot, int lfrac, int lscale, const char* rhs, int rint, int rdot, int rfrac, int rscale, int scale, int sign) {
	int llen = ldot - lint;
	int rlen = rdot - rint;

	int resint, resdot = -1, resfrac = -1, resscale;

	int resultLen = std::max(llen + rscale - rlen + 1, 1);
	int resultScale = scale;
	int resultSize = resultLen + resultScale + 3;

	if (rscale == 0 && rhs[rint] == '0') {
		std::cerr << ("Division by zero in function bcdiv") << std::endl << std::endl;
		return ZERO;
	}

	int dividendLen = llen + lscale;
	int dividerLen = rlen + rscale;
	int* dividend = (int*)malloc(sizeof(int) * (resultSize + dividendLen + dividerLen));
	int* divider = (int*)malloc(sizeof(int) * dividerLen);
	memset(dividend, 0, sizeof(int) * (resultSize + dividendLen + dividerLen));
	memset(divider, 0, sizeof(int) * dividerLen);

	for (int i = -lscale; i < llen; i++) {
		int x = (i < 0 ? lhs[lfrac - i - 1] : lhs[ldot - i - 1]) - '0';
		dividend[llen - i - 1] = x;
	}

	for (int i = -rscale; i < rlen; i++) {
		int x = (i < 0 ? rhs[rfrac - i - 1] : rhs[rdot - i - 1]) - '0';
		divider[rlen - i - 1] = x;
	}

	int dividerSkip = 0;
	while (dividerLen > 0 && divider[0] == 0) {
		divider++;
		dividerSkip++;
		dividerLen--;
	}
	BLA_ASSERT(dividerLen > 0);

	int curPow = llen - rlen + dividerSkip;
	int curPos = 2;

	if (curPow < -scale) {
		divider -= dividerSkip;
		dividerLen += dividerSkip;
		free(dividend);
		free(divider);
		return _blaZero(scale);
	}

	std::string result(resultSize, '0');
	resint = curPos;
	if (curPow < 0) {
		result[curPos++] = '0';
		resdot = curPos;
		result[curPos++] = '.';
		resfrac = curPos;
		for (int i = -1; i > curPow; i--) {
			result[curPos++] = '0';
		}
	}

	int beg = 0, realBeg = 0;
	while (curPow >= -scale) {
		char dig = '0';
		while (true) {
			if (realBeg < beg && dividend[realBeg] == 0) {
				realBeg++;
			}

			bool less = false;
			if (realBeg == beg) {
				for (int i = 0; i < dividerLen; i++) {
					if (dividend[beg + i] != divider[i]) {
						less = (dividend[beg + i] < divider[i]);
						break;
					}
				}
			}
			if (less) {
				break;
			}

			for (int i = dividerLen - 1; i >= 0; i--) {
				dividend[beg + i] -= divider[i];
				if (dividend[beg + i] < 0) {
					dividend[beg + i] += 10;
					dividend[beg + i - 1]--;
				}
			}
			dig++;
		}

		result[curPos++] = dig;

		if (curPow == 0) {
			resdot = curPos;
			if (scale > 0) {
				result[curPos++] = '.';
			}
			resfrac = curPos;
		}
		curPow--;
		beg++;
	}
	resscale = curPos - resfrac;

	divider -= dividerSkip;
	dividerLen += dividerSkip;
	free(dividend);
	free(divider);

	char* data = (char*)malloc((result.length() + 1) * sizeof(char));
	sprintf(data, result.c_str());
	std::string ret = _blaRound(data, resint, resdot, resfrac, resscale, scale, sign, 0);
	free(data);

	return ret;
}


static std::string _blaAdd(const char* lhs, int lsign, int lint, int ldot, int lfrac, int lscale, const char* rhs, int rsign, int rint, int rdot, int rfrac, int rscale, int scale) {
	if (lsign > 0 && rsign > 0) {
		return _blaAddPositive(lhs, lint, ldot, lfrac, lscale, rhs, rint, rdot, rfrac, rscale, scale, 1);
	}

	if (lsign > 0 && rsign < 0) {
		if (_blaComp(lhs, lint, ldot, lfrac, lscale, rhs, rint, rdot, rfrac, rscale, 1000000000) >= 0) {
			return _blaSubPositive(lhs, lint, ldot, lfrac, lscale, rhs, rint, rdot, rfrac, rscale, scale, 1);
		}
		else {
			return _blaSubPositive(rhs, rint, rdot, rfrac, rscale, lhs, lint, ldot, lfrac, lscale, scale, -1);
		}
	}

	if (lsign < 0 && rsign > 0) {
		if (_blaComp(lhs, lint, ldot, lfrac, lscale, rhs, rint, rdot, rfrac, rscale, 1000000000) <= 0) {
			return _blaSubPositive(rhs, rint, rdot, rfrac, rscale, lhs, lint, ldot, lfrac, lscale, scale, 1);
		}
		else {
			return _blaSubPositive(lhs, lint, ldot, lfrac, lscale, rhs, rint, rdot, rfrac, rscale, scale, -1);
		}
	}

	if (lsign < 0 && rsign < 0) {
		return _blaAddPositive(lhs, lint, ldot, lfrac, lscale, rhs, rint, rdot, rfrac, rscale, scale, -1);
	}

	BLA_ASSERT(0);
	return ZERO;
}
#pragma endregion

#pragma region Constructor

Blamath::Blamath() {
	this->value = ZERO;
}

Blamath::Blamath(const Blamath& bla) {
	this->value = bla.value;
	this->isInteger = bla.isInteger;
}

map<string, string> Blamath::getLayer1SubEquations(string equation) {
	/*
		// JavaScript Version
		let stackStr = []; // 找到的最小層運算式
		let cache = ""; // 快取
		let finding = false; // 開始尋找
		a.split("").forEach(e => {
			if (e == "(") {
				finding = true;
			}else if (e == ")"){
				finding = false;
				stackStr.push(cache);
				cache = "";
			}
			if (e != "(" && e != ")" && finding) {
				cache += e;
			}
		})
		console.log(stackStr);
	*/
	map<string, string> equationStack;
	string cache = "";
	bool finding = false;
	for (const char& item : equation) {
		if (item == '(') {
			finding = true;
		}
		else if (item == ')' && finding && cache != "") {
			finding = false;
			Blamath newEquation = cache;
			string originalEquation = '(' + cache + ')';
			if (equationStack.find(originalEquation) == equationStack.end()) {
				equationStack[originalEquation] = newEquation.getValue();
			}
			cache = "";
		}

		if (item != '(' && item != ')' && finding) {
			cache += item;
		}
	}
	return equationStack;
}

int Blamath::getSubEquaCount(string eqation) {
	int leftPair = 0;
	int rightPair = 0;
	for (const char& item : eqation) {
		if (item == '(') {
			leftPair++;
		}
		else if (item == ')') {
			rightPair++;
		}
	}
	return leftPair != rightPair ? -1 : leftPair;
}

Blamath::Blamath(const char* num) {
	// TODO
	this->originalEquation = num;
	string equation = num;
	replaceAll(equation, " ", "");
	int subEquationCount = getSubEquaCount(equation);
	if (subEquationCount < 0) {
		this->value = ZERO;
		return;
	}
	while (getSubEquaCount(equation) > 0) {
		map<string, string> layer1Equations = getLayer1SubEquations(equation);
		for (const auto& item : layer1Equations) {
			replaceAll(equation, item.first, item.second);
		}
		replaceAll(equation, "--", "");
	}
	this->value = evaluatePostfixExpression(infixToPostfix(equation));
	this->value = blaAdd(this->value, ZERO);
	string temp = this->value.substr(this->value.find(".") + 1);
	for (char& c : temp) {
		if (c != '0') {
			this->isInteger = false;
			break;
		}
	}
}


Blamath::Blamath(std::string num) {
	// TODO
	string equation = num;
	replaceAll(equation, " ", "");
	int subEquationCount = getSubEquaCount(equation);
	if (subEquationCount < 0) {
		this->value = ZERO;
		return;
	}
	while (getSubEquaCount(equation) > 0) {
		map<string, string> layer1Equations = getLayer1SubEquations(equation);
		for (const auto& item : layer1Equations) {
			replaceAll(equation, item.first, item.second);
		}
		replaceAll(equation, "--", "");
	}
	this->value = evaluatePostfixExpression(infixToPostfix(equation)); // num = -5.0 + 1 * 50 => 45 
	this->value = blaAdd(this->value, ZERO);
	string temp = this->value.substr(this->value.find(".") + 1);
	for (char& c : temp) {
		if (c != '0') {
			this->isInteger = false;
			break;
		}
	}
}

Blamath::Blamath(int num) {
	std::stringstream ss;
	ss << num;
	this->value = ss.str();
}

Blamath::Blamath(long long num) {
	std::stringstream ss;
	ss << num;
	this->value = ss.str();
}

Blamath::Blamath(unsigned int num) {
	std::stringstream ss;
	ss << num;
	this->value = ss.str();
}

Blamath::Blamath(unsigned long long num) {
	std::stringstream ss;
	ss << num;
	this->value = ss.str();
}

Blamath::Blamath(float num) {
	std::stringstream ss;
	ss << num;
	this->value = ss.str();
}

Blamath::Blamath(double num) {
	std::stringstream ss;
	ss << num;
	this->value = ss.str();
}

Blamath::Blamath(long double num) {
	std::stringstream ss;
	ss << num;
	this->value = ss.str();
}

#pragma endregion

#pragma region Operation

Blamath Blamath::operator+(const Blamath& bla) {
	Blamath r = Blamath::blaAdd(this->value, bla.value);
	if (!bla.isInteger) {
		r.isInteger = false;
	}
	return r;
}
Blamath Blamath::operator-(const Blamath& bla) {
	Blamath r = Blamath::blaSub(this->value, bla.value);
	if (!bla.isInteger) {
		r.isInteger = false;
	}
	return r;
}
Blamath Blamath::operator*(const Blamath& bla) {
	Blamath r = Blamath::blaMul(this->value, bla.value);
	if (!bla.isInteger) {
		r.isInteger = false;
	}
	return r;
}
Blamath Blamath::operator/(const Blamath& bla) {
	Blamath r = Blamath::blaDiv(this->value, bla.value);
	if (!bla.isInteger) {
		r.isInteger = false;
	}
	return r;
}
Blamath Blamath::operator%(const Blamath& bla) {
	Blamath r = Blamath::blaMod(this->value, bla.value);
	if (!bla.isInteger) {
		r.isInteger = false;
	}
	return r;
}
Blamath Blamath::operator^(const Blamath& bla) {
	Blamath r = Blamath::blaPow(this->value, bla.value);
	if (!bla.isInteger) {
		r.isInteger = false;
	}
	return r;
}

void Blamath::operator+=(const Blamath& bla) {
	this->value = Blamath::blaAdd(this->value, bla.value);
}
void Blamath::operator-=(const Blamath& bla) {
	this->value = Blamath::blaSub(this->value, bla.value);
}
void Blamath::operator*=(const Blamath& bla) {
	this->value = Blamath::blaMul(this->value, bla.value);
}
void Blamath::operator/=(const Blamath& bla) {
	this->value = Blamath::blaDiv(this->value, bla.value);
}
void Blamath::operator^=(const Blamath& bla) {
	this->value = Blamath::blaPow(this->value, bla.value);
}

bool Blamath::operator > (const Blamath& bla) {
	return Blamath::blaCompare(this->value, bla.value) > 0;
}
bool Blamath::operator >= (const Blamath& bla) {
	return Blamath::blaCompare(this->value, bla.value) >= 0;
}
bool Blamath::operator == (const Blamath& bla) {
	return Blamath::blaCompare(this->value, bla.value) == 0;
}
bool Blamath::operator < (const Blamath& bla) {
	return Blamath::blaCompare(this->value, bla.value) < 0;
}
bool Blamath::operator <= (const Blamath& bla) {
	return Blamath::blaCompare(this->value, bla.value) <= 0;
}

std::istream& operator>>(std::istream& is, Blamath& bla) {
	string equ;
	getline(is, equ);
	bla = equ;
	return is;
}
std::ostream& operator<<(std::ostream& os, const Blamath& bla) {
	if (bla.isInteger) {
		os << bla.getIntPart();
	}
	else {
		os << bla.toString();
	}

	return os;
}

#pragma endregion

#pragma region Public

std::string Blamath::toString() const {
	return this->value;
}

void Blamath::round(int scale) {
	if (scale >= 1)
		this->value = Blamath::blaRound(this->value, scale);
}

Blamath Blamath::getFactorial() {
	Blamath result(1);
	for (Blamath i(1); i <= this->value; i += 1) {
		result *= i;
	}

	return result;
}

std::string Blamath::getIntPart() const {
	std::size_t dot = this->value.find('.');
	if (dot != std::string::npos) {
		if (dot == 0)
			return std::string("0");
		if (dot == 1 && this->value[0] == '-')
			return std::string("-0");
		return this->value.substr(0, dot);
	}
	else {
		return this->value;
	}
}

std::string Blamath::getDecPart() const {
	std::size_t dot = this->value.find('.');
	if (dot != std::string::npos)
		return this->value.length() > dot + 1 ? this->value.substr(dot + 1) : std::string("0");
	else
		return std::string("0");
}

string Blamath::getValue() {
	if (this->isInteger) {
		return this->getIntPart();
	}
	else {
		return this->toString();
	}
}

void Blamath::blaScale(int scale) {
	if (scale < 0) {
		_blaScale = 0;
	}
	else {
		_blaScale = scale;
	}
}

std::string Blamath::blaDiv(const std::string& lhs, const std::string& rhs, int scale) {
	if (scale == INT_MIN) {
		scale = _blaScale;
	}
	if (scale < 0) {
		std::cerr << "Wrong parameter scale = " << scale << " in function bcdiv" << std::endl;
		scale = 0;
	}
	if (lhs.empty()) {
		return _blaZero(scale);
	}
	if (rhs.empty()) {
		std::cerr << "Division by empty " << rhs.c_str() << " in function bcdiv" << std::endl;
		return _blaZero(scale);
	}

	int lsign, lint, ldot, lfrac, lscale;
	if (_blaParseNumber(lhs, lsign, lint, ldot, lfrac, lscale) < 0) {
		std::cerr << "First parameter \"" << lhs.c_str() << "\" in function bcdiv is not a number" << std::endl;
		return ZERO;
	}

	int rsign, rint, rdot, rfrac, rscale;
	if (_blaParseNumber(rhs, rsign, rint, rdot, rfrac, rscale) < 0) {
		std::cerr << "Second parameter \"" << rhs.c_str() << "\" in function bcdiv is not a number" << std::endl;
		return ZERO;
	}

	return _blaDivPositive(lhs.c_str(), lint, ldot, lfrac, lscale, rhs.c_str(), rint, rdot, rfrac, rscale, scale, lsign * rsign);
}

std::string Blamath::blaMod(const std::string& lhs, const std::string& rhs) {
	if (lhs.empty()) {
		return ZERO;
	}
	if (rhs.empty()) {
		std::cerr << "Modulo by empty " << rhs.c_str() << " in function bcmod" << std::endl;
		return ZERO;
	}

	int lsign, lint, ldot, lfrac, lscale;
	if (_blaParseNumber(lhs, lsign, lint, ldot, lfrac, lscale) != 0) {
		std::cerr << "First parameter \"" << lhs.c_str() << "\" in function bcmod is not an integer" << std::endl;
		return ZERO;
	}

	int rsign, rint, rdot, rfrac, rscale;
	if (_blaParseNumber(rhs, rsign, rint, rdot, rfrac, rscale) != 0) {
		std::cerr << "Second parameter \"" << rhs.c_str() << "\" in function bcmod is not an integer" << std::endl;
		return ZERO;
	}

	long long mod = 0;
	for (int i = rint; i < rdot; i++) {
		mod = mod * 10 + rhs[i] - '0';
	}

	if (rdot - rint > 18 || mod == 0) {
		std::cerr << "Second parameter \"" << rhs.c_str() << "\" in function bcmod is not a non zero integer less than 1e18 by absolute this->value" << std::endl;
		return ZERO;
	}

	long long res = 0;
	for (int i = lint; i < ldot; i++) {
		res = res * 2;
		if (res >= mod) {
			res -= mod;
		}
		res = res * 5 + lhs[i] - '0';
		while (res >= mod) {
			res -= mod;
		}
	}

	char buffer[20];
	int curPos = 20;
	do {
		buffer[--curPos] = (char)(res % 10 + '0');
		res /= 10;
	} while (res > 0);

	if (lsign < 0) {
		buffer[--curPos] = '-';
	}

	return std::string(std::string(buffer + curPos).substr(0, 20 - curPos));
}

std::string Blamath::blaPow(const std::string& lhs, const std::string& rhs) {
	if (lhs.empty()) {
		return ZERO;
	}
	if (rhs.empty()) {
		return ONE;
	}

	int lsign, lint, ldot, lfrac, lscale;
	if (_blaParseNumber(lhs, lsign, lint, ldot, lfrac, lscale) != 0) {
		std::cerr << "First parameter \"" << lhs.c_str() << "\" in function bcpow is not an integer" << std::endl;
		return ZERO;
	}

	int rsign, rint, rdot, rfrac, rscale;
	if (_blaParseNumber(rhs, rsign, rint, rdot, rfrac, rscale) != 0) {
		std::cerr << "Second parameter \"" << rhs.c_str() << "\" in function bcpow is not an integer" << std::endl;
		return ZERO;
	}

	long long deg = 0;
	for (int i = rint; i < rdot; i++) {
		deg = deg * 10 + rhs[i] - '0';
	}

	if (rdot - rint > 18 || (rsign < 0 && deg != 0)) {
		std::cerr << "Second parameter \"" << rhs.c_str() << "\" in function bcpow is not a non negative integer less than 1e18" << std::endl;
		return ZERO;
	}

	if (deg == 0) {
		return ONE;
	}

	std::string result = ONE;
	std::string mul = lhs;
	while (deg > 0) {
		if (deg & 1) {
			result = blaMul(result, mul, 0);
		}
		mul = blaMul(mul, mul, 0);
		deg >>= 1;
	}

	return result;
}

std::string Blamath::blaAdd(const std::string& lhs, const std::string& rhs, int scale) {
	if (lhs.empty()) {
		return blaAdd(ZERO, rhs, scale);
	}
	if (rhs.empty()) {
		return blaAdd(lhs, ZERO, scale);
	}

	if (scale == INT_MIN) {
		scale = _blaScale;
	}
	if (scale < 0) {
		std::cerr << "Wrong parameter scale = " << scale << " in function bcadd" << std::endl;
		scale = 0;
	}

	int lsign, lint, ldot, lfrac, lscale;
	if (_blaParseNumber(lhs, lsign, lint, ldot, lfrac, lscale) < 0) {
		std::cerr << "First parameter \"" << lhs.c_str() << "\" in function bcadd is not a number" << std::endl;
		return _blaZero(scale);
	}

	int rsign, rint, rdot, rfrac, rscale;
	if (_blaParseNumber(rhs, rsign, rint, rdot, rfrac, rscale) < 0) {
		std::cerr << "Second parameter \"" << rhs.c_str() << "\" in function bcadd is not a number" << std::endl;
		return _blaZero(scale);
	}

	return _blaAdd(lhs.c_str(), lsign, lint, ldot, lfrac, lscale, rhs.c_str(), rsign, rint, rdot, rfrac, rscale, scale);
}

std::string Blamath::blaSub(const std::string& lhs, const std::string& rhs, int scale) {
	if (lhs.empty()) {
		return blaSub(ZERO, rhs, scale);
	}
	if (rhs.empty()) {
		return blaSub(lhs, ZERO, scale);
	}

	if (scale == INT_MIN) {
		scale = _blaScale;
	}
	if (scale < 0) {
		std::cerr << "Wrong parameter scale = " << scale << " in function bcsub" << std::endl;
		scale = 0;
	}

	int lsign, lint, ldot, lfrac, lscale;
	if (_blaParseNumber(lhs, lsign, lint, ldot, lfrac, lscale) < 0) {
		std::cerr << "First parameter \"" << lhs.c_str() << "\" in function bcsub is not a number" << std::endl;
		return _blaZero(scale);
	}

	int rsign, rint, rdot, rfrac, rscale;
	if (_blaParseNumber(rhs, rsign, rint, rdot, rfrac, rscale) < 0) {
		std::cerr << "Second parameter \"" << rhs.c_str() << "\" in function bcsub is not a number" << std::endl;
		return _blaZero(scale);
	}

	rsign *= -1;

	return _blaAdd(lhs.c_str(), lsign, lint, ldot, lfrac, lscale, rhs.c_str(), rsign, rint, rdot, rfrac, rscale, scale);
}

std::string Blamath::blaMul(const std::string& lhs, const std::string& rhs, int scale) {
	if (lhs.empty()) {
		return blaMul(ZERO, rhs, scale);
	}
	if (rhs.empty()) {
		return blaMul(lhs, ZERO, scale);
	}

	if (scale == INT_MIN) {
		scale = _blaScale;
	}
	if (scale < 0) {
		std::cerr << "Wrong parameter scale = " << scale << " in function bcmul" << std::endl;
		scale = 0;
	}

	int lsign, lint, ldot, lfrac, lscale;
	if (_blaParseNumber(lhs, lsign, lint, ldot, lfrac, lscale) < 0) {
		std::cerr << "First parameter \"" << lhs.c_str() << "\" in function bcmul is not a number" << std::endl;
		return ZERO;
	}

	int rsign, rint, rdot, rfrac, rscale;
	if (_blaParseNumber(rhs, rsign, rint, rdot, rfrac, rscale) < 0) {
		std::cerr << "Second parameter \"" << rhs.c_str() << "\" in function bcmul is not a number" << std::endl;
		return ZERO;
	}

	return _blaMulPositive(lhs.c_str(), lint, ldot, lfrac, lscale, rhs.c_str(), rint, rdot, rfrac, rscale, scale, lsign * rsign);
}

int Blamath::blaCompare(const std::string& lhs, const std::string& rhs, int scale) {
	if (lhs.empty()) {
		return Blamath::blaCompare(ZERO, rhs, scale);
	}
	if (rhs.empty()) {
		return Blamath::blaCompare(lhs, ZERO, scale);
	}

	if (scale == INT_MIN) {
		scale = _blaScale;
	}
	if (scale < 0) {
		std::cerr << "Wrong parameter scale = " << scale << " in function bccomp" << std::endl;
		scale = 0;
	}

	int lsign, lint, ldot, lfrac, lscale;
	if (_blaParseNumber(lhs, lsign, lint, ldot, lfrac, lscale) < 0) {
		std::cerr << "First parameter \"" << lhs.c_str() << "\" in function bccomp is not a number" << std::endl;
		return 0;
	}

	int rsign, rint, rdot, rfrac, rscale;
	if (_blaParseNumber(rhs, rsign, rint, rdot, rfrac, rscale) < 0) {
		std::cerr << "Second parameter \"" << rhs.c_str() << "\" in function bccomp is not a number" << std::endl;
		return 0;
	}

	if (lsign != rsign) {
		return (lsign - rsign) / 2;
	}

	return (1 - 2 * (lsign < 0)) * _blaComp(lhs.c_str(), lint, ldot, lfrac, lscale, rhs.c_str(), rint, rdot, rfrac, rscale, scale);
}

std::string Blamath::blaRound(const std::string& lhs, int scale) {
	if (lhs.empty()) {
		return Blamath::blaRound(ZERO, scale);
	}

	if (scale == INT_MIN) {
		scale = _blaScale;
	}

	if (scale < 0) {
		std::cerr << "Wrong parameter scale = " << scale << " in function bccomp" << std::endl;
		scale = 0;
	}

	int lsign, lint, ldot, lfrac, lscale;
	if (_blaParseNumber(lhs, lsign, lint, ldot, lfrac, lscale) < 0) {
		std::cerr << "First parameter \"" << lhs.c_str() << "\" in function bcround is not a number" << std::endl;
		return 0;
	}

	int len = lhs.size();
	std::string result(len + 1, '0');
	for (int i = len - 1; i >= lint; --i) {
		result[i + 1] = lhs[i];
	}

	char* data = (char*)malloc((result.length() + 1) * sizeof(char));
	sprintf(data, result.c_str());
	std::string ret = _blaRound(data, lint + 1, ldot + 1, lfrac + 1, lscale, scale, lsign, 1, 1);
	free(data);

	return ret;
}
#pragma endregion