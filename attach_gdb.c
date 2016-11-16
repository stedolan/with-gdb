#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

volatile int has_gdb_attached = 0;

static char command[1024];
static pthread_once_t once = PTHREAD_ONCE_INIT;

#define SAY(msg) \
  write(STDERR_FILENO, "with-gdb: " msg "\n", sizeof("with-gdb: " msg "\n"))

static void attach_gdb(void) {
  pid_t child = fork();
  if (child < 0) {
    SAY("fork() failed when trying to connect gdb");
    _exit(128);
  } else if (child == 0) {
    dup2(STDERR_FILENO, STDOUT_FILENO); // redirect all gdb output to stderr
    execl("/bin/sh", "sh", "-c", command, (char*)NULL);
    _exit(128); // we only get here if execl fails
  }
  while (!has_gdb_attached) {
    int wstatus;
    if (waitpid(child, &wstatus, WNOHANG) != 0) {
      SAY("unable to attach gdb");
      has_gdb_attached = 1;
    }
  }
}

static void handle_signal(int sig) {
  pthread_once(&once, &attach_gdb);
  while (!has_gdb_attached) {}
  raise(sig);
}

static char altstackbuf[SIGSTKSZ];

static void init_sigstack() {
  stack_t altstack;
  sigaltstack(0, &altstack);
  if (altstack.ss_size < sizeof(altstackbuf)) {
    altstack.ss_sp = altstackbuf;
    altstack.ss_size = sizeof(altstackbuf);
    altstack.ss_flags = 0;
    sigaltstack(&altstack, 0);
  }
}

static void add_handler(int signum, void (*act)(int)) {
  struct sigaction s;
  s.sa_handler = act;
  s.sa_flags = SA_ONSTACK | SA_RESETHAND;
  sigemptyset(&s.sa_mask);
  if (sigaction(signum, &s, 0) < 0) {
    perror("setting up signal handler");
  }
}


__attribute__((constructor)) static void setup()  {
  if (getenv("GDB_ATTACH_NORECURSE")) return;
  const char* extra_cmds =
    (isatty(STDIN_FILENO) && isatty(STDERR_FILENO)) ? "" : "-ex quit";
  snprintf(command, sizeof(command),
   "exec env GDB_ATTACH_NORECURSE=1 "
     "gdb -quiet "
     "-ex 'set confirm off' "
     "-ex 'set variable has_gdb_attached = 1' "
     "-ex continue "
     "-ex bt %s -p %d", extra_cmds, getpid());
  init_sigstack();
  add_handler(SIGABRT, &handle_signal);
  add_handler(SIGSEGV, &handle_signal);
  add_handler(SIGBUS, &handle_signal);
}
