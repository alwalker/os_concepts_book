import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;

enum State {
    THINKING, HUNGRY, EATING
}

public class DiningPhilosopher implements DiningServer, Runnable {

    int id;
    Lock lock;
    Condition[] philosopherConditions;
    State[] philosopherStates;
    java.util.Random rand;

    public DiningPhilosopher(int id, Lock lock, State[] philosopherStates, Condition[] philosopherConditions) {
        this.id = id;
        this.lock = lock;
        rand = new java.util.Random();
        this.philosopherConditions = philosopherConditions;
        this.philosopherStates = philosopherStates;
    }

    public void takeForks(int i) {
        philosopherStates[i] = State.HUNGRY;
        test(i);
        if (philosopherStates[i] != State.EATING) {
            try {
                System.out.println("Philosopher " + i + " is waiting for forks");
                philosopherConditions[i].await();
            } catch (InterruptedException iex) {
                System.out.println("Philosopher " + i + " got interrupted taking forks");
            }
        }
    }

    public void returnForks(int i) {
        philosopherStates[i] = State.THINKING;

        if (rand.nextBoolean()) {
            test((i + 4) % 5);
            test((i + 1) % 5);
        } else {
            test((i + 1) % 5);
            test((i + 4) % 5);
        }
    }

    void test(int i) {
        if ((philosopherStates[(i + 4) % 5] != State.EATING) && (philosopherStates[i] == State.HUNGRY)
                && (philosopherStates[(i + 1) % 5] != State.EATING)) {
            System.out.println("Philosopher " + i + " is free to eat");
            philosopherStates[i] = State.EATING;
            philosopherConditions[i].signal();
        }
    }

    public void run() {
        System.out.println("Philosopher " + id + " thinking...");

        while (true) {
            try {
                Thread.sleep(rand.nextInt(9000) + 1000);
            } catch (InterruptedException e) {
                System.out.println("Philosopher " + id + " got interrupted thinking");
            }

            lock.lock();
            try {
                System.out.println("Philosopher " + id + " is hungry, taking forks");
                takeForks(id);
            } catch (Exception ex) {
                System.out.println("Philosopher " + id + " threw trying to take forks");
                System.out.print(ex.getMessage());
            } finally {
                lock.unlock();
            }

            try {
                Thread.sleep(5000);
            } catch (InterruptedException e) {
                System.out.println("Philosopher " + id + " got interrupted eating");
            }

            lock.lock();
            try {
                System.out.println("Philosopher " + id + " has eaten, putting down forks");
                returnForks(id);
            } catch (Exception ex) {
                System.out.println("Philosopher " + id + " threw trying to return forks");
                System.out.print(ex.getMessage());
            } finally {
                lock.unlock();
            }
        }
    }
}