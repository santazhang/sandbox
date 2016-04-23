package yarn_workflow.test;

import org.junit.Test;
import yarn_workflow.DagHelper;

public class TestDagHelper {

    @Test
    public void test1() {
        DagHelper dag = DagHelper.createFromString("# THIS IS COMMENT\na:b");
        System.out.println(dag.numImmediatelySchedulableJobs());
        System.out.println(dag.getNextUnscheduled());
        System.out.println(dag.numImmediatelySchedulableJobs());
        System.out.println(dag.getNextUnscheduled());
        System.out.println(dag.numImmediatelySchedulableJobs());
        dag.markJobDone("a");
        System.out.println(dag.numImmediatelySchedulableJobs());
        System.out.println(dag.getNextUnscheduled());
        System.out.println(dag.numImmediatelySchedulableJobs());
        dag.markJobDone("b");
        System.out.println(dag.getNextUnscheduled());

        System.out.println(dag.numImmediatelySchedulableJobs());
        System.out.println(dag.shouldStop());
        System.out.println(dag.succeeded());
    }

    @Test
    public void test2() {
        DagHelper dag = DagHelper.createFromString("# Makefile-ish dependency\n" +
                "\n" +
                "step-3.sh: step-2.sh\n" +
                "\n" +
                "step-2.sh: step-1.sh\n");
    }
}
