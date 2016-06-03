/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * <p>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package yarn_workflow;

import com.google.common.annotations.VisibleForTesting;
import org.apache.commons.cli.*;
import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveInputStream;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.hadoop.classification.InterfaceAudience;
import org.apache.hadoop.classification.InterfaceStability;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.DataOutputBuffer;
import org.apache.hadoop.io.IOUtils;
import org.apache.hadoop.net.NetUtils;
import org.apache.hadoop.security.Credentials;
import org.apache.hadoop.security.UserGroupInformation;
import org.apache.hadoop.security.token.Token;
import org.apache.hadoop.util.ExitUtil;
import org.apache.hadoop.util.Shell;
import org.apache.hadoop.yarn.api.ApplicationConstants;
import org.apache.hadoop.yarn.api.ApplicationConstants.Environment;
import org.apache.hadoop.yarn.api.ApplicationMasterProtocol;
import org.apache.hadoop.yarn.api.ContainerManagementProtocol;
import org.apache.hadoop.yarn.api.protocolrecords.*;
import org.apache.hadoop.yarn.api.records.*;
import org.apache.hadoop.yarn.client.api.AMRMClient.ContainerRequest;
import org.apache.hadoop.yarn.client.api.async.AMRMClientAsync;
import org.apache.hadoop.yarn.client.api.async.NMClientAsync;
import org.apache.hadoop.yarn.client.api.async.impl.NMClientAsyncImpl;
import org.apache.hadoop.yarn.conf.YarnConfiguration;
import org.apache.hadoop.yarn.exceptions.YarnException;
import org.apache.hadoop.yarn.security.AMRMTokenIdentifier;
import org.apache.hadoop.yarn.util.ConverterUtils;
import org.apache.log4j.LogManager;

import java.io.*;
import java.net.URI;
import java.net.URISyntaxException;
import java.nio.ByteBuffer;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ConcurrentMap;
import java.util.zip.GZIPInputStream;

import static yarn_workflow.Client.SELF_EXTRACT_HEADER;
import static yarn_workflow.Client.WORKFLOW;
import static yarn_workflow.Client.WORKFLOW_PKG;

/**
 * An ApplicationMaster for executing shell commands on a set of launched
 * containers using the YARN framework.
 * <p>
 * <p>
 * This class is meant to act as an example on how to write yarn-based
 * application masters.
 * </p>
 * <p>
 * <p>
 * The ApplicationMaster is started on a container by the
 * <code>ResourceManager</code>'s launcher. The first thing that the
 * <code>ApplicationMaster</code> needs to do is to connect and register itself
 * with the <code>ResourceManager</code>. The registration sets up information
 * within the <code>ResourceManager</code> regarding what host:port the
 * ApplicationMaster is listening on to provide any form of functionality to a
 * client as well as a tracking url that a client can use to keep track of
 * status/job history if needed. However, in the distributedshell, trackingurl
 * and appMasterHost:appMasterRpcPort are not supported.
 * </p>
 * <p>
 * <p>
 * The <code>ApplicationMaster</code> needs to send a heartbeat to the
 * <code>ResourceManager</code> at regular intervals to inform the
 * <code>ResourceManager</code> that it is up and alive. The
 * {@link ApplicationMasterProtocol#allocate} to the <code>ResourceManager</code> from the
 * <code>ApplicationMaster</code> acts as a heartbeat.
 * <p>
 * <p>
 * For the actual handling of the job, the <code>ApplicationMaster</code> has to
 * request the <code>ResourceManager</code> via {@link AllocateRequest} for the
 * required no. of containers using {@link ResourceRequest} with the necessary
 * resource specifications such as node location, computational
 * (memory/disk/cpu) resource requirements. The <code>ResourceManager</code>
 * responds with an {@link AllocateResponse} that informs the
 * <code>ApplicationMaster</code> of the set of newly allocated containers,
 * completed containers as well as current state of available resources.
 * </p>
 * <p>
 * <p>
 * For each allocated container, the <code>ApplicationMaster</code> can then set
 * up the necessary launch context via {@link ContainerLaunchContext} to specify
 * the allocated container id, local resources required by the executable, the
 * environment to be setup for the executable, commands to execute, etc. and
 * submit a {@link StartContainerRequest} to the {@link ContainerManagementProtocol} to
 * launch and execute the defined commands on the given allocated container.
 * </p>
 * <p>
 * <p>
 * The <code>ApplicationMaster</code> can monitor the launched container by
 * either querying the <code>ResourceManager</code> using
 * {@link ApplicationMasterProtocol#allocate} to get updates on completed containers or via
 * the {@link ContainerManagementProtocol} by querying for the status of the allocated
 * container's {@link ContainerId}.
 * <p>
 * <p>
 * After the job has been completed, the <code>ApplicationMaster</code> has to
 * send a {@link FinishApplicationMasterRequest} to the
 * <code>ResourceManager</code> to inform it that the
 * <code>ApplicationMaster</code> has been completed.
 */
@InterfaceAudience.Public
@InterfaceStability.Unstable
public class ApplicationMaster {

    private static final Log LOG = LogFactory.getLog(ApplicationMaster.class);

    @VisibleForTesting
    @InterfaceAudience.Private
    public static enum DSEvent {
        DS_APP_ATTEMPT_START, DS_APP_ATTEMPT_END, DS_CONTAINER_START, DS_CONTAINER_END
    }

    @VisibleForTesting
    @InterfaceAudience.Private
    public static enum DSEntity {
        DS_APP_ATTEMPT, DS_CONTAINER
    }

    // Configuration
    private Configuration conf;

    // Handle to communicate with the Resource Manager
    @SuppressWarnings("rawtypes")
    private AMRMClientAsync amRMClient;

    // In both secure and non-secure modes, this points to the job-submitter.
    private UserGroupInformation appSubmitterUgi;

    // Handle to communicate with the Node Manager
    private NMClientAsync nmClientAsync;
    // Listen to process the response from the Node Manager
    private NMCallbackHandler containerListener;

    // Application Attempt Id ( combination of attemptId and fail count )
    @VisibleForTesting
    protected ApplicationAttemptId appAttemptID;

    // TODO
    // For status update for clients - yet to be implemented
    // Hostname of the container
    private String appMasterHostname = "";
    // Port on which the app master listens for status updates from clients
    private int appMasterRpcPort = -1;
    // Tracking url to which app master publishes info for clients to monitor
    private String appMasterTrackingUrl = "";

    // App Master configuration
    // Memory to request for the container on which the shell command will run
    private int containerMemory = 1;
    // VirtualCores to request for the container on which the shell command will run
    private int containerVirtualCores = 1;
    // Priority of the request
    private int requestPriority;

    // Shell command to be executed
    private String shellCommand = "";
    // Args to be passed to the shell command
    private String shellArgs = "";
    // Env variables to be setup for the shell command
    private Map<String, String> shellEnv = new HashMap<String, String>();

    // Location of shell script ( obtained from info set in env )
    // Shell script path in fs
    private String scriptPath = "";
    // Timestamp needed for creating a local resource
    private long shellScriptPathTimestamp = 0;
    // File length needed for local resource
    private long shellScriptPathLen = 0;

    // Hardcoded path to custom log_properties
    private static final String log4jPath = "log4j.properties";

    private volatile boolean done;

    private ByteBuffer allTokens;

    // Launch threads
    private List<Thread> launchThreads = new ArrayList<Thread>();

    private final String linux_bash_command = "bash";
    private final String windows_command = "cmd /c";
    private DagHelper dagHelper = null;
    private ConcurrentLinkedQueue<String> jobsToRun = new ConcurrentLinkedQueue<String>();

    /**
     * @param args Command line args
     */
    public static void main(String[] args) {
        boolean result = false;
        try {
            ApplicationMaster appMaster = new ApplicationMaster();
            LOG.info("Initializing ApplicationMaster");
            boolean doRun = appMaster.init(args);
            if (!doRun) {
                System.exit(0);
            }
            appMaster.run();
            result = appMaster.finish();
        } catch (Throwable t) {
            LOG.fatal("Error running ApplicationMaster", t);
            LogManager.shutdown();
            ExitUtil.terminate(1, t);
        }
        if (result) {
            LOG.info("Application Master completed successfully. exiting");
            System.exit(0);
        } else {
            LOG.info("Application Master failed. exiting");
            System.exit(2);
        }
    }

    /**
     * Dump out contents of $CWD and the environment to stdout for debugging
     */
    private void dumpOutDebugInfo() {

        LOG.info("Dump debug output");
        Map<String, String> envs = System.getenv();
        for (Map.Entry<String, String> env : envs.entrySet()) {
            LOG.info("System env: key=" + env.getKey() + ", val=" + env.getValue());
            System.out.println("System env: key=" + env.getKey() + ", val="
                    + env.getValue());
        }

        BufferedReader buf = null;
        try {
            String lines = Shell.WINDOWS ? Shell.execCommand("cmd", "/c", "dir") :
                    Shell.execCommand("ls", "-al");
            buf = new BufferedReader(new StringReader(lines));
            String line = "";
            while ((line = buf.readLine()) != null) {
                LOG.info("System CWD content: " + line);
                System.out.println("System CWD content: " + line);
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            IOUtils.cleanup(LOG, buf);
        }
    }

    public ApplicationMaster() {
        // Set up the configuration
        conf = new YarnConfiguration();
    }

    /**
     * Parse command line options
     *
     * @param args Command line args
     * @return Whether init successful and run should be invoked
     * @throws ParseException
     * @throws IOException
     */
    public boolean init(String[] args) throws ParseException, IOException {
        Options opts = new Options();
        opts.addOption("app_attempt_id", true,
                "App Attempt ID. Not to be used unless for testing purposes");
        opts.addOption("shell_env", true,
                "Environment for shell script. Specified as env_key=env_val pairs");
        opts.addOption("container_memory", true,
                "Amount of memory in MB to be requested to run the shell command");
        opts.addOption("container_vcores", true,
                "Amount of virtual cores to be requested to run the shell command");
        opts.addOption("num_containers", true,
                "No. of containers on which the shell command needs to be executed");
        opts.addOption("priority", true, "Application Priority. Default 0");
        opts.addOption("debug", false, "Dump out debug information");

        opts.addOption("help", false, "Print usage");
        CommandLine cliParser = new GnuParser().parse(opts, args);

        if (args.length == 0) {
            printUsage(opts);
            throw new IllegalArgumentException(
                    "No args specified for application master to initialize");
        }

        //Check whether customer log4j.properties file exists
        if (fileExist(log4jPath)) {
            try {
                yarn_workflow.Log4jPropertyHelper.updateLog4jConfiguration(ApplicationMaster.class,
                        log4jPath);
            } catch (Exception e) {
                LOG.warn("Can not set up custom log4j properties. " + e);
            }
        }

        if (cliParser.hasOption("help")) {
            printUsage(opts);
            return false;
        }

        if (cliParser.hasOption("debug")) {
            dumpOutDebugInfo();
        }

        Map<String, String> envs = System.getenv();

        if (!envs.containsKey(Environment.CONTAINER_ID.name())) {
            if (cliParser.hasOption("app_attempt_id")) {
                String appIdStr = cliParser.getOptionValue("app_attempt_id", "");
                appAttemptID = ConverterUtils.toApplicationAttemptId(appIdStr);
            } else {
                throw new IllegalArgumentException(
                        "Application Attempt Id not set in the environment");
            }
        } else {
            ContainerId containerId = ConverterUtils.toContainerId(envs
                    .get(Environment.CONTAINER_ID.name()));
            appAttemptID = containerId.getApplicationAttemptId();
        }

        if (!envs.containsKey(ApplicationConstants.APP_SUBMIT_TIME_ENV)) {
            throw new RuntimeException(ApplicationConstants.APP_SUBMIT_TIME_ENV
                    + " not set in the environment");
        }
        if (!envs.containsKey(Environment.NM_HOST.name())) {
            throw new RuntimeException(Environment.NM_HOST.name()
                    + " not set in the environment");
        }
        if (!envs.containsKey(Environment.NM_HTTP_PORT.name())) {
            throw new RuntimeException(Environment.NM_HTTP_PORT
                    + " not set in the environment");
        }
        if (!envs.containsKey(Environment.NM_PORT.name())) {
            throw new RuntimeException(Environment.NM_PORT.name()
                    + " not set in the environment");
        }

        LOG.info("Application master for app" + ", appId="
                + appAttemptID.getApplicationId().getId() + ", clustertimestamp="
                + appAttemptID.getApplicationId().getClusterTimestamp()
                + ", attemptId=" + appAttemptID.getAttemptId());

        if (cliParser.hasOption("shell_env")) {
            String shellEnvs[] = cliParser.getOptionValues("shell_env");
            for (String env : shellEnvs) {
                env = env.trim();
                int index = env.indexOf('=');
                if (index == -1) {
                    shellEnv.put(env, "");
                    continue;
                }
                String key = env.substring(0, index);
                String val = "";
                if (index < (env.length() - 1)) {
                    val = env.substring(index + 1);
                }
                shellEnv.put(key, val);
            }
        }

        if (envs.containsKey(Constants.YARNWORKFLOW_PAYLOADLOCATION)) {
            scriptPath = envs.get(Constants.YARNWORKFLOW_PAYLOADLOCATION);

            if (envs.containsKey(Constants.YARNWORKFLOW_PAYLOADTIMESTAMP)) {
                shellScriptPathTimestamp = Long.valueOf(envs
                        .get(Constants.YARNWORKFLOW_PAYLOADTIMESTAMP));
            }
            if (envs.containsKey(Constants.YARNWORKFLOW_PAYLOADSIZE)) {
                shellScriptPathLen = Long.valueOf(envs
                        .get(Constants.YARNWORKFLOW_PAYLOADSIZE));
            }

            if (!scriptPath.isEmpty()
                    && (shellScriptPathTimestamp <= 0 || shellScriptPathLen <= 0)) {
                LOG.error("Illegal values in env for shell script path" + ", path="
                        + scriptPath + ", len=" + shellScriptPathLen + ", timestamp="
                        + shellScriptPathTimestamp);
                throw new IllegalArgumentException(
                        "Illegal values in env for shell script path");
            }
        }

        // Get WORKFLOW to local
        FileSystem fs = FileSystem.get(conf);

        String workflowPathSuffix = "YarnWorkflow/" + appAttemptID.getApplicationId() + "/" + WORKFLOW_PKG;
        Path shellSrc = new Path(fs.getHomeDirectory(), workflowPathSuffix);
        String workflowTxt = "";
        fs.copyToLocalFile(shellSrc, new Path(WORKFLOW_PKG));
        FileInputStream fi = new FileInputStream(WORKFLOW_PKG);
        fi.skip(SELF_EXTRACT_HEADER.length());
        GZIPInputStream gzIn = new GZIPInputStream(fi);
        TarArchiveInputStream tarIn = new TarArchiveInputStream(gzIn);
        TarArchiveEntry entry = tarIn.getNextTarEntry();
        while (entry != null) {
            if (entry.isDirectory()) {
                entry = tarIn.getNextTarEntry();
                continue;
            }
            if (entry.getName().equals(WORKFLOW)) {
                byte[] buf = new byte[(int) entry.getSize()];
                tarIn.read(buf);
                workflowTxt = new String(buf, "UTF-8");
                break;
            }
            entry = tarIn.getNextTarEntry();
        }
        gzIn.close();
        LOG.info(" *** WORKFLOW: " + workflowTxt);
        dagHelper = DagHelper.createFromString(workflowTxt);

        containerMemory = Integer.parseInt(cliParser.getOptionValue("container_memory", "10"));
        containerVirtualCores = Integer.parseInt(cliParser.getOptionValue("container_vcores", "1"));
        int immediateJobs = dagHelper.numImmediatelySchedulableJobs();
        if (immediateJobs == 0) {
            throw new IllegalArgumentException("No jobs can be run");
        }
        requestPriority = Integer.parseInt(cliParser.getOptionValue("priority", "0"));

        return true;
    }

    /**
     * Helper function to print usage
     *
     * @param opts Parsed command line options
     */
    private void printUsage(Options opts) {
        new HelpFormatter().printHelp("ApplicationMaster", opts);
    }

    /**
     * Main run function for the application master
     *
     * @throws YarnException
     * @throws IOException
     */
    @SuppressWarnings({"unchecked"})
    public void run() throws YarnException, IOException {
        LOG.info("Starting ApplicationMaster");
        // Note: Credentials, Token, UserGroupInformation, DataOutputBuffer class
        // are marked as LimitedPrivate
        Credentials credentials =
                UserGroupInformation.getCurrentUser().getCredentials();
        DataOutputBuffer dob = new DataOutputBuffer();
        credentials.writeTokenStorageToStream(dob);
        // Now remove the AM->RM token so that containers cannot access it.
        Iterator<Token<?>> iter = credentials.getAllTokens().iterator();
        LOG.info("Executing with tokens:");
        while (iter.hasNext()) {
            Token<?> token = iter.next();
            LOG.info(token);
            if (token.getKind().equals(AMRMTokenIdentifier.KIND_NAME)) {
                iter.remove();
            }
        }
        allTokens = ByteBuffer.wrap(dob.getData(), 0, dob.getLength());

        // Create appSubmitterUgi and add original tokens to it
        String appSubmitterUserName =
                System.getenv(ApplicationConstants.Environment.USER.name());
        appSubmitterUgi =
                UserGroupInformation.createRemoteUser(appSubmitterUserName);
        appSubmitterUgi.addCredentials(credentials);

        AMRMClientAsync.CallbackHandler allocListener = new RMCallbackHandler();
        amRMClient = AMRMClientAsync.createAMRMClientAsync(1000, allocListener);
        amRMClient.init(conf);
        amRMClient.start();

        containerListener = createNMCallbackHandler();
        nmClientAsync = new NMClientAsyncImpl(containerListener);
        nmClientAsync.init(conf);
        nmClientAsync.start();

        // Setup local RPC Server to accept status requests directly from clients
        // TODO need to setup a protocol for client to be able to communicate to
        // the RPC server
        // TODO use the rpc port info to register with the RM for the client to
        // send requests to this app master

        // Register self with ResourceManager
        // This will start heartbeating to the RM
        appMasterHostname = NetUtils.getHostname();
        RegisterApplicationMasterResponse response = amRMClient
                .registerApplicationMaster(appMasterHostname, appMasterRpcPort,
                        appMasterTrackingUrl);
        // Dump out information about cluster capability as seen by the
        // resource manager
        int maxMem = response.getMaximumResourceCapability().getMemory();
        LOG.info("Max mem capabililty of resources in this cluster " + maxMem);

        int maxVCores = response.getMaximumResourceCapability().getVirtualCores();
        LOG.info("Max vcores capabililty of resources in this cluster " + maxVCores);

        // A resource ask cannot exceed the max.
        if (containerMemory > maxMem) {
            LOG.info("Container memory specified above max threshold of cluster."
                    + " Using max value." + ", specified=" + containerMemory + ", max="
                    + maxMem);
            containerMemory = maxMem;
        }

        if (containerVirtualCores > maxVCores) {
            LOG.info("Container virtual cores specified above max threshold of cluster."
                    + " Using max value." + ", specified=" + containerVirtualCores + ", max="
                    + maxVCores);
            containerVirtualCores = maxVCores;
        }

        for (; ; ) {
            String nextJob = dagHelper.getNextUnscheduled();
            if (nextJob == null) {
                break;
            }
            jobsToRun.add(nextJob);
            LOG.info(" *** WILL RUN " + nextJob);
            ContainerRequest containerAsk = setupContainerAskForRM();
            amRMClient.addContainerRequest(containerAsk);
        }
    }

    @VisibleForTesting
    NMCallbackHandler createNMCallbackHandler() {
        return new NMCallbackHandler(this);
    }

    @VisibleForTesting
    protected boolean finish() {
        // wait for completion.
        while (!done && !dagHelper.shouldStop()) {
            try {
                Thread.sleep(200);
            } catch (InterruptedException ex) {
            }
        }

        // Join all launched threads
        // needed for when we time out
        // and we need to release containers
        for (Thread launchThread : launchThreads) {
            try {
                launchThread.join(10000);
            } catch (InterruptedException e) {
                LOG.info("Exception thrown in thread join: " + e.getMessage());
                e.printStackTrace();
            }
        }

        // When the application completes, it should stop all running containers
        LOG.info("Application completed. Stopping running containers");
        nmClientAsync.stop();

        // When the application completes, it should send a finish application
        // signal to the RM
        LOG.info("Application completed. Signalling finish to RM");

        FinalApplicationStatus appStatus;
        String appMessage = null;
        boolean success = true;
        if (dagHelper.succeeded()) {
            appStatus = FinalApplicationStatus.SUCCEEDED;
        } else {
            appStatus = FinalApplicationStatus.FAILED;
            success = false;
        }
        try {
            amRMClient.unregisterApplicationMaster(appStatus, appMessage, null);
        } catch (YarnException ex) {
            LOG.error("Failed to unregister application", ex);
        } catch (IOException e) {
            LOG.error("Failed to unregister application", e);
        }

        amRMClient.stop();

        return success;
    }

    private class RMCallbackHandler implements AMRMClientAsync.CallbackHandler {
        @SuppressWarnings("unchecked")
        @Override
        public void onContainersCompleted(List<ContainerStatus> completedContainers) {
            LOG.info("Got response from RM for container ask, completedCnt="
                    + completedContainers.size());
            for (ContainerStatus containerStatus : completedContainers) {
                LOG.info(appAttemptID + " got container status for containerID="
                        + containerStatus.getContainerId() + ", state="
                        + containerStatus.getState() + ", exitStatus="
                        + containerStatus.getExitStatus() + ", diagnostics="
                        + containerStatus.getDiagnostics());

                // non complete containers should not be here
                assert (containerStatus.getState() == ContainerState.COMPLETE);

                // increment counters for completed/failed containers
                int exitStatus = containerStatus.getExitStatus();
                if (0 != exitStatus) {
                    // container failed
                    if (ContainerExitStatus.ABORTED != exitStatus) {
                        // shell script failed, retry
                        String dagJobName = dagHelper.getJobNameByContainerId(containerStatus.getContainerId().toString());
                        dagHelper.markJobFailedAndRetry(dagJobName);
                    } else {
                        // container was killed by framework, possibly preempted
                        // we should re-try as the container was lost for some reason
//                        numAllocatedContainers.decrementAndGet();
//                        numRequestedContainers.decrementAndGet();
                        // we do not need to release the container as it would be done
                        // by the RM
                        String dagJobName = dagHelper.getJobNameByContainerId(containerStatus.getContainerId().toString());
                        if (dagJobName != null) {
                            dagHelper.markJobFailedAndRetry(dagJobName);
                        }
                    }
                } else {
                    // nothing to do
                    // container completed successfully
//                    numCompletedContainers.incrementAndGet();
                    String dagJobName = dagHelper.getJobNameByContainerId(containerStatus.getContainerId().toString());
                    dagHelper.markJobDone(dagJobName);
                    LOG.info("Container completed successfully." + ", containerId="
                            + containerStatus.getContainerId() + ", jobScript=" + dagJobName);
                }
            }

            for (; ; ) {
                String nextJob = dagHelper.getNextUnscheduled();
                if (nextJob == null) {
                    break;
                }
                jobsToRun.add(nextJob);
                LOG.info(" *** WILL RUN " + nextJob);
                ContainerRequest containerAsk = setupContainerAskForRM();
                amRMClient.addContainerRequest(containerAsk);
            }
        }

        @Override
        public void onContainersAllocated(List<Container> allocatedContainers) {
            LOG.info("Got response from RM for container ask, allocatedCnt="
                    + allocatedContainers.size());
            for (Container allocatedContainer : allocatedContainers) {
                LOG.info("Newly allocated container: " + allocatedContainer.getId());
            }
            for (Container allocatedContainer : allocatedContainers) {
                String jobScript = jobsToRun.poll();
                if (jobScript == null) {
                    amRMClient.releaseAssignedContainer(allocatedContainer.getId());
                    continue;
                }

                LOG.info("Launching shell command on a new container."
                        + ", containerId=" + allocatedContainer.getId()
                        + ", containerNode=" + allocatedContainer.getNodeId().getHost()
                        + ":" + allocatedContainer.getNodeId().getPort()
                        + ", containerNodeURI=" + allocatedContainer.getNodeHttpAddress()
                        + ", containerResourceMemory="
                        + allocatedContainer.getResource().getMemory()
                        + ", containerResourceVirtualCores="
                        + allocatedContainer.getResource().getVirtualCores()
                        + ",jobScript=" + jobScript);

                dagHelper.schedJobOnContainer(jobScript, allocatedContainer.getId().toString());
                LaunchContainerRunnable runnableLaunchContainer =
                        new LaunchContainerRunnable(jobScript, allocatedContainer, containerListener);
                Thread launchThread = new Thread(runnableLaunchContainer);

                // launch and start the container on a separate thread to keep
                // the main thread unblocked
                // as all containers may not be allocated at one go.
                launchThreads.add(launchThread);
                launchThread.start();
            }
        }

        @Override
        public void onShutdownRequest() {
            done = true;
        }

        @Override
        public void onNodesUpdated(List<NodeReport> updatedNodes) {
        }

        @Override
        public float getProgress() {
            // set progress to deliver to RM on next heartbeat
//            float progress = (float) numCompletedContainers.get() / dagHelper.totalNodes();
            float progress = (float) dagHelper.completedNodes() / dagHelper.totalNodes();
            return progress;
        }

        @Override
        public void onError(Throwable e) {
            done = true;
            amRMClient.stop();
        }
    }

    @VisibleForTesting
    static class NMCallbackHandler
            implements NMClientAsync.CallbackHandler {

        private ConcurrentMap<ContainerId, Container> containers =
                new ConcurrentHashMap<ContainerId, Container>();
        private final ApplicationMaster applicationMaster;

        public NMCallbackHandler(ApplicationMaster applicationMaster) {
            this.applicationMaster = applicationMaster;
        }

        public void addContainer(ContainerId containerId, Container container) {
            containers.putIfAbsent(containerId, container);
        }

        @Override
        public void onContainerStopped(ContainerId containerId) {
            if (LOG.isDebugEnabled()) {
                LOG.debug("Succeeded to stop Container " + containerId);
            }
            containers.remove(containerId);
        }

        @Override
        public void onContainerStatusReceived(ContainerId containerId,
                                              ContainerStatus containerStatus) {
            if (LOG.isDebugEnabled()) {
                LOG.debug("Container Status: id=" + containerId + ", status=" +
                        containerStatus);
            }
        }

        @Override
        public void onContainerStarted(ContainerId containerId,
                                       Map<String, ByteBuffer> allServiceResponse) {
            if (LOG.isDebugEnabled()) {
                LOG.debug("Succeeded to start Container " + containerId);
            }
            Container container = containers.get(containerId);
            if (container != null) {
                applicationMaster.nmClientAsync.getContainerStatusAsync(containerId, container.getNodeId());
            }
        }

        @Override
        public void onStartContainerError(ContainerId containerId, Throwable t) {
            LOG.error("Failed to start Container " + containerId);
            containers.remove(containerId);
//            applicationMaster.numCompletedContainers.incrementAndGet();
//            applicationMaster.numFailedContainers.incrementAndGet();
        }

        @Override
        public void onGetContainerStatusError(
                ContainerId containerId, Throwable t) {
            LOG.error("Failed to query the status of Container " + containerId);
        }

        @Override
        public void onStopContainerError(ContainerId containerId, Throwable t) {
            LOG.error("Failed to stop Container " + containerId);
            containers.remove(containerId);
        }
    }

    /**
     * Thread to connect to the {@link ContainerManagementProtocol} and launch the container
     * that will execute the shell command.
     */
    private class LaunchContainerRunnable implements Runnable {

        String jobScript;
        // Allocated container
        Container container;

        NMCallbackHandler containerListener;

        /**
         * @param lcontainer        Allocated container
         * @param containerListener Callback handler of the container
         */
        public LaunchContainerRunnable(
                String jobScript, Container lcontainer, NMCallbackHandler containerListener) {
            this.jobScript = jobScript;
            this.container = lcontainer;
            this.containerListener = containerListener;
        }

        @Override
        /**
         * Connects to CM, sets up container launch context
         * for shell command and eventually dispatches the container
         * start request to the CM.
         */
        public void run() {
            LOG.info("Setting up container launch container for containerid="
                    + container.getId());

            // Set the local resources
            Map<String, LocalResource> localResources = new HashMap<String, LocalResource>();

            // The container for the eventual shell commands needs its own local
            // resources too.
            // In this scenario, if a shell script is specified, we need to have it
            // copied and made available to the container.
            if (!scriptPath.isEmpty()) {
                Path path = new Path(scriptPath);
                URL yarnUrl = null;
                try {
                    yarnUrl = ConverterUtils.getYarnUrlFromURI(
                            new URI(path.toString()));
                } catch (URISyntaxException e) {
                    LOG.error("Error when trying to use shell script path specified"
                            + " in env, path=" + path, e);
                    // A failure scenario on bad input such as invalid shell script path
                    // We know we cannot continue launching the container
                    // so we should release it.
                    // TODO
//                    numCompletedContainers.incrementAndGet();
//                    numFailedContainers.incrementAndGet();
                    return;
                }
                LocalResource shellRsrc = LocalResource.newInstance(yarnUrl,
                        LocalResourceType.FILE, LocalResourceVisibility.APPLICATION,
                        shellScriptPathLen, shellScriptPathTimestamp);
                localResources.put(WORKFLOW_PKG, shellRsrc);
                shellCommand = Shell.WINDOWS ? windows_command : linux_bash_command;
            }

            // Set the necessary command to execute on the allocated container
            Vector<CharSequence> vargs = new Vector<CharSequence>(5);

            // Set executable command
            vargs.add(shellCommand);
            // Set shell script path
            if (!scriptPath.isEmpty()) {
                vargs.add(WORKFLOW_PKG + " " + jobScript);
            }

            // Set args for the shell command if any
            vargs.add(shellArgs);
            // Add log redirect params
            vargs.add("1>" + ApplicationConstants.LOG_DIR_EXPANSION_VAR + "/stdout");
            vargs.add("2>" + ApplicationConstants.LOG_DIR_EXPANSION_VAR + "/stderr");

            // Get final commmand
            StringBuilder command = new StringBuilder();
            for (CharSequence str : vargs) {
                command.append(str).append(" ");
            }

            List<String> commands = new ArrayList<String>();
            commands.add(command.toString());

            // Set up ContainerLaunchContext, setting local resource, environment,
            // command and token for constructor.

            // Note for tokens: Set up tokens for the container too. Today, for normal
            // shell commands, the container in distribute-shell doesn't need any
            // tokens. We are populating them mainly for NodeManagers to be able to
            // download anyfiles in the distributed file-system. The tokens are
            // otherwise also useful in cases, for e.g., when one is running a
            // "hadoop dfs" command inside the distributed shell.
            ContainerLaunchContext ctx = ContainerLaunchContext.newInstance(
                    localResources, shellEnv, commands, null, allTokens.duplicate(), null);
            containerListener.addContainer(container.getId(), container);
            nmClientAsync.startContainerAsync(container, ctx);
        }
    }

    /**
     * Setup the request that will be sent to the RM for the container ask.
     *
     * @return the setup ResourceRequest to be sent to RM
     */
    private ContainerRequest setupContainerAskForRM() {
        // setup requirements for hosts
        // using * as any host will do for the distributed shell app
        // set the priority for the request
        // TODO - what is the range for priority? how to decide?
        Priority pri = Priority.newInstance(requestPriority);

        // Set up resource type requirements
        // For now, memory and CPU are supported so we set memory and cpu requirements
        Resource capability = Resource.newInstance(containerMemory,
                containerVirtualCores);

        ContainerRequest request = new ContainerRequest(capability, null, null,
                pri);
        LOG.info("Requested container ask: " + request.toString());
        return request;
    }

    private boolean fileExist(String filePath) {
        return new File(filePath).exists();
    }

    private String readContent(String filePath) throws IOException {
        DataInputStream ds = null;
        try {
            ds = new DataInputStream(new FileInputStream(filePath));
            return ds.readUTF();
        } finally {
            org.apache.commons.io.IOUtils.closeQuietly(ds);
        }
    }
}