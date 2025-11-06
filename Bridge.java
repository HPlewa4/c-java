import functions.DecimalToBinary;

public class Bridge {
    static {
        System.loadLibrary("bridge");
    }

    private native int cppNextPrime(int n);

    public static void main(String[] args) {
        Bridge b = new Bridge();

        int input = 16;
        int prime = b.cppNextPrime(input);
        String binary = DecimalToBinary.toBinary(prime);

        System.out.println("Input: " + input);
        System.out.println("Next/Same Prime: " + prime);
        System.out.println("Binary: " + binary);
    }
}
