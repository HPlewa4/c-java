package functions;

public class RemoveEvenNumbers {
    public static void printWithoutEvens(int[] arr) {
        for (int num : arr) {
            if (num % 2 != 0) System.out.print(num + " ");
        }
        System.out.println();
    }
}
