import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.Semaphore;
import java.util.concurrent.locks.ReentrantLock;

public class ThreadPool {

	Queue<Runnable> taskQueue;
	ReentrantLock queueLock;
	Semaphore queueSemaphore;
	List<Thread> pool;

	public ThreadPool() throws InterruptedException {
		this.taskQueue = new LinkedList<Runnable>();
		pool = new ArrayList<Thread>(10);
		queueLock = new ReentrantLock();
		queueSemaphore = new Semaphore(10);

		StartPool(10);
	}

	public ThreadPool(int size) throws InterruptedException{
		this.taskQueue = new LinkedList<Runnable>();
		pool = new ArrayList<Thread>(size);
		queueLock = new ReentrantLock();
		queueSemaphore = new Semaphore(size);

		StartPool(size);
	}

	public void shutdown() {
		for (Thread Thread : pool) {
			Thread.interrupt();
		}
	}

	public void add(Runnable task) throws InterruptedException {
		System.out.println("Adding task");
		queueLock.lock();

		try {
			taskQueue.add(task);
		}
		catch(Exception e) {
			System.err.println("Error adding to task queue!");
			System.err.println(e);
		}
		finally {
			queueLock.unlock();
		}

		queueSemaphore.release();
	}

	private void StartPool(int size) throws InterruptedException {
		queueSemaphore.acquire(size);

		for (int i = 0; i < size; i++) {
			Thread worker = new Thread(new Worker(i, taskQueue, queueLock, queueSemaphore));
			worker.start();
			pool.add(worker);
		}
	}

}
