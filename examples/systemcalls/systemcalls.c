#include "systemcalls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>


#include <fcntl.h>
#include <syslog.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/

   int ret = system(cmd);
   
    return ret;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

int global;
bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    int ret=true;
    pid_t child_pid;
    int child_status;
    /*pid_t tpid;*/
    int local=0;
  
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
        printf("command %s\n",command[i]);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/

     
  child_pid = fork();


        if (child_pid == 0) /* fork() returns 0 for the child process */
        {
            printf("child process!\n");

            // Increment the local and global variables
            local++;
            global++;

            printf("child PID =  %d, parent pid = %d\n", getpid(), getppid());
            printf("\n child's local = %d, child's global = %d\n",local,global);

            execv(command[0], command); // call whoami command
            return false;
            /*exit(0);*/

         }
         else /* parent process */
         {
             printf("parent process!\n");
             printf("parent PID =  %d, child pid = %d\n", getpid(), child_pid);
             wait(&child_status); /* wait for child to exit, and store child's exit status */
             printf("Child exit code: %d\n", WEXITSTATUS(child_status));

             //The change in local and global variable in child process should not reflect here in parent process.
             printf("\n Parent'z local = %d, parent's  global = %d\n",local,global);

             printf("Parent says bye!\n");
             exit(0);  /* parent exits */
         }
         
          
    va_end(args);

printf("exiting with Child exit code: %d\n", WEXITSTATUS(child_status));
if( WEXITSTATUS(child_status) == 0){
ret=false;
}
else
{
ret=true;
}
printf("do_exec %d\n", ret);
   return ret;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    int ret=true;
    
    pid_t child_pid;
    int child_status;
    pid_t tpid;
    int fd;
    
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
        printf("command %s\n",command[i]);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/

   

fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
if (fd < 0) { perror("open"); abort(); }
  child_pid = fork();

switch (child_pid) {
  case -1: perror("fork"); abort();
  case 0:
    if (dup2(fd, 1) < 0) { perror("dup2"); abort(); }
    close(fd);
    execvp(command[0], command); perror("execvp"); abort();
    ret = false;
  default:
    close(fd);
    /* do whatever the parent wants to do. */
    do {
       tpid = wait(&child_status);
       if(tpid != child_pid) kill(tpid, SIGSEGV);
     } while(tpid != child_pid);
}


    va_end(args);
printf("do_exec_redirect %d\n", ret);
    return ret;
}

