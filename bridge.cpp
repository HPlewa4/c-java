#include "Bridge.h"
#include <iostream>
#include "functions/prime.cpp"   // include your prime logic

using namespace std;

JNIEXPORT jint JNICALL Java_Bridge_cppNextPrime(JNIEnv *, jobject, jint n) {
    int prime = nextOrSamePrime(n);
    cout << "[C++] Received " << n << " → returning prime " << prime << endl;
    return prime;
}
