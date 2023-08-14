import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

public class FCFS implements Algorithm {

    List<Task> input;
    Queue<Task> taskQueue;

    public FCFS(List<Task> queue) {
        input = queue;
    }

    @Override
    public void schedule() {
        taskQueue = new LinkedList<Task>();
        for (Task task : input) {
            taskQueue.add(task);
        }

        Task currentTask = pickNextTask();
        while (currentTask != null) {
            CPU.run(currentTask, 0);

            currentTask = pickNextTask();
        }
    }

    @Override
    public Task pickNextTask() {
        if (taskQueue.isEmpty()) {
            return null;
        } else {
            return taskQueue.remove();
        }
    }
}
