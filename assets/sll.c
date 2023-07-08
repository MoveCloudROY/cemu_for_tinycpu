int main() {
    int          n = 3;
    while (n <= (1<<15)) {
        n <= 1;
    }

    volatile int keep_spinning = 1;
    while (keep_spinning) {
    }; // spin
}
// 1 1 2 3 5 8 13 21 34 55
