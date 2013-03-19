/*
 * tsh - A tiny shell program with job control
 *
 * <Put your name and login ID here>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>

/* Misc manifest constants */
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */
#define LEFT          1
#define RIGHT         2

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

/*
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE) */
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */
sigset_t sig_mask;

struct job_t {              /* The job struct */
  pid_t pid;              /* job PID */
  int jid;                /* job ID [1, 2, ...] */
  int state;              /* UNDEF, BG, FG, or ST */
  char cmdline[MAXLINE];  /* command line */
};
struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */


/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);
void kill_job(char **argv);
int checkForRedirects(int argc, char** argv, int side, int isPiped);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
int parseline(const char *cmdline, char **argv, int* argc);
void sigquit_handler(int sig);

void clearjob(struct job_t *job);
void initjobs(struct job_t *jobs);
int maxjid(struct job_t *jobs);
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);
int deletejob(struct job_t *jobs, pid_t pid);
pid_t fgpid(struct job_t *jobs);
struct job_t *getjobpid(struct job_t *jobs, pid_t pid);
struct job_t *getjobjid(struct job_t *jobs, int jid);
int pid2jid(pid_t pid);
void listjobs(struct job_t *jobs);
char* find_command(char* executable);

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);

/*
 * main - The shell's main routine
 */
int main(int argc, char **argv)
{
  char c;
  char cmdline[MAXLINE];
  int emit_prompt = 1; /* emit prompt (default) */
  //printf("%s", getenv("PATH"));
  /* Redirect stderr to stdout (so that driver will get all output
   * on the pipe connected to stdout) */
  //dup2(1, 2);
  
  /* Parse the command line */
  while ((c = getopt(argc, argv, "hvp")) != EOF) {
    switch (c) {
      case 'h':             /* print help message */
        usage();
        break;
      case 'v':             /* emit additional diagnostic info */
        verbose = 1;
        break;
      case 'p':             /* don't print a prompt */
        emit_prompt = 0;  /* handy for automatic testing */
        break;
      default:
        usage();
    }
  }
  
  /* Install the signal handlers */
  
  /* These are the ones you will need to implement */
  Signal(SIGINT,  sigint_handler);   /* ctrl-c */
  Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
  Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */
  
  /* This one provides a clean way to kill the shell */
  Signal(SIGQUIT, sigquit_handler);
  
  /* Initialize the job list */
  initjobs(jobs);
  
  /* Execute the shell's read/eval loop */
  while (1) {
    
    /* Read command line */
    if (emit_prompt) {
	    printf("%s", prompt);
	    fflush(stdout);
    }
    if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin)) {
      app_error("fgets error");
    }
    if (feof(stdin)) { /* End of file (ctrl-d) */
	    fflush(stdout);
	    exit(0);
    }
    
    /* Evaluate the command line */
    eval(cmdline);
    fflush(stdout);
    fflush(stdout);
  }
  
  exit(0); /* control never reaches here */
}

/*
 * eval - Evaluate the command line that the user has just typed in
 *
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.
 */
void eval(char *cmdline)
{
  if(strncmp(cmdline, "\n", 1) == 0) return;   /* If a blank line is entered, just start over */
  char* argv[MAXARGS], *argv_right[MAXARGS];
  int argc = 0, argc_right = 0, status = 0, isBGtask, redirected = 0, job_state;
  pid_t pid, pid2;
  int fds[2];
  int stdincpy = dup(STDIN_FILENO);
  int stdoutcpy = dup(STDOUT_FILENO);
  char* LHS = strtok(cmdline, "|"), *RHS = strtok(NULL, "|");
  isBGtask = parseline(LHS, argv, &argc);
  sigset_t sig_mask;
	sigemptyset(&sig_mask);
	sigaddset(&sig_mask, SIGINT);
	sigaddset(&sig_mask, SIGCHLD);
	sigaddset(&sig_mask, SIGTSTP);
  if(builtin_cmd(argv)) return;  /* If we're executing a builtin command, do it and leave */
  redirected = checkForRedirects(argc, argv, LEFT, (RHS != NULL));
  if(RHS != NULL) {
    redirected = 1;
    status = pipe(fds);
    if(status < 0) {
      unix_error("pipe error");
      exit(1);
    }
    dup2(fds[1], STDOUT_FILENO);
    close(fds[1]);
  }
  sigprocmask(SIG_BLOCK, &sig_mask, NULL); /* Stop signals while we fork */
  pid = fork();
  if(pid < 0) {
    unix_error("fork error");
    return;
  }
  if(pid == 0) {
    status = sigprocmask(SIG_UNBLOCK, &sig_mask, NULL); /* In the child, so we can start listening for signals */
    status = setpgid(0, 0); /* Attach child process to it's own process group */
    if(status < 0) {
      unix_error("setpgid error");
      exit(0);
    }
    if(execvp(argv[0], argv) < 0) {
      unix_error("execvp error");
      exit(1);
    }
    exit(0);
  } else {
    if(redirected) {
      dup2(stdoutcpy, STDOUT_FILENO);
      dup2(stdincpy, STDIN_FILENO);
    }
    job_state = (isBGtask) ? BG : FG;
    addjob(jobs, pid, job_state, LHS); /* Add job before unmasking signals */
    status = sigprocmask(SIG_UNBLOCK, &sig_mask, NULL);
    if(isBGtask) {
      struct job_t* job = getjobpid(jobs, pid);
      printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline); /* Print jid and pid for backgrounded task */
    } else {
      waitfg(pid); /* Busy wait until process is finished or backgrounded */
    }
    if(RHS != NULL) {
      if(builtin_cmd(argv_right)) return;
      isBGtask = parseline(RHS, argv_right, &argc_right);
      redirected = checkForRedirects(argc_right, argv_right, RIGHT, 1);
      dup2(fds[0], STDIN_FILENO);
      close(fds[0]);
      sigprocmask(SIG_BLOCK, &sig_mask, NULL);
      pid2 = fork();
      if(pid2 < 0) {
        unix_error("fork error");
        exit(1);
      }
      if(pid2 == 0) {
        sigprocmask(SIG_UNBLOCK, &sig_mask, NULL);
        setpgid(0, 0);
        if(execvp(argv_right[0], argv_right) < 0) {
          unix_error("execvp error");
          exit(1);
        }
      } else {
        dup2(stdoutcpy, STDOUT_FILENO);
        dup2(stdincpy, STDIN_FILENO);
        job_state = (isBGtask) ? BG : FG;
        addjob(jobs, pid2, job_state, RHS);
        sigprocmask(SIG_UNBLOCK, &sig_mask, NULL);
        if(isBGtask) {
          struct job_t* job = getjobpid(jobs, pid2);
          printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline); /* Print jid and pid for backgrounded task */
        } else {
          waitfg(pid2);
        }
      }
    }
  }
}

/*
 * parseline - Parse the command line and build the argv array.
 *
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.
 */
int parseline(const char *cmdline, char **argv, int* argc)
{
  static char array[MAXLINE]; /* holds local copy of command line */
  char *buf = array;          /* ptr that traverses command line */
  char *delim;                /* points to first space delimiter */
  //int argc;                   /* number of args */
  int bg;                     /* background job? */
  
  strcpy(buf, cmdline);
  buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */
  while (*buf && (*buf == ' ')) /* ignore leading spaces */
    buf++;
  
  /* Build the argv list */
  *argc = 0;
  if (*buf == '\'') {
    buf++;
    delim = strchr(buf, '\'');
  }
  else {
    delim = strchr(buf, ' ');
  }
  
  while (delim) {
    argv[(*argc)++] = buf;
    *delim = '\0';
    buf = delim + 1;
    while (*buf && (*buf == ' ')) /* ignore spaces */
      buf++;
    
    if (*buf == '\'') {
	    buf++;
	    delim = strchr(buf, '\'');
    }
    else {
	    delim = strchr(buf, ' ');
    }
  }
  argv[*argc] = NULL;
  
  if (argc == 0)  /* ignore blank line */
    return 1;
  
  /* should the job run in the background? */
  if ((bg = (*argv[(*argc) - 1] == '&')) != 0) {
    argv[--(*argc)] = NULL;
  }
  return bg;
}

int checkForRedirects(int argc, char** argv, int side, int isPiped) {
  int i, j, rc1 = 0, rc2 = 0, fd1 = 0, fd2 = 0, redirected = 0;
  for(i = 0; i != argc; ++i) {
    /* Handle input redirection */
    if(strncmp(argv[i], "<", 1) == 0 && argv[i+1] != NULL) {
      if(side == RIGHT && isPiped) continue;
      rc1++;
      if(rc1 > 1) {
        fprintf(stderr, "Ambiguous I/O Redirection error\n");
        return -1;
      }
      /* Open fd for filename provided */
      if((fd1 = open(argv[i+1], O_RDONLY, 0)) < 0) {
        unix_error("Input redirection error");
        exit(1);
      }
      /* Redirect STDIN to opened file */
      dup2(fd1, STDIN_FILENO);
      for(j = i; j != MAXARGS - 2; ++j) {
        argv[j] = argv[j + 2];
      }
      argv[argc - 2] = NULL;
      argc -= 2;
      i -= 2;
      redirected = 1;
    }
    /* Handle output redirection */
    else if (strncmp(argv[i], ">", 1) == 0 && argv[i+1] != NULL) {
      if(side == LEFT && isPiped) continue;
      rc2++;
      if(rc2 > 1) {
        fprintf(stderr, "I/O Redirection is vague.\n");
        return -1;
      }
      /* Open or create new file for writing (files created with 0644 permissions */
      if((fd2 = open(argv[i+1], O_CREAT | O_TRUNC | O_WRONLY, 0644)) < 0) {
        unix_error("fd2 error");
        exit(1);
      }
      /* Replace STDOUT with opened file fd */
      dup2(fd2, STDOUT_FILENO);
      for(j = i; j != MAXARGS - 2; ++j) {
        argv[j] = argv[j + 2];
      }
      argv[argc - 2] = NULL;
      i -= 2;
      argc -= 2;
      redirected = 1;
    }
  }
  return redirected;
}

/*
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.
 */
int builtin_cmd(char **argv)
{
  /** Alright, our builtin commands:
   **   quit or exit
   **   jobs
   **   fg or bg
   **   kill
   **/
  if(strncmp(argv[0], "quit", 4) == 0 || strncmp(argv[0], "exit", 4) == 0) {
    printf("Bye!\n");
    exit(EXIT_SUCCESS);
  }
  else if(strncmp(argv[0], "kill", 4) == 0) {
    kill_job(argv);
    return 1;
  }
  else if(strncmp(argv[0], "fg", 2) == 0 || strncmp(argv[0], "bg", 2) == 0) {
    do_bgfg(argv);
    return 1;
  }
  else if(strncmp(argv[0], "jobs", 4) == 0) {
    listjobs(jobs);
    return 1;
  }
  return 0;     /* not a builtin command */
}

void kill_job(char **argv) {
  int i;
  struct job_t* job;
  
  /* Make sure that we have a pid or jid and that it contains only numbers */
  if(argv[1] == NULL) {
    fprintf(stderr, "%s: Invalid argument count.\n", argv[0]);
    return;
  }
  for (i = 0; i != strlen(argv[1]); ++i) {
    if(i == 0 && argv[1][i] == '%') continue;
    if(!isdigit(argv[1][i])) {
      fprintf(stderr, "Invalid PID or %%JID\n");
      return;
    }
  }
  
  /* Do we have a jid or a pid? */
  if (argv[1][0] == '%') {
    argv[1][0] = '0';
    int jid = atoi(argv[1]);
    job = getjobjid(jobs, jid);
    int pid = job->pid;
    if(jid < 1 || job == NULL) {
      fprintf(stderr, "Job doesn't exist with jid [%d]\n", jid);
      return;
    }
    // send SIGINT to the process to end it.
    kill(-(job->pid), SIGINT);
    deletejob(jobs, pid);
  } else {
    int pid = atoi(argv[1]);
    job = getjobpid(jobs, pid);
    if(pid < 1 || job == NULL) {
      fprintf(stderr, "Process doesn't exist with pid (%d)\n", pid);
      return;
    }
    kill(-(job->pid), SIGINT);
    deletejob(jobs, pid);
  }
  return;
}

/*
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv)
{
  struct job_t* job = NULL;
  int i;
  if(argv[1] == NULL) {
    fprintf(stderr, "%s: Invalid argument count.\n", argv[0]);
    return;
  }
  for (i = 0; i < strlen(argv[1]); i++) {
    if(i == 0 && argv[1][i] == '%') continue;
    if(!isdigit(argv[1][i])) {
      fprintf(stderr, "Invalid PID or %%JID\n");
      return;
    }
  }
  
  if(strncmp(argv[0], "fg", strlen("fg") + 1) == 0) {
    // Handle jid
    if (argv[1][0] == '%') {
      argv[1][0] = '0';
      int jid = atoi(argv[1]);
      job = getjobjid(jobs, jid);
      if(jid < 1 || job == NULL) {
        fprintf(stderr, "No such job with jid: %%%d\n", jid);
        return;
      }
      if(job->state == BG || job->state == ST) {
        kill(-(job->pid), SIGCONT);
        job->state = FG;
        waitfg(job->pid);
      }
      return;
    } else {
      int pid = atoi(argv[1]);
      job = getjobpid(jobs, pid);
      if(pid < 1 || job == NULL) {
        fprintf(stderr, "No such process with pid %d\n", pid);
        return;
      }
      if(job->state == ST || job->state == BG) {
        kill(-(job->pid), SIGCONT);
        job->state = FG;
        waitfg(job->pid);
      }
      return;
    }
  } else {
    if (argv[1][0] == '%') {
      argv[1][0] = '0';
      int jid = atoi(argv[1]);
      job = getjobjid(jobs, jid);
      if(jid < 1 || job == NULL) {
        fprintf(stderr, "No such job with jid: %%%d\n", jid);
        return;
      }
      if(job->state == ST) {
        kill(-(job->pid), SIGCONT);
        job->state = BG;
      }
      return;
    } else {
      int pid = atoi(argv[1]);
      job = getjobpid(jobs, pid);
      if(pid < 1 || job == NULL) {
        fprintf(stderr, "No such process with pid %d\n", pid);
        return;
      }
      if(job->state == ST) {
        kill(-(job->pid), SIGCONT);
        job->state = BG;
      }
      return;
    }
  }
  return;
}

/*
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid)
{
  struct job_t* job = getjobpid(jobs, pid);
  while(job != NULL && job->state == FG) {
    sleep(0);
    job = getjobpid(jobs, pid);
  }
  return;
}

/*****************
 * Signal handlers
 *****************/

/*
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.
 */
void sigchld_handler(int sig)
{
  int status, pid;
  while((pid = waitpid(-1, &status, WNOHANG|WUNTRACED)) > 0) {
    if(pid > 0) {
      if(WIFEXITED(status)) {
        deletejob(jobs, pid);
      }
      else if(WIFSIGNALED(status)) {
        if(WTERMSIG(status) == SIGINT) {
          printf("job[%d] terminated by SIGINT\n", pid2jid(pid));
        }
        deletejob(jobs, pid);
      }
      else if(WIFSTOPPED(status)) {
        getjobpid(jobs, pid)->state = ST;
        printf("job[%d] stopped by signal %d\n", pid2jid(pid), WSTOPSIG(status));
      }
    }
  }
  return;
}

/*
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.
 */
void sigint_handler(int sig)
{
  int killpid = fgpid(jobs);
  
  if(killpid > 0) {
    /* Send signal to -1 * pid to kill whole process group */
    kill(-(killpid), SIGINT);
  }
  return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.
 */
void sigtstp_handler(int sig)
{
  int killpid = fgpid(jobs);
  
  if(killpid > 0) {
    kill(-(killpid), SIGTSTP);
  }
  return;
}


/*********************
 * End signal handlers
 *********************/

/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/

/* clearjob - Clear the entries in a job struct */
void clearjob(struct job_t *job) {
  job->pid = 0;
  job->jid = 0;
  job->state = UNDEF;
  job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void initjobs(struct job_t *jobs) {
  int i;
  
  for (i = 0; i < MAXJOBS; i++)
    clearjob(&jobs[i]);
}

/* maxjid - Returns largest allocated job ID */
int maxjid(struct job_t *jobs)
{
  int i, max=0;
  
  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i].jid > max)
	    max = jobs[i].jid;
  return max;
}

/* addjob - Add a job to the job list */
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline)
{
  int i;
  
  if (pid < 1)
    return 0;
  
  for (i = 0; i < MAXJOBS; i++) {
    if (jobs[i].pid == 0) {
	    jobs[i].pid = pid;
	    jobs[i].state = state;
	    jobs[i].jid = nextjid++;
	    if (nextjid > MAXJOBS)
        nextjid = 1;
	    strcpy(jobs[i].cmdline, cmdline);
      if(verbose){
        printf("Added job [%d] %d %s\n", jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
      }
      return 1;
    }
  }
  printf("Tried to create too many jobs\n");
  return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list */
int deletejob(struct job_t *jobs, pid_t pid)
{
  int i;
  
  if (pid < 1)
    return 0;
  
  for (i = 0; i < MAXJOBS; i++) {
    if (jobs[i].pid == pid) {
	    clearjob(&jobs[i]);
	    nextjid = maxjid(jobs)+1;
	    return 1;
    }
  }
  return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(struct job_t *jobs) {
  int i;
  
  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i].state == FG)
	    return jobs[i].pid;
  return 0;
}

/* getjobpid  - Find a job (by PID) on the job list */
struct job_t *getjobpid(struct job_t *jobs, pid_t pid) {
  int i;
  
  if (pid < 1)
    return NULL;
  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i].pid == pid)
	    return &jobs[i];
  return NULL;
}

/* getjobjid  - Find a job (by JID) on the job list */
struct job_t *getjobjid(struct job_t *jobs, int jid)
{
  int i;
  
  if (jid < 1)
    return NULL;
  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i].jid == jid)
	    return &jobs[i];
  return NULL;
}

/* pid2jid - Map process ID to job ID */
int pid2jid(pid_t pid)
{
  int i;
  
  if (pid < 1)
    return 0;
  for (i = 0; i < MAXJOBS; i++)
    if (jobs[i].pid == pid) {
      return jobs[i].jid;
    }
  return 0;
}

/* listjobs - Print the job list */
void listjobs(struct job_t *jobs)
{
  int i;
  
  for (i = 0; i < MAXJOBS; i++) {
    if (jobs[i].pid != 0) {
	    printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
	    switch (jobs[i].state) {
        case BG:
          printf("Running ");
          break;
        case FG:
          printf("Foreground ");
          break;
        case ST:
          printf("Stopped ");
          break;
        default:
          printf("listjobs: Internal error: job[%d].state=%d ",
                 i, jobs[i].state);
	    }
	    printf("%s", jobs[i].cmdline);
    }
  }
}
/******************************
 * end job list helper routines
 ******************************/


/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void)
{
  printf("Usage: shell [-hvp]\n");
  printf("   -h   print this message\n");
  printf("   -v   print additional diagnostic information\n");
  printf("   -p   do not emit a command prompt\n");
  exit(1);
}

/*
 * unix_error - unix-style error routine
 */
void unix_error(char *msg)
{
  fprintf(stdout, "%s: %s\n", msg, strerror(errno));
  exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(char *msg)
{
  fprintf(stdout, "%s\n", msg);
  exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler)
{
  struct sigaction action, old_action;
  
  action.sa_handler = handler;
  sigemptyset(&action.sa_mask); /* block sigs of type being handled */
  action.sa_flags = SA_RESTART; /* restart syscalls if possible */
  
  if (sigaction(signum, &action, &old_action) < 0)
    unix_error("Signal error");
  return (old_action.sa_handler);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig)
{
  printf("Terminating after receipt of SIGQUIT signal\n");
  exit(1);
}



