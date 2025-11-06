#include "Bridge.h"
#include <iostream>
#include <string>
#include "functions/prime.cpp"
#include "functions/fib.cpp"
#include "functions/randString.cpp"
using namespace std;

// ---- PRIME ----
JNIEXPORT jint JNICALL Java_Bridge_cppNextPrime(JNIEnv *, jobject, jint n) {
    return nextOrSamePrime(n);
}

// ---- RANDOM STRING ----
JNIEXPORT jstring JNICALL Java_Bridge_cppGenerateRandomString(JNIEnv* env, jobject, jint len) {
    string rand = generateRandomString(len);
    return env->NewStringUTF(rand.c_str());
}

// ---- FIBONACCI ----
JNIEXPORT jintArray JNICALL Java_Bridge_cppGenerateFibonacci(JNIEnv* env, jobject, jint count) {
    jintArray arr = env->NewIntArray(count);
    int* fib = new int[count];
    generateFibonacci(fib, count);
    env->SetIntArrayRegion(arr, 0, count, fib);
    delete[] fib;
    return arr;
}
