#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/reg.h>

int main()
{
	pid_t childPid;
	int status;
	
	childPid = fork();
	

	if(childPid == 0)
	{
		printf("hello from child\n");
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execl("/bin/ls", "ls", NULL);
	}
	else if(childPid > 0) //родительский процесс
	{
		printf("hello from parent\n");

		wait(&status); //ожидание завершения дочернего процесса
		while(WIFSTOPPED(status)) //проверка остановлен ли процесс
		{
			struct user_regs_struct regs;
			ptrace(PTRACE_GETREGS, childPid, NULL, &regs);//получение значений регистров процесса
	
			printf("System call: %lld\n", regs.orig_rax);

			ptrace(PTRACE_SYSCALL, childPid, NULL, NULL);
			//возобновление выполнения процесса после остановки из-за системного ввызова
			
			wait(&status);
			
		}	
		
	}
	return 0;
}
