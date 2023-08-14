import java.util.ArrayList;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;

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

public class PriorityRR implements Algorithm {
    int slice, currentTaskQueueId, currentTaskId;
    List<Task> input;
    List<List<Task>> taskQueues;

    public PriorityRR(List<Task> queue) {
        currentTaskId = 0;
        currentTaskQueueId = 0;
        slice = 10;

        input = queue;

        taskQueues = new ArrayList<List<Task>>();
        for (int i = 0; i < 10; i++) {
            taskQueues.add(new LinkedList<Task>());
        }
    }

    @Override
    public void schedule() {
        for (Task task : input) {
            taskQueues.get(task.getPriority() - 1).add(task);
        }

        Task currentTask = pickNextTask();

        while (currentTask != null) {
            CPU.run(currentTask, 10);

            currentTask = pickNextTask();
        }
    }

    @Override
    public Task pickNextTask() {
        Task currentTask = taskQueues.get(currentTaskQueueId).get(currentTaskId);
        int currentBurst = currentTask.getBurst();
        int runtime = slice < currentBurst ? slice : currentBurst;

        currentTask.setBurst(currentBurst - runtime);

        if (currentTask.getBurst() <= 0) {
            taskQueues.get(currentTaskQueueId).remove(currentTaskId);

            if (!taskQueues.get(currentTaskQueueId).isEmpty()) {
                currentTaskId = (currentTaskId + 1) % taskQueues.get(currentTaskQueueId).size();
            } else {
                currentTaskId = 0;
                int index = 0;
                while (taskQueues.get(currentTaskQueueId).isEmpty()) {
                    currentTaskQueueId = (currentTaskQueueId + 1) % 10;
                    index++;

                    if(index == 10) {
                        return null;
                    }
                }
            }
        }
        else {
            currentTaskId = (currentTaskId + 1) % taskQueues.get(currentTaskQueueId).size();
        }

        return currentTask;

    }
}
