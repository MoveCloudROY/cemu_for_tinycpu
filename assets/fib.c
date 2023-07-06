int fib(int n, int ret1, int ret2) {
    if (n == 0) {
        return ret1;
    } else {
        return fib(n - 1, ret2, ret1 + ret2);
    }
}

int main() {
    int          n = 10;
    volatile int a = fib(n, 0, 1);

    volatile int keep_spinning = 1;
    while (keep_spinning) {
    }; // spin
}
// 1 1 2 3 5 8 13 21 34 55