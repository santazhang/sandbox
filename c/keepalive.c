#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <memory.h>

#define YES 1
#define NO 0

#ifdef WIN32

// Windows code
#pragma warning(disable: 4996)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <TlHelp32.h>
#include <process.h>

int my_pid;
int sibling_pid = -1;
int target_pid = -1;

char mem_mapping_name[100];
HANDLE hMapFile = 0;

void* p_share_mem;

int shared_locked = 1;
int shared_unlocked = 0;
int shared_lock;

char** sibling_argv;
char** target_argv;

void acquire_shared_lock() {
  for (;;) {
    memcpy(&shared_lock, p_share_mem, sizeof(int));
    if (shared_lock != shared_locked)
      break;
  }
  memcpy(p_share_mem, &shared_locked, sizeof(int));
}


void release_shared_lock() {
  memcpy(p_share_mem, &shared_unlocked, sizeof(int));
}


int my_exec_new(char* arglist[]) {

  intptr_t process_ptr = _spawnv(
    _P_NOWAIT,
    arglist[0],
    arglist
  );

  return GetProcessId((HANDLE) process_ptr);
}


int my_exists_pid(int pid) {
  HANDLE hProcessSnap;
  PROCESSENTRY32 pe32;
  hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  pe32.dwSize = sizeof(PROCESSENTRY32);
  Process32First(hProcessSnap, &pe32);
  for (;;) {
    if (pe32.th32ProcessID == pid)
      return YES;
    if (!Process32Next(hProcessSnap, &pe32)) {
      break;
    }
  }
  return NO;
}

// entrance for windows code
int main(int argc, char* argv[]) {
  
  int i, j;
  int real_arg_start = 1;
  char my_pid_str[20];

  my_pid = GetProcessId(GetCurrentProcess());
  srand((int) time(NULL) + my_pid);

  sibling_argv = (char **) malloc(sizeof(char *) * (argc + 5));
  target_argv = (char **) malloc(sizeof(char *) * (argc + 1));

  if (argc == 1) {
    fprintf(stderr, "Usage: KeepAlive Command\n");
    exit(1);
  }

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-s") == 0) {
      i++;
      sibling_pid = atoi(argv[i]);
      real_arg_start += 2;
    } else if (strcmp(argv[i], "-m") == 0) {
      i++;
      strcpy(mem_mapping_name, argv[i]);
      real_arg_start += 2;
    }
  }

  // create share file
  if (sibling_pid == -1) {

    // not passive, should create the memory mapping
    strcpy(mem_mapping_name, "KEEP_ALIVE_");
    itoa((int) time(NULL), mem_mapping_name + strlen(mem_mapping_name), 10);
    strcat(mem_mapping_name, "_");
    itoa(my_pid, mem_mapping_name + strlen(mem_mapping_name), 10);
    strcat(mem_mapping_name, "_");
    itoa(rand(), mem_mapping_name + strlen(mem_mapping_name), 10);

    hMapFile = CreateFileMapping(
      INVALID_HANDLE_VALUE,    // use paging file
      NULL,                    // default security 
      PAGE_READWRITE,          // read/write access
      0,                       // max. object size 
      1024,                // buffer size  
      (LPCWSTR) mem_mapping_name
    );

    p_share_mem = MapViewOfFile(
      hMapFile,   // handle to map object
      FILE_MAP_ALL_ACCESS, // read/write permission
      0,
      0,
      1024
    );

    // not locked
    memcpy(p_share_mem, &shared_unlocked, sizeof(int));
    memcpy(((int *) p_share_mem) + 1, &target_pid, sizeof(int));

  } else {
    // passive, get memory mapping

    hMapFile = OpenFileMapping(
      FILE_MAP_ALL_ACCESS,   // read/write access
      FALSE,                 // do not inherit the name
      (LPCWSTR) mem_mapping_name
    );

    p_share_mem = MapViewOfFile(
      hMapFile,   // handle to map object
      FILE_MAP_ALL_ACCESS, // read/write permission
      0,
      0,
      1024
    );
    memcpy(&shared_lock, p_share_mem, sizeof(int));
    memcpy(&target_pid, ((int *) p_share_mem) + 1, sizeof(int));
  }

  itoa(my_pid, my_pid_str, 10);

  // set sibling argv
  sibling_argv[0] = argv[0];
  sibling_argv[1] = "-s";
  sibling_argv[2] = my_pid_str;
  sibling_argv[3] = "-m";
  sibling_argv[4] = mem_mapping_name;
  for (i = 5, j = real_arg_start; j < argc; i++, j++) {
    sibling_argv[i] = argv[j];
  }
  sibling_argv[i] = NULL;

  // set target argv
  for (i = 0, j = real_arg_start; j < argc; i++, j++) {
    target_argv[i] = argv[j];
  }
  target_argv[i] = NULL;

  for (;;) {
    
    if (sibling_pid == -1 || my_exists_pid(sibling_pid) == NO) {
      sibling_pid = my_exec_new(sibling_argv);
    }

    // test target running
    {
      int shared_target;

      acquire_shared_lock();

      // check shared target id
      memcpy(&shared_target, ((int *) p_share_mem) + 1, sizeof(int));

      if (target_pid != shared_target) {
        target_pid = shared_target;
      }

      if (target_pid == -1 || my_exists_pid(target_pid) == NO) {
        target_pid = my_exec_new(target_argv);
        memcpy(((int *) p_share_mem) + 1, &target_pid, sizeof(int));
      }

      release_shared_lock();
    }
    Sleep(100);
  }
  free(target_argv);
  free(sibling_argv);
  
  return 0;
}

#else

// linux code

#include <sys/mman.h>
#include <signal.h>
#include <unistd.h>

int test_pid_exist(int pid) {
  char cmd_buffer[128];
  FILE *fp;
  int result;

  sprintf(cmd_buffer, "ps -eo pid | grep -c %d", pid);
  fp = popen(cmd_buffer, "r");
  fscanf(fp, "%d", &result);
  pclose(fp);
  
  if (result == 1) {
    return YES;
  } else {
    return NO;
  }
}

int* p_share;

int shared_locked = 1;
int shared_unlocked = 0;
int shared_lock;

void acquire_shared_lock() {
  for (;;) {
    memcpy(&shared_lock, p_share, sizeof(int));
    if (shared_lock != shared_locked)
      break;
  }
  memcpy(p_share, &shared_locked, sizeof(int));
}

void release_shared_lock() {
  memcpy(p_share, &shared_unlocked, sizeof(int));
}


// entrance for linux code
int main(int argc, char* argv[]) {

  int i;
  int sibling_pid = -1;
  int my_pid = getpid();
  int target_pid = -1;
  srand((int) time(NULL));
  
  if (argc == 1) {
    fprintf(stderr, "Usage: KeepAlive Command\n");
    exit(1);
  }
  
  for (;;) {
  
    // test sibling exist
    if (sibling_pid == -1 || test_pid_exist(sibling_pid) == 0) {
    
      // prevents zombies
      signal(SIGCHLD,SIG_IGN);
      
      // share memory
      p_share = (int* ) mmap(NULL, sizeof(int) * 2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
      memcpy(p_share, &shared_unlocked, sizeof(int));
      memcpy(p_share + 1, &target_pid, sizeof(int));
      
      sibling_pid = fork();
      
      if (sibling_pid == 0) {
        // child process
        
        // get parent pid
        sibling_pid = getppid();
        my_pid = getpid();
        
        srand((int) time(NULL) + my_pid);
      } else {
        // parent
        // do nothing, already got the correct value
      }
    }
    
    // testing target existance
    {
      int shared_pid;
      acquire_shared_lock();
      
      // test shared_pid
      memcpy(&shared_pid, p_share + 1, sizeof(int));
      
      if (shared_pid != target_pid) {
        target_pid = shared_pid;
      }
      
      if (target_pid == -1 || test_pid_exist(target_pid) == 0) {
        
        // prevent zombie
        signal(SIGCHLD,SIG_IGN);
        target_pid = fork();
        if (target_pid == 0) {
          // child_process
          int i;
          char** arg_list = (char **) malloc(sizeof(char *) * argc);
          for (i = 1; i < argc; i++) {
            arg_list[i - 1] = argv[i];
          }
          arg_list[argc] = NULL;
          execv(arg_list[0], arg_list);
        } else {
          // update shared_info
          memcpy(p_share + 1, &target_pid, sizeof(int));
        }
      }
      release_shared_lock();
    }
    sleep(1);
  }
  return 0;
}

#endif  // WIN32
