import java.util.Random;

public class Client {
    public static void main(String[] args) throws InterruptedException {
        Random rnd = new Random();
        ThreadPool pool = new ThreadPool();

        Thread.sleep(500);

        while (true) {
            int a = rnd.nextInt(30);
            int b = rnd.nextInt(30);
            int sleepBetweenTasks = rnd.nextInt(750) + 250;

            System.out.println(
                    "Waiting " + sleepBetweenTasks + " milliseconds before adding a task to add " + a + " and " + b);

            Thread.sleep(sleepBetweenTasks);

            pool.add(new Task(a, b));

            if (rnd.nextInt(10000) == 4) {
                System.out.println("One in 10k chance achieved, shutting down pool and exiting");
                pool.shutdown();
                break;
            }
        }
    }
}
