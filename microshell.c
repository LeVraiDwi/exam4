#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define STDIN_FILNO 0
#define STDOUT_FILNO 1

void	ft_putstr_f2(char *str)
{
	int	l;

	l = 0;
	while (str[l])
		l++;
	write(2, str, l);
}

int	ft_execute(char **argv, int i, char **env)
{
	argv[i] = NULL;
	execve(argv[0], argv, env);
	ft_putstr_f2("error: cannot execute ");
	ft_putstr_f2(argv[0]);
	write(2, "\n", 1);
	return (1);
}

int	main(int argc, char **argv, char **env)
{
	int	i;
	int	pid;
	int	fd[2];
	int	tmp_fd;
	(void)argc;

	pid = 0;
	i = 0;
	tmp_fd = dup(STDIN_FILNO);
	while (argv[i] && argv[i + 1])
	{
		argv = &argv[i + 1];
		i = 0;
		while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
			i++;
		if (strcmp(argv[0], "cd") == 0)
		{
			if (i != 2)
				ft_putstr_f2("error: cd: bad arguments\n");
			else if (chdir(argv[1]) != 0)
			{
				ft_putstr_f2("error: cd: cannot change directory to ");
				ft_putstr_f2(argv[1]);
				write(2, "\n", 1);
			}
		}
		else if (i != 0 && (argv[i] == NULL || (strcmp(argv[i], ";") == 0)))
		{
			pid = fork();
			if (pid == 0)
			{
				dup2(tmp_fd, STDIN_FILNO);
				close(tmp_fd);
				if (ft_execute(argv, i, env))
					return (1);
			}
			else
			{
				close(tmp_fd);
				while(waitpid(-1, NULL, WUNTRACED) != -1)
					;
				tmp_fd = dup(STDIN_FILNO);
			}
		}
		else if (i != 0 && strcmp(argv[i], "|") == 0)
		{
			pipe(fd);
			pid = fork();
			if (pid == 0)
			{
				dup2(tmp_fd, STDIN_FILNO);
				dup2(fd[1], STDOUT_FILNO);
				close(fd[0]);
				close(fd[1]);
				close(tmp_fd);
				if (ft_execute(argv, i, env))
					return (1);
			}
			else
			{
				close(fd[1]);
				close(tmp_fd);
				tmp_fd = fd[0];
			}
		}
	}
	close(tmp_fd);
	return (0);
}
