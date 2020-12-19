#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main()
{
	while (1)
	{

		char megethos[20000];
		char *com = megethos;
		size_t size = 20000;

		printf("$ ");

		//user's input
		getline(&com, &size, stdin);

		size_t len = strlen(com);
		for (int j = 0; j < len; j++)
		{
			if (com[j] == '\t')
			{
				com[j] = ' ';
			}
		}

		//replacement of each '\n' with ';' in order to know each user's command
		for (int j = 0; j < len; j++)
		{
			if (com[j] == '\n')
			{
				com[j] = ';';
			}
		}

		//manage the case in which user will press ' ' or '\t' before the command's submit
		if (com[len - 2] == ' ' || com[len - 2] == '\t')
		{
			com[len - 2] = '@';
		}

		//store each user's word of the command in array
		int n = 0;
		char *mykey0;
		char **array_of_pipe_commands_0;
		mykey0 = strtok(com, ";");
		array_of_pipe_commands_0 = malloc(1 * sizeof(char *));
		do
		{
			size_t length1 = strlen(mykey0);
			array_of_pipe_commands_0 = (char **)realloc(array_of_pipe_commands_0, (n + 1) * sizeof(*array_of_pipe_commands_0));
			array_of_pipe_commands_0[n] = (char *)malloc(length1 * sizeof(char));
			array_of_pipe_commands_0[n] = mykey0;
			mykey0 = strtok(NULL, ";");
			n++;
		} while (mykey0 != NULL);
		int last_el = n;

		//When the array's size > 1, it means that user use more than one word in the command
		if (n > 1)
		{

			for (n = 0; n < last_el; n++)
			{

				//using the same method below,, if the command includes '|', break the command in parts of the '|'
				int i = 0;
				char *mykey;
				char **array_of_pipe_commands;
				mykey = strtok(array_of_pipe_commands_0[n], "|");
				array_of_pipe_commands = malloc(1 * sizeof(char *));
				do
				{
					size_t length0 = strlen(mykey);
					array_of_pipe_commands = (char **)realloc(array_of_pipe_commands, (i + 1) * sizeof(*array_of_pipe_commands));
					array_of_pipe_commands[i] = (char *)malloc(length0 * sizeof(char));
					array_of_pipe_commands[i] = mykey;
					mykey = strtok(NULL, "|");
					i++;
				} while (mykey != NULL);
				int each_command = i;
				int numberof_pipes = i - 1;

				//i have one pipe or more in the command
				if (i > 1)
				{

					//PIPE HANDLING
					int fd[numberof_pipes][2];
					pid_t pid0;

					for (i = 0; i < each_command; i++)
					{
						int p = 0;
						char *mykey2;
						char *cmd[10000];
						mykey2 = strtok(array_of_pipe_commands[i], " ");
						do
						{
							cmd[p] = mykey2;
							mykey2 = strtok(NULL, " ");
							p++;
						} while (mykey2 != NULL);
						cmd[p] = NULL;

						//PIPE HANDLING
						pipe(fd[i]);
						if (pipe(fd[i]) < 0)
							exit(1);

						//fork in order the child of the process to execute the command
						pid0 = fork();
						if (pid0 == -1)
						{
							perror("Error forking...");
							exit(1);
						}
						if (pid0 == 0)
						{
							if (i != 0 && i != each_command - 1)
							{
								dup2(fd[i - 1][0], 0);
								close(fd[i - 1][1]);
								dup2(fd[i][1], 1);
								close(fd[i][0]);
							}
							else if (i == 0)
							{
								dup2(fd[i][1], 1);
								close(fd[i][0]);
							}
							else
							{
								dup2(fd[i - 1][0], 0);
								close(fd[i - 1][1]);
							}
							//execute the command
							execvp(cmd[0], cmd);
							perror(cmd[0]);
							exit(1);
						}
						else
						{
							if (i != 0)
							{
								close(fd[i - 1][0]);
								close(fd[i - 1][1]);
							}
						}
					}
					int ptr;
					while ((pid0 = waitpid(-1, &ptr, 0)) != -1)
					{
					}
				}
				//user didnt use pipes
				else
				{
					//handling without pipes
					char *mykey;
					int i = 0;
					char **com3;
					mykey = strtok(array_of_pipe_commands_0[n], " ");
					com3 = malloc(1 * sizeof(char *));
					do
					{
						size_t length0 = strlen(mykey);
						com3 = (char **)realloc(com3, (i + 1) * sizeof(*com3));
						com3[i] = (char *)malloc(length0 * sizeof(char));
						com3[i] = mykey;
						mykey = strtok(NULL, " ");
						i++;
					} while (mykey != NULL);
					com3[i] = NULL;

					//cd command
					if (strcmp(com3[0], "cd") == 0)
					{
						char *mydirectory = com3[1];
						int changedir = chdir(mydirectory);
					}

					//EXIT
					if (strcmp(com3[0], "exit") == 0)
					{
						exit(1);
					}

					//FORK
					pid_t pid;
					pid = fork();

					if (pid == 0)
					{
						execvp(com3[0], com3);
						exit(1);
					}
					else
					{
						waitpid(pid, NULL, 0);
					}
				}

				free(array_of_pipe_commands);
				array_of_pipe_commands = NULL;
			}
		}
		else
		{

			int i = 0;
			char *mykey;
			char **array_of_pipe_commands;
			mykey = strtok(array_of_pipe_commands_0[0], "|");
			array_of_pipe_commands = malloc(1 * sizeof(char *));
			do
			{
				size_t length0 = strlen(mykey);
				array_of_pipe_commands = (char **)realloc(array_of_pipe_commands, (i + 1) * sizeof(*array_of_pipe_commands));
				array_of_pipe_commands[i] = (char *)malloc(length0 * sizeof(char));
				array_of_pipe_commands[i] = mykey;
				mykey = strtok(NULL, "|");
				i++;
			} while (mykey != NULL);
			int each_command = i;
			int numberof_pipes = i - 1;

			if (i > 1)
			{

				//PIPE HANDLING
				int fd[numberof_pipes][2];
				pid_t pid0;

				for (i = 0; i < each_command; i++)
				{
					int p = 0;
					char *mykey2;
					char *cmd[10000];
					mykey2 = strtok(array_of_pipe_commands[i], " ");
					do
					{
						cmd[p] = mykey2;
						mykey2 = strtok(NULL, " ");
						p++;
					} while (mykey2 != NULL);
					cmd[p] = NULL;

					pipe(fd[i]);
					if (pipe(fd[i]) < 0)
						exit(1);

					//FORK
					pid0 = fork();
					if (pid0 == -1)
					{
						perror("Error forking...");
						exit(1);
					}
					if (pid0 == 0)
					{
						if (i != 0 && i != each_command - 1)
						{
							dup2(fd[i - 1][0], 0);
							close(fd[i - 1][1]);
							dup2(fd[i][1], 1);
							close(fd[i][0]);
						}
						else if (i == 0)
						{
							dup2(fd[i][1], 1);
							close(fd[i][0]);
						}
						else
						{
							dup2(fd[i - 1][0], 0);
							close(fd[i - 1][1]);
						}
						execvp(cmd[0], cmd);
						perror(cmd[0]);
						exit(1);
					}
					else
					{
						if (i != 0)
						{
							close(fd[i - 1][0]);
							close(fd[i - 1][1]);
						}
					}
				}
				int ptr;
				while ((pid0 = waitpid(-1, &ptr, 0)) != -1)
				{
				}
			}
			else
			{

				char *mykey;
				int i = 0;
				char **com3;
				mykey = strtok(array_of_pipe_commands[i], " ");
				com3 = malloc(1 * sizeof(char *));
				do
				{
					size_t length0 = strlen(mykey);
					com3 = (char **)realloc(com3, (i + 1) * sizeof(*com3));
					com3[i] = (char *)malloc(length0 * sizeof(char));
					com3[i] = mykey;
					mykey = strtok(NULL, " ");
					i++;
				} while (mykey != NULL);
				com3[i] = NULL;

				//cd command
				if (strcmp(com3[0], "cd") == 0)
				{
					char *mydirectory = com3[1];
					int changedir = chdir(mydirectory);
				}

				//exit
				if (strcmp(com3[0], "exit") == 0)
				{
					exit(1);
				}

				//FORK
				pid_t pid;
				pid = fork();

				if (pid == 0)
				{
					execvp(com3[0], com3);
					exit(1);
				}
				else
				{
					waitpid(pid, NULL, 0);
				}
			}

			free(array_of_pipe_commands);
			array_of_pipe_commands = NULL;
		}
	}
	return 0;
}
