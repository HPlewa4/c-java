#include <iostream>
using namespace std;

void generateFibonacci(int fib[], int n) {
    if (n <= 0) return;

    fib[0] = 0;
    if (n == 1) return;

    fib[1] = 1;

    for (int i = 2; i < n; i++) {
        fib[i] = fib[i - 1] + fib[i - 2];
    }
}
