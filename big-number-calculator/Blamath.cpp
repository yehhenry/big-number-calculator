#define BLA_ASSERT(cond) ((!(cond)) ? blaAssert(#cond,__FILE__,__LINE__) : blaNoassert())

#include "Blamath.h"
#include <string.h>
#include <stdlib.h>

#pragma region Internal

static int _blaScale = 100;

static const std::string ONE("1");
static const std::string ZERO("0");

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

static std::string _blaRound(char* lhs, int lint, int ldot, int lfrac, int lscale, int scale, int sign, bool add_trailing_zeroes, bool round_last = false) {
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

	if (round_last) {
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

	if (lscale == scale || !add_trailing_zeroes) {
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

	int result_len = std::max(llen, rlen) + 1;
	int result_scale = std::max(lscale, rscale);
	int result_size = result_len + result_scale + 3;
	std::string result(result_size, '0');

	int i, um = 0;
	int cur_pos = result_size;
	int was_frac = 0;
	for (i = result_scale - 1; i >= 0; i--) {
		if (i < lscale) {
			um += lhs[lfrac + i] - '0';
		}
		if (i < rscale) {
			um += rhs[rfrac + i] - '0';
		}

		if (um != 0 || was_frac) {
			result[--cur_pos] = (char)(um % 10 + '0');
			um /= 10;
			was_frac = 1;
		}
	}
	resscale = result_size - cur_pos;
	resfrac = cur_pos;
	if (was_frac) {
		result[--cur_pos] = '.';
	}
	resdot = cur_pos;

	for (int i = 0; i < result_len; i++) {
		if (i < llen) {
			um += lhs[ldot - i - 1] - '0';
		}
		if (i < rlen) {
			um += rhs[rdot - i - 1] - '0';
		}

		result[--cur_pos] = (char)(um % 10 + '0');
		um /= 10;
	}
	resint = cur_pos;
	BLA_ASSERT(cur_pos > 0);

	return _blaRound((char*)result.data(), resint, resdot, resfrac, resscale, scale, sign, 1);
}

static std::string _blaSubPositive(const char* lhs, int lint, int ldot, int lfrac, int lscale, const char* rhs, int rint, int rdot, int rfrac, int rscale, int scale, int sign) {
	int llen = ldot - lint;
	int rlen = rdot - rint;

	int resint, resdot, resfrac, resscale;

	int result_len = llen;
	int result_scale = std::max(lscale, rscale);
	int result_size = result_len + result_scale + 3;
	std::string result(result_size, '0');

	int i, um = 0, next_um = 0;
	int cur_pos = result_size;
	int was_frac = 0;
	for (i = result_scale - 1; i >= 0; i--) {
		um = next_um;
		if (i < lscale) {
			um += lhs[lfrac + i] - '0';
		}
		if (i < rscale) {
			um -= rhs[rfrac + i] - '0';
		}
		if (um < 0) {
			next_um = -1;
			um += 10;
		}
		else {
			next_um = 0;
		}

		if (um != 0 || was_frac) {
			result[--cur_pos] = (char)(um + '0');
			was_frac = 1;
		}
	}
	resscale = result_size - cur_pos;
	resfrac = cur_pos;
	if (was_frac) {
		result[--cur_pos] = '.';
	}
	resdot = cur_pos;

	for (int i = 0; i < result_len; i++) {
		um = next_um;
		um += lhs[ldot - i - 1] - '0';
		if (i < rlen) {
			um -= rhs[rdot - i - 1] - '0';
		}
		if (um < 0) {
			next_um = -1;
			um += 10;
		}
		else {
			next_um = 0;
		}

		result[--cur_pos] = (char)(um + '0');
	}
	resint = cur_pos;
	BLA_ASSERT(cur_pos > 0);

	return _blaRound((char*)result.data(), resint, resdot, resfrac, resscale, scale, sign, 1);
}

static std::string _blaMulPositive(const char* lhs, int lint, int ldot, int lfrac, int lscale, const char* rhs, int rint, int rdot, int rfrac, int rscale, int scale, int sign) {
	int llen = ldot - lint;
	int rlen = rdot - rint;

	int resint, resdot, resfrac, resscale;

	int result_len = llen + rlen;
	int result_scale = lscale + rscale;
	int result_size = result_len + result_scale + 3;
	std::string result(result_size, '0');

	int* res = (int*)malloc(sizeof(int) * result_size);
	memset(res, 0, sizeof(int) * result_size);
	for (int i = -lscale; i < llen; i++) {
		int x = (i < 0 ? lhs[lfrac - i - 1] : lhs[ldot - i - 1]) - '0';
		for (int j = -rscale; j < rlen; j++) {
			int y = (j < 0 ? rhs[rfrac - j - 1] : rhs[rdot - j - 1]) - '0';
			res[i + j + result_scale] += x * y;
		}
	}
	for (int i = 0; i + 1 < result_size; i++) {
		res[i + 1] += res[i] / 10;
		res[i] %= 10;
	}

	int cur_pos = result_size;
	for (int i = 0; i < result_scale; i++) {
		result[--cur_pos] = (char)(res[i] + '0');
	}
	resscale = result_size - cur_pos;
	resfrac = cur_pos;
	if (result_scale > 0) {
		result[--cur_pos] = '.';
	}
	resdot = cur_pos;

	for (int i = result_scale; i < result_len + result_scale; i++) {
		result[--cur_pos] = (char)(res[i] + '0');
	}
	resint = cur_pos;
	BLA_ASSERT(cur_pos > 0);

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

	int result_len = std::max(llen + rscale - rlen + 1, 1);
	int result_scale = scale;
	int result_size = result_len + result_scale + 3;

	if (rscale == 0 && rhs[rint] == '0') {
		std::cerr << ("Division by zero in function bcdiv") << std::endl << std::endl;
		return ZERO;
	}

	int dividend_len = llen + lscale;
	int divider_len = rlen + rscale;
	int* dividend = (int*)malloc(sizeof(int) * (result_size + dividend_len + divider_len));
	int* divider = (int*)malloc(sizeof(int) * divider_len);
	memset(dividend, 0, sizeof(int) * (result_size + dividend_len + divider_len));
	memset(divider, 0, sizeof(int) * divider_len);

	for (int i = -lscale; i < llen; i++) {
		int x = (i < 0 ? lhs[lfrac - i - 1] : lhs[ldot - i - 1]) - '0';
		dividend[llen - i - 1] = x;
	}

	for (int i = -rscale; i < rlen; i++) {
		int x = (i < 0 ? rhs[rfrac - i - 1] : rhs[rdot - i - 1]) - '0';
		divider[rlen - i - 1] = x;
	}

	int divider_skip = 0;
	while (divider_len > 0 && divider[0] == 0) {
		divider++;
		divider_skip++;
		divider_len--;
	}
	BLA_ASSERT(divider_len > 0);

	int cur_pow = llen - rlen + divider_skip;
	int cur_pos = 2;

	if (cur_pow < -scale) {
		divider -= divider_skip;
		divider_len += divider_skip;
		free(dividend);
		free(divider);
		return _blaZero(scale);
	}

	std::string result(result_size, '0');
	resint = cur_pos;
	if (cur_pow < 0) {
		result[cur_pos++] = '0';
		resdot = cur_pos;
		result[cur_pos++] = '.';
		resfrac = cur_pos;
		for (int i = -1; i > cur_pow; i--) {
			result[cur_pos++] = '0';
		}
	}

	int beg = 0, real_beg = 0;
	while (cur_pow >= -scale) {
		char dig = '0';
		while (true) {
			if (real_beg < beg && dividend[real_beg] == 0) {
				real_beg++;
			}

			bool less = false;
			if (real_beg == beg) {
				for (int i = 0; i < divider_len; i++) {
					if (dividend[beg + i] != divider[i]) {
						less = (dividend[beg + i] < divider[i]);
						break;
					}
				}
			}
			if (less) {
				break;
			}

			for (int i = divider_len - 1; i >= 0; i--) {
				dividend[beg + i] -= divider[i];
				if (dividend[beg + i] < 0) {
					dividend[beg + i] += 10;
					dividend[beg + i - 1]--;
				}
			}
			dig++;
		}

		result[cur_pos++] = dig;

		if (cur_pow == 0) {
			resdot = cur_pos;
			if (scale > 0) {
				result[cur_pos++] = '.';
			}
			resfrac = cur_pos;
		}
		cur_pow--;
		beg++;
	}
	resscale = cur_pos - resfrac;

	divider -= divider_skip;
	divider_len += divider_skip;
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

Blamath::Blamath() { }

Blamath::Blamath(const Blamath& bla) {
	this->value = bla.value;
}

Blamath::Blamath(const char* num) {
	this->value = num;
}

Blamath::Blamath(std::string num) {
	this->value = num;
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
	return Blamath::blaAdd(this->value, bla.value);
}
Blamath Blamath::operator-(const Blamath& bla) {
	return Blamath::blaSub(this->value, bla.value);
}
Blamath Blamath::operator*(const Blamath& bla) {
	return Blamath::blaMul(this->value, bla.value);
}
Blamath Blamath::operator/(const Blamath& bla) {
	return Blamath::blaDiv(this->value, bla.value);
}
Blamath Blamath::operator%(const Blamath& bla) {
	return Blamath::blaMod(this->value, bla.value);
}
Blamath Blamath::operator^(const Blamath& bla) {
	return Blamath::blaPow(this->value, bla.value);
}
Blamath Blamath::operator=(const std::string& bla) {
	//TODO =
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

std::istream& operator>>(std::istream& is, Blamath& o) {
	//TODO >>
	is >> o;

	return is;
}
std::ostream& operator<<(std::ostream& os, Blamath& o) {
	if (o.isInteger) {
		os << o.getIntPart();
	}
	else {
		os << o.toString();
	}

	return os;
}

#pragma endregion

#pragma region Public

std::string Blamath::toString() {
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

std::string Blamath::getIntPart() {
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

std::string Blamath::getDecPart() {
	std::size_t dot = this->value.find('.');
	if (dot != std::string::npos)
		return this->value.length() > dot + 1 ? this->value.substr(dot + 1) : std::string("0");
	else
		return std::string("0");
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
	int cur_pos = 20;
	do {
		buffer[--cur_pos] = (char)(res % 10 + '0');
		res /= 10;
	} while (res > 0);

	if (lsign < 0) {
		buffer[--cur_pos] = '-';
	}

	return std::string(std::string(buffer + cur_pos).substr(0, 20 - cur_pos));
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