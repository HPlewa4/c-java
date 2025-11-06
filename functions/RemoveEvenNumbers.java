package functions;

import java.util.Scanner;
import java.util.ArrayList;

public class RemoveEvenNumbers {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);

        System.out.print("Enter size of array: ");
        int n = sc.nextInt();

        int[] arr = new int[n];
        System.out.println("Enter " + n + " numbers:");
        for (int i = 0; i < n; i++) {
            arr[i] = sc.nextInt();
        }

        ArrayList<Integer> result = new ArrayList<>();

        // Keep only odd numbers
        for (int num : arr) {
            if (num % 2 != 0) {
                result.add(num);
            }
        }

        System.out.println("Array after removing even numbers:");
        for (int num : result) {
            System.out.print(num + " ");
        }
        System.out.println();

        sc.close();
    }
}
