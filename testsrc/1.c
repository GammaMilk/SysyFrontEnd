/* 2021_functional 测试用例集：
   012_func_defn.sy
*/
int a;

int func(int p)
{
    p = p - 1;
    return p;
}

int main()
{
    int b;
    a = 10;
    b = func(a);
    return b;
}