/* 2021_functional 测试用例集：
   012_func_defn.sy
*/
const int a = 3;
const int aa = 0x48;
const int aaa = 03333;
const float b1 = 2.3;
// float global constants
const float RADIUS = 5.5, PI = 03.141592653589793, EPS = 1e-6;

// hexadecimal float constant
const float PI_HEX = 0x1.921fb6p+1, HEX2 = 0x.AP-3;

// float constant evaluation
const float FACT = -.33E+5, EVAL1 = PI * RADIUS * RADIUS, EVAL2 = 2 * PI_HEX * RADIUS,
            EVAL3 = PI * 2 * RADIUS;

// float constant implicit conversion
const float CONV1 = 233, CONV2 = 0xfff;

const int   cccccccc = 2 * 31 * 1847;
const float dddddddd = (cccccccc * 2) / (31 * 1847) / 2;

float     bb = 2.3, cc = 3.3;
const int c[2][3]  = {{1, 2, 3}, {4, 5, 6}};
float     d[1 * 2] = {1.1, 2.2};

int func(int p)
{
    p = p - 1;
    return p;
}

int main()
{
    int b;
    b = func(a);
    return b;
}