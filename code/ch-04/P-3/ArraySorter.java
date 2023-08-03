import java.util.Arrays;
import java.util.concurrent.*;

public class ArraySorter extends RecursiveTask<int[]> {
    static final int SIZE = 100;
    static final int THRESHOLD = 10;

    private int begin;
    private int end;
    private int[] array;

    public ArraySorter(int begin, int end, int[] array) {
        this.begin = begin;
        this.end = end;
        this.array = array;
    }

    protected int[] compute() {
        if (end - begin < THRESHOLD) {
            var subArray = Arrays.copyOfRange(array, begin, end + 1);
            Arrays.sort(subArray);

            return subArray;
        } else {
            int mid = begin + (end - begin) / 2;

            ArraySorter leftTask = new ArraySorter(begin, mid, array);
            ArraySorter rightTask = new ArraySorter(mid + 1, end, array);

            leftTask.fork();
            rightTask.fork();

            int[] left = leftTask.join();
            int left_index = 0;
            int[] right = rightTask.join();
            int right_index = 0;
            int[] sorted = new int[left.length + right.length];

            for (int i = 0; i < left.length + right.length; i++) {
                if (left_index >= left.length && right_index >= right.length) {
                    break;
                }
                else if (left_index >= left.length && right_index < right.length) {
                    sorted[i] = right[right_index];
                    right_index++;
                } else if (right_index >= right.length && left_index < left.length) {
                    sorted[i] = left[left_index];
                    left_index++;
                } else if (left[left_index] <= right[right_index]) {
                    sorted[i] = left[left_index];
                    left_index++;
                } else if (left[left_index] >= right[right_index]) {
                    sorted[i] = right[right_index];
                    right_index++;
                } else {
                    System.out.println("We done fucked up");
                    break;
                }
            }

            return sorted;
        }
    }

    public static void main(String[] args) {
        ForkJoinPool pool = new ForkJoinPool();

        int[] unsorted = new int[SIZE];
        // int[] unsorted = { 7, 12, 19, 3, 18, 4, 2, 6, 15, 8 };

        java.util.Random rand = new java.util.Random();
        for (int i = 0; i < SIZE; i++) {
            unsorted[i] = rand.nextInt(10);
        }

        ArraySorter task = new ArraySorter(0, SIZE - 1, unsorted);

        var sorted = pool.invoke(task);

        System.out.println("Started with: " + Arrays.toString(unsorted));
        System.out.println("Result: " + Arrays.toString(sorted));
    }
}
