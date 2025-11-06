import functions.DecimalToBinary;
import functions.CountVowels;
import functions.RemoveEvenNumbers;

import java.util.Arrays;

public class Bridge {
    static {
        System.loadLibrary("bridge");
    }

    // Native methods
    private native int cppNextPrime(int n);
    private native String cppGenerateRandomString(int length);
    private native int[] cppGenerateFibonacci(int count);

    public static void main(String[] args) {
        Bridge b = new Bridge();

        // 1️⃣ Find next prime and convert to binary
        int input = 16;
        int prime = b.cppNextPrime(input);
        String binary = DecimalToBinary.toBinary(prime);
        System.out.println("Input: " + input);
        System.out.println("Next/Same Prime: " + prime);
        System.out.println("Binary: " + binary);

        // 2️⃣ Generate a random string in C++ and count vowels in Java
        String rand = b.cppGenerateRandomString(12);
        int vowelCount = CountVowels.countVowels(rand);
        System.out.println("\nRandom String: " + rand);
        System.out.println("Vowel Count: " + vowelCount);

        // 3️⃣ Generate Fibonacci numbers (C++) and remove evens (Java)
        int[] fib = b.cppGenerateFibonacci(20);
        System.out.println("\nFibonacci sequence (20 terms): " + Arrays.toString(fib));

        System.out.println("After removing even numbers:");
        RemoveEvenNumbers.printWithoutEvens(fib);
    }
}
