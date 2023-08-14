import java.util.List;

public class RR implements Algorithm {

    int slice, currentTaskId;
    List<Task> input;

    public RR(List<Task> queue) {
        currentTaskId = 0;
        slice = 10;
        input = queue;
    }

    @Override
    public void schedule() {
        Task currentTask = pickNextTask();

        while (currentTask != null) {
            CPU.run(currentTask, 10);

            currentTask = pickNextTask();
        }
    }

    @Override
    public Task pickNextTask() {
        if (input.isEmpty()) {
            return null;
        }

        Task currentTask = input.get(currentTaskId);
        int currentBurst = currentTask.getBurst();
        int runtime = slice < currentBurst ? slice : currentBurst;
        currentTask.setBurst(currentBurst - runtime);

        if (currentTask.getBurst() <= 0) {
            input.remove(currentTaskId);
        }
        else if (!input.isEmpty()) {
            currentTaskId = (currentTaskId + 1) % input.size();
        }

        return currentTask;

    }
}
