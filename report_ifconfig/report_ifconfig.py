#!/usr/bin/python

# This script, when executed in /etc/rc.local, could report current machine's
# ifconfig info to a remote server.
# It is used to identify worker machine's IP address.
#
# Author:: Santa Zhang

import os
import time
import sys

def daemonize (stdin='/dev/null', stdout='/dev/null', stderr='/dev/null'):

    '''This forks the current process into a daemon. The stdin, stdout, and
    stderr arguments are file names that will be opened and be used to replace
    the standard file descriptors in sys.stdin, sys.stdout, and sys.stderr.
    These arguments are optional and default to /dev/null. Note that stderr is
    opened unbuffered, so if it shares a file with stdout then interleaved
    output may not appear in the order that you expect. '''

    # Do first fork.
    try: 
        pid = os.fork() 
        if pid > 0:
            sys.exit(0)   # Exit first parent.
    except OSError, e: 
        sys.stderr.write ("fork #1 failed: (%d) %s\n" % (e.errno, e.strerror) )
        sys.exit(1)

    # Decouple from parent environment.
    os.chdir("/") 
    os.umask(0) 
    os.setsid() 

    # Do second fork.
    try: 
        pid = os.fork() 
        if pid > 0:
            sys.exit(0)   # Exit second parent.
    except OSError, e: 
        sys.stderr.write ("fork #2 failed: (%d) %s\n" % (e.errno, e.strerror) )
        sys.exit(1)

    # Now I am a daemon!
    
    # Redirect standard file descriptors.
    si = open(stdin, 'r')
    so = open(stdout, 'a+')
    se = open(stderr, 'a+', 0)
    os.dup2(si.fileno(), sys.stdin.fileno())
    os.dup2(so.fileno(), sys.stdout.fileno())
    os.dup2(se.fileno(), sys.stderr.fileno())


def exec_and_get_output(cmd):
  p = os.popen(cmd)
  output = p.read()
  p.close()
  return output

def my_exec(cmd):
  print "[exec] %s" % cmd
  p = os.popen(cmd)
  try:
    p.read()
  finally:
    p.close()

def report_main():
  f = open(os.path.splitext(__file__)[0] + ".conf", "r")
  conf = f.read()
  exec conf
  f.close()
  if "-d" in sys.argv:
    daemonize()

  while True:
    ifconfig = exec_and_get_output("ifconfig")
    hostname = exec_and_get_output("hostname")
    hostname = hostname.strip()
    report_f = open("/tmp/report_ifconfig.txt", "w")
    report_f.write(time.strftime("%Y%m%d-%H%M%S\n", time.localtime()))
    report_f.write("hostname: %s\n" % hostname)
    report_f.write(ifconfig)
    report_f.close()
    for rs in report_servers:
      my_exec('ssh -o stricthostkeychecking=no %s "mkdir -p %s"' % (rs, report_folder))
      my_exec('scp -o stricthostkeychecking=no /tmp/report_ifconfig.txt %s:%s/%s.txt' % (rs, report_folder, hostname))
    os.remove("/tmp/report_ifconfig.txt")
    print "Sleeping %d seconds" % interval
    time.sleep(interval)

if __name__ == "__main__":
  report_main()

