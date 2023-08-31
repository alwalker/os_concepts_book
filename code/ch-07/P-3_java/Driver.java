import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Driver {

    public static void main(String[] args) {
        Condition[] philosopherConditions = new Condition[5];
        State[] philosopherStates = new State[5];
        Lock lock = new ReentrantLock();

        for (int i = 0; i < 5; i++) {
            philosopherStates[i] = State.THINKING;
            philosopherConditions[i] = lock.newCondition();
        }

        for (int i = 0; i < 5; i++) {
			Thread philosopher = new Thread(new DiningPhilosopher(i, lock, philosopherStates, philosopherConditions));
			philosopher.start();
		}
    }
}
