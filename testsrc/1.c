int ga[8][8][8];
int foo2(int arg1[]) {
    int i[12];
    foo2(arg1);
    foo2(i);
}
int foo1(int arg1[][8]) {
    foo2(arg1[1]);
    foo1(arg1);
    return arg1[1][2];
}
int foo3(int arg1[][8][8]) {
    return arg1[1][2][3];
}

int main() {
    int ma[10][8];
    int i, j;
    i=ga[1][2][3];
    i=foo1(ga[6]);
    i=foo2(ga[1][2]);
    i=foo3(ga);
    return i+j;
}