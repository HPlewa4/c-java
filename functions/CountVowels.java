package functions;

public class CountVowels {
    public static int countVowels(String str) {
        int count = 0;
        str = str.toLowerCase(); // make it case-insensitive

        for (int i = 0; i < str.length(); i++) {
            char ch = str.charAt(i);
            if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u') {
                count++;
            }
        }
        return count;
    }

    public static void main(String[] args) {
        String input = "Hello World";
        System.out.println("Number of vowels in \"" + input + "\": " + countVowels(input));
    }
}
