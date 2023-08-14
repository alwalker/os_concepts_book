import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

class BurstComp implements Comparator<Task> {
    @Override
    public int compare(Task lhs, Task rhs) {
        if (lhs.getBurst() > rhs.getBurst()) {
            return 1;
        } else {
            return -1;
        }
    }
}

public class SJF implements Algorithm {

    List<Task> input;
    Queue<Task> taskQueue;

    public SJF(List<Task> queue) {
        input = queue;
    }

    @Override
    public void schedule() {
        input.sort(new BurstComp());
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
