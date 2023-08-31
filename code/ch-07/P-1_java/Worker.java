import java.util.Queue;
import java.util.concurrent.Semaphore;
import java.util.concurrent.locks.ReentrantLock;

public class Worker implements Runnable {
    int id;
    Queue<Runnable> taskQueue;
    ReentrantLock queueLock;
    Semaphore queueSemaphore;

    public Worker(int id, Queue<Runnable> taskQueue, ReentrantLock queueLock, Semaphore queueSemaphore) {
        this.id = id;
        this.taskQueue = taskQueue;
        this.queueLock = queueLock;
        this.queueSemaphore = queueSemaphore;
    }

    public void doWork() throws InterruptedException {
        Runnable task = null;

        queueLock.lock();

        try {
            System.out.println("Thread " + id + " is looking on the queue for work");
            task = taskQueue.poll();
        } catch (Exception e) {
            System.err.println("Error getting task from queue!");
            System.err.println(e);
        } finally {
            queueLock.unlock();
        }

        if (task != null) {
            task.run();
        }
    }

    public void run() {
        try {
            while (!Thread.currentThread().isInterrupted()) {
                System.out.println("Thread " + id + " looking for work...");
                queueSemaphore.acquire();
                doWork();
            }
        } catch (InterruptedException ie) {
            System.out.println("Thread " + id + " was interrupted");
        }
    }
}