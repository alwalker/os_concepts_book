import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

class PriorityComp implements Comparator<Task> {
    @Override
    public int compare(Task lhs, Task rhs) {
        if (lhs.getPriority() > rhs.getPriority()) {
            return 1;
        } else {
            return -1;
        }
    }
}

public class Priority implements Algorithm {

    List<Task> input;
    Queue<Task> taskQueue;

    public Priority(List<Task> queue) {
        input = queue;
    }

    @Override
    public void schedule() {
        input.sort(new PriorityComp());
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
