int ga[2][3]={1,2,3,4,5,6};
const int gb[2][3] = {{},5,6};
int add(int i, int j) {
    return i+j;
}
int access(int a[], int n) {
    return a[n];
}

int main() {
    int k=2;
    return access(ga[1], k);
}