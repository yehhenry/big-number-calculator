# BlaMath

__Blabla - A Big Number Calculate__

## 1. GUI (Support macOS, Windows)
__檔案在一個叫GUI的資料夾裡__

```
macOS: ./GUI/BlaCalculate.app
最低系統要求: macOS 12.0

Windows: ./GUI/葉柏亨把路徑補上來，icon記得換
最低系統要求：Windows 11
```

## 2. build the app by yourself

make sure you have installed xcode-commend-tool

```shell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

> **Note**: _To force CMake to generate 64-bit projects on Windows, use `cmake .. -DCMAKE_GENERATOR_PLATFORM=x64` instead of `cmake ..`_


## 3. Run the app
__If the Application are build by yourself__

### On macOS and Linux

Navigate to `ultralight-quick-start/build` and run `MyApp` to launch the program.

### On Windows

Navigate to `ultralight-quick-start/build/Release` and run `MyApp` to launch the program.

## 4. Usage
```c++
#include <iostream>
#include <string>
#include "Blamath.h"
#include "Decimal.h"
#include "Integer.h"

using namespace std;

Blamath bla = "-((-5.0+1*50)*5/(5*3*-1))";
cout << bla.originalEquation << ": \n" << bla.getValue() << endl;
// -((-5.0+1*50)*5/(5*3*-1)): 
// 15

Blamath bla2;
cin >> bla2;
cout << bla2; // calc equation from cin data

// *************************
// ********* API 1 *********
// *************************
Integer i;
Decimal d;
cin >> i >> d;
cout << i + d;

// *************************
// ********* API 2 *********
// *************************
Integer i = "123";
Decimal d = "0.3 * 3";

// *************************
// ********* API 3 *********
// *************************
Integer i("123");
Decimal d("123.3");
```

## 5. Other Code Test
```c++
Blamath::blaScale(4); // Num Decimals
std::cout << "Other 1: " << Blamath::blaMul("1000000.0134", "8.0234").c_str() << std::endl;
std::cout << "Other 2: " << Blamath::blaAdd("1000000.0134", "8.0234").c_str() << std::endl;

std::cout << "Compare 1:  " << Blamath::blaCompare("1", "2") << std::endl;
std::cout << "Compare 2:  " << Blamath::blaCompare("1.00001", "1", 3) << std::endl;
std::cout << "Compare 3:  " << Blamath::blaCompare("1.00001", "1", 5) << std::endl;
std::cout << "Compare 4:  " << (Blamath("1") <  Blamath("2")) << std::endl;
std::cout << "Compare 5:  " << (Blamath("1") <= Blamath("2")) << std::endl;
std::cout << "Compare 6:  " << (Blamath("1") >  Blamath("2")) << std::endl;
std::cout << "Compare 7:  " << (Blamath("1") >= Blamath("2")) << std::endl;
std::cout << "Compare 8:  " << (Blamath("2") <  Blamath("2")) << std::endl;
std::cout << "Compare 9:  " << (Blamath("2") <= Blamath("2")) << std::endl;
std::cout << "Compare 10: " << (Blamath("2") >  Blamath("2")) << std::endl;
std::cout << "Compare 11: " << (Blamath("2") >= Blamath("2")) << std::endl;

std::cout << "Round 1: " << Blamath::blaRound("123.01254").c_str() << std::endl;
std::cout << "Round 2: " << Blamath::blaRound("-123.01254", 3).c_str() << std::endl;
std::cout << "Round 3: " << Blamath::blaRound("123.01254", 2).c_str() << std::endl;
pi.round(3);
std::cout << "Round 4: " << pi.toString().c_str() << std::endl;

Blamath part1("-.123");
Blamath part2(".123");
Blamath part3("123");
std::cout << "Int part 1: " << part1.getIntPart().c_str() << std::endl;
std::cout << "Dec part 1: " << part1.getDecPart().c_str() << std::endl;
std::cout << "Int part 2: " << part2.getIntPart().c_str() << std::endl;
std::cout << "Dec part 2: " << part2.getDecPart().c_str() << std::endl;
std::cout << "Int part 3: " << part3.getIntPart().c_str() << std::endl;
std::cout << "Dec part 3: " << part3.getDecPart().c_str() << std::endl;
```
## 6. Other Code Test Result
```
Result BigDecimal 1:  198005530669253749533290222782634796336450786581284861381777714804795900171726938603997395193921984842256586113024
Result BigDecimal 2:  198005530669253749533290222782634796336450786581284861381777714804795900171726938603997395193921984842256586113022.7700
Result BigDecimal 3:  243546802723182111925946974022640799493834467494980379499586589209898957211224134482916796088524041355975600919018.0071
Result BigDecimal 4:  -299562567349513997668914778047848183377416395018825866784491504728175717369805685413987659188884570867849989130392.1487

Result Super Precision 1:  -5943534512345234542.7583001856941315459727023167204971158028306006248941790250554076921835
Result Super Precision 2:  -5943534512345234549.0414854928737180228979890832795028841971693993751058209749445923078164
Result Super Precision 3:  -18672164360341183116.9114783895073349180904753962992796943871920962352436079118338887287186
Result Super Precision 4:  -1891885794154043400.2804849527556211973567525043250278948318788149660700494315139982452600

Other 1:  8023400.1075
Other 2:  1000008.0368

Compare 1:   -1
Compare 2:   0
Compare 3:   1
Compare 4:   true
Compare 5:   true
Compare 6:   false
Compare 7:   false
Compare 8:   false
Compare 9:   true
Compare 10:  false
Compare 11:  true

Round 1:  123.0125
Round 2:  -123.013
Round 3:  123.01
Round 4:  3.142

Int part 1:  -0
Dec part 1:  123
Int part 2:  0
Dec part 2:  123
Int part 3:  123
Dec part 3:  0
```