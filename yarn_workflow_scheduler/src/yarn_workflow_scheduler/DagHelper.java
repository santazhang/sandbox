package yarn_workflow_scheduler;

import java.util.*;

/**
 * Created by santa on 4/22/16.
 */
public class DagHelper {

    private int maxRetryPerJob = 5;

    private enum JobStatus {
        NOT_SCHEDULED,
        RUNNING,
        FAILED,
        SUCCEEDED,
    }

    private static class DagJob {
        String name = "";
        int retries = 0;
        String containerName = "";
        JobStatus status = JobStatus.NOT_SCHEDULED;

        Set<String> depends = new HashSet<String>();
        Set<String> precedes = new HashSet<String>();
        int blocked = 0;
    }

    Map<String, DagJob> jobs = new HashMap<String, DagJob>();
    Set<String> candidate = new HashSet<String>();


    private DagHelper() { }

    public static DagHelper createFromString(String dagTxt) {
        DagHelper dag = new DagHelper();

        for (String line : dagTxt.split("\n")) {
            int idx = line.indexOf('#');
            if (idx >= 0) {
                line = line.substring(0, idx);
            }
            idx = line.indexOf(':');
            if (idx < 0) {
                continue;
            }
            String left = line.substring(0, idx).trim();
            String[] right = line.substring(idx + 1).split(" ");
            List<String> cleanRight = new ArrayList<String>();
            for (int i = 0; i < right.length; i++) {
                right[i] = right[i].trim();
                if (!right[i].isEmpty()) {
                    cleanRight.add(right[i]);
                }
            }
            right = cleanRight.toArray(new String[1]);
//            for (int i = 0; i < right.length; i++) {
//                System.out.println("RIGHT = " + right[i]);
//            }
//            System.out.println("LEFT = " + left);

            if (!dag.jobs.containsKey(left)) {
                DagJob job = new DagJob();
                job.name = left;
                dag.jobs.put(left, job);
            }
            for (String j : right) {
                if (!dag.jobs.containsKey(j)) {
                    DagJob job = new DagJob();
                    job.name = j;
                    dag.jobs.put(j, job);
                }
            }
            for (String j : right) {
                dag.jobs.get(left).depends.add(j);
                dag.jobs.get(j).precedes.add(left);
            }
        }
        for (DagJob j : dag.jobs.values()) {
            j.blocked = j.depends.size();
        }

        return dag;
    }

    public String getNextUnscheduled() {
        String next = null;
        synchronized (this) {
            if (candidate.isEmpty()) {
                for (DagJob job : jobs.values()) {
                    if (job.status == JobStatus.NOT_SCHEDULED) {
                        if (job.blocked == 0) {
                            candidate.add(job.name);
                        }
                    }
                }
            }

            if (!candidate.isEmpty()) {
                next = candidate.iterator().next();
                jobs.get(next).status = JobStatus.RUNNING;
                candidate.remove(next);
            }
        }
        return next;
    }

    public int numImmediatelySchedulableJobs() {
        synchronized (this) {
            if (candidate.isEmpty()) {
                for (DagJob job : jobs.values()) {
                    if (job.status == JobStatus.NOT_SCHEDULED) {
                        if (job.blocked == 0) {
                            candidate.add(job.name);
                        }
                    }
                }
            }
            return candidate.size();
        }
    }

    public void markJobDone(String job) {
        synchronized (this) {
            DagJob dagJob = jobs.get(job);
            dagJob.status = JobStatus.SUCCEEDED;
            for (String j : dagJob.precedes) {
                DagJob anotherJob = jobs.get(j);
                anotherJob.blocked--;
                if (anotherJob.blocked == 0) {
                    candidate.add(anotherJob.name);
                }
            }
        }
    }

    public void markJobFailedAndRetry(String job) {
        synchronized (this) {
            DagJob dagJob = jobs.get(job);
            if (dagJob.status == JobStatus.RUNNING) {
                dagJob.retries++;
                if (dagJob.retries <= maxRetryPerJob) {
                    dagJob.status = JobStatus.NOT_SCHEDULED;
                } else {
                    dagJob.status = JobStatus.FAILED;
                }
            }
        }
    }

    public boolean shouldStop() {
        boolean allJobsSucceeded = true;
        synchronized (this) {
            for (Map.Entry<String, DagJob> e : jobs.entrySet()) {
                if (e.getValue().status == JobStatus.FAILED) {
                    // stop if some job failed
                    return true;
                } else if (e.getValue().status != JobStatus.SUCCEEDED) {
                    allJobsSucceeded = false;
                }
            }
        }
        if (allJobsSucceeded) {
            return true;
        } else {
            return false;
        }
    }

    public boolean succeeded() {
        synchronized (this) {
            for (Map.Entry<String, DagJob> e : jobs.entrySet()) {
                if (e.getValue().status != JobStatus.SUCCEEDED) {
                    return false;
                }
            }
        }
        return true;
    }

    public void schedJobOnContainer(String job, String containerId) {
        synchronized (this) {
            System.out.println("**** PUT job " + job + " on container " + containerId);
            jobs.get(job).containerName = containerId;
        }
    }

    public String getJobNameByContainerId(String containerId) {
        synchronized (this) {
            System.out.println("**** Find job on container " + containerId);
            for (DagJob j : jobs.values()) {
                System.out.println("**** Job " + j.name + " is on container " + j.containerName);
                if (j.containerName.equals(containerId)) {
                    return j.name;
                }
            }
        }
        return null;
    }

    public int totalNodes() {
        synchronized (this) {
            return jobs.size();
        }
    }
}
