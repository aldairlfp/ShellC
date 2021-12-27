#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/stat.h>
#include "shell.h"

#define LIMIT 256 // max number of tokens for a command
#define MAXLINE 1024 // max number of characters from user input

/**
 * Function used to initialize our shell. We used the approach explained in
 * http://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html
 */
void init() {
	// See if we are running interactively
	GBSH_PID = getpid();
	// The shell is interactive if STDIN is the terminal  
	GBSH_IS_INTERACTIVE = isatty(STDIN_FILENO);

	if (GBSH_IS_INTERACTIVE) {
		// Loop until we are in the foreground
		while (tcgetpgrp(STDIN_FILENO) != (GBSH_PGID = getpgrp()))
			kill(GBSH_PID, SIGTTIN);


		// Set the signal handlers for SIGCHILD and SIGINT
		act_child.sa_handler = signalHandler_child;
		act_int.sa_handler = signalHandler_int;

		sigaction(SIGCHLD, &act_child, 0);
		sigaction(SIGINT, &act_int, 0);

		// Put ourselves in our own process group
		setpgid(GBSH_PID, GBSH_PID); // we make the shell process the new process group leader
		GBSH_PGID = getpgrp();
		if (GBSH_PID != GBSH_PGID) {
			printf("Error, the shell is not process group leader");
			exit(EXIT_FAILURE);
		}
		// Grab control of the terminal
		tcsetpgrp(STDIN_FILENO, GBSH_PGID);

		// Save default terminal attributes for shell
		tcgetattr(STDIN_FILENO, &GBSH_TMODES);

		// Get the current directory that will be used in different methods
		currentDirectory = (char*)calloc(1024, sizeof(char));
	}
	else {
		printf("Could not make the shell interactive.\n");
		exit(EXIT_FAILURE);
	}
}

void Help(char* args[]) {
	if (args[1] == NULL) {
		printf("Members: Aldair Alfonso Perez y Ramon Cruz Alfonso \n \n");
		printf("Functionalities: \n ");
		printf("basic: Basic functionalities (3 points) \n");
		printf("background: Operator & and have processes in the background (0.5 points) \n");
		printf("spaces: Spaces between commands and parameters (0.5 points) \n");
		printf("history: Command history (0.5 points) \n");
		printf("ctrl + c: Capture and send signals to processes (0.5 points) \n");
		printf("if: Conditional expressions (1 points) \n");
		printf("help: Print a help (1 points) \n");
		printf("variables: Variables and backquotes (1 points) \n \n");

		printf("Comandos built-in: \n");
		printf("cd: Change directories \n");
		printf("exit: Finish shell \n ");
		printf("help: Show this help \n ");
		printf("if: Perform a conditional operation on a single line \n");
		printf("set/get/unset: Create variables, print their values, delete variables \n \n");
		printf("Total: 8 points");
	}
	else {
		if (args[1] == "prompt") printf("Our prompt shows us a list of characters, which make up a list of commands indicating that it is waiting for an order. In our case, where our first line shows the following: <user> @ <host> <cwd>> and then, before each command list we print: my-prompt, letting the user know that our shell. We had no difficulty in performing this functionality or exploiting test cases");
		else if (args[1] == "cd") printf("This command allows you to change the current address, it is very easy, since the chdir function does all the work. In the event that the address that is passed as a parameter is null, it sets x default home, and in case it is not valid, it will print that that address is not found. or we had no difficulty in performing this functionality or cases of tests that exploit");
		else if (args[1] == "<") printf("We implement this command to redirect the standard input/output of commands to/from files with >/</>>, for this we use \"open\" and \"close\". The open function returns an integer that identifies a descriptor and It has as parameters a pointer to the path of the file that we want to open and some flags that indicate how to open it: read only, write only, read / write or others. The \"close\" function closes the file descriptor that we pass as a parameter. Returns 0 on success and -1 on failure. Then we use the \"setenv\" function to define a new environment variable or change the existing one. Three arguments are required, the first and second of which are char pointers pointing to the variable name and its value, respectively. The third argument is of type int and specifies whether the value of the given variable should be overwritten if it already exists in the environment. The non-zero value of this argument denotes the overwrite behavior and the zero value the opposite.");
		else if (args[1] == "pipe") printf("A pipeline consists of a chain of processes connected in such a way that the output of each element in the chain is the input of the next. They allow communication and synchronization between processes. The use of data buffer between consecutive elements is common. To implement these we use");
		else if (args[1] == "history") printf("Our history consists of saving in a txt, which we save in the local folder where the project is located, all the commands that are executed listed and separated by line changes. To do this command, we use the functions fopen, fread and fwrite, With fopen we open the file, and if it does not exist it creates it, where the first parameter is the name of the file and the second is the mode, in this case we use \"a\" since it allows adding texti at the end of the file without replacing the previous text, and then with fwrite and fread to write and read the file respectively.");
		else if (args[1] == "ctrl + c") printf("The ctrl + c functionality consists in that when this combination of keys is touched, the current process is not destroyed, but it is executed again if the prompt is killed. To do this we create the methods \"signalHandler_child\" and \"signalHandler_int\"; \, in which if when killing the process it returns 0, we change the variable that controls whether we should make a prompt or not.");
		else if (args[1] == "help") printf("This functionality shows us how to use the commands, and in case of only using the help without another command, it shows the functionalities implemented");
	}
	// switch (args[1]) {
	// case "prompt" :
	// 	printf("Our prompt shows us a list of characters, which make up a list of commands indicating that it is waiting for an order. In our case, where our first line shows the following: <user> @ <host> <cwd>> and then, before each command list we print: my-prompt, letting the user know that our shell. We had no difficulty in performing this functionality or exploiting test cases");
	// 	break;
	// case "cd" :
	// 	printf("This command allows you to change the current address, it is very easy, since the chdir function does all the work. In the event that the address that is passed as a parameter is null, it sets x default home, and in case it is not valid, it will print that that address is not found. or we had no difficulty in performing this functionality or cases of tests that exploit");
	// 	break;
	// case "Redireccion de entrada y salida estandar" :
	// 	printf("We implement this command to redirect the standard input/output of commands to/from files with >/</>>, for this we use \"open\" and \"close\". The open function returns an integer that identifies a descriptor and It has as parameters a pointer to the path of the file that we want to open and some flags that indicate how to open it: read only, write only, read / write or others. The \"close\" function closes the file descriptor that we pass as a parameter. Returns 0 on success and -1 on failure. Then we use the \"setenv\" function to define a new environment variable or change the existing one. Three arguments are required, the first and second of which are char pointers pointing to the variable name and its value, respectively. The third argument is of type int and specifies whether the value of the given variable should be overwritten if it already exists in the environment. The non-zero value of this argument denotes the overwrite behavior and the zero value the opposite.");
	// 	break;
	// case "Pipe" : 
	// 	printf("A pipeline consists of a chain of processes connected in such a way that the output of each element in the chain is the input of the next. They allow communication and synchronization between processes. The use of data buffer between consecutive elements is common. To implement these we use");
	// 	break;
	// case "history" : 
	// 	printf("Our history consists of saving in a txt, which we save in the local folder where the project is located, all the commands that are executed listed and separated by line changes. To do this command, we use the functions fopen, fread and fwrite, With fopen we open the file, and if it does not exist it creates it, where the first parameter is the name of the file and the second is the mode, in this case we use \"a\" since it allows adding texti at the end of the file without replacing the previous text, and then with fwrite and fread to write and read the file respectively.");
	// 	break;
	// case "ctrl + c" : 
	// 	printf("The ctrl + c functionality consists in that when this combination of keys is touched, the current process is not destroyed, but it is executed again if the prompt is killed. To do this we create the methods \"signalHandler_child\" and \"signalHandler_int\"; \, in which if when killing the process it returns 0, we change the variable that controls whether we should make a prompt or not.");  
	// 	break;
	// case "help" : 
	// 	printf("This functionality shows us how to use the commands, and in case of only using the help without another command, it shows the functionalities implemented");
	// 	break;
}

// void saveHistory(char * args){
//     FILE * fichero = fopen(historyFileName, "a");
//     if(!fichero){
//         perror("fopen");
//         exit(EXIT_FAILURE);
//     }
// 	char * stringToSave = strcat(buffer,historyCount);
// 	strcat(stringToSave, " ");
// 	strcat(stringToSave, args);
//     fwrite(stringToSave,1,strlen(stringToSave), fichero);
// 	fwrite("\n", 1, strlen("\n"), fichero);
//     fclose(fichero);
// 	exit(EXIT_SUCCESS);
// }

// void LoadFromHistory(){
// 	FILE* inputFile = fopen(historyFileName, "rb+");
//     if(!inputFile) exit(EXIT_FAILURE);
// 	struct stat sb;
// 	if(stat(historyFileName, &sb) == -1){
// 		perror("stat");
// 		exit(EXIT_FAILURE);
// 	}
// 	char * loadHistory = malloc(sb.st_size);
// 	fread(loadHistory, sb.st_size, 1, inputFile);
// 	printf("%s\n", loadHistory);
// 	actualHistory = strcat(buffer,loadHistory);
// 	fclose(inputFile);
// 	free(loadHistory);
// 	exit(EXIT_SUCCESS);
// }

// void Again(int number){
// 	int i = 0;
// 	int j = 0;
// 	int count = 1;
// 	char * againCommand;
// 	while (i < strlen(actualHistory))
// 	{
// 		if(actualHistory[i] == "\n"){
// 			if(number == count){
// 				launchProg(againCommand, );
// 				break;
// 			}
// 			j = 0;
// 			againCommand = "";
// 			count++;
// 		}
// 		againCommand[j] = actualHistory[i];
// 		i++;
// 		j++;
// 	}
// }

/**
 * SIGNAL HANDLERS
 */

 /**
  * signal handler for SIGCHLD
  */
void signalHandler_child(int p) {
	/* Wait for all dead processes.
	 * We use a non-blocking call (WNOHANG) to be sure this signal handler will not
	 * block if a child was cleaned up in another part of the program. */
	while (waitpid(-1, NULL, WNOHANG) > 0) {
	}
	printf("\n");
}

/**
 * Signal handler for SIGINT
 */
void signalHandler_int(int p) {
	// We send a SIGTERM signal to the child process
	if (kill(pid, SIGTERM) == 0) {
		printf("\nProcess %d received a SIGINT signal\n", pid);
		no_reprint_prmpt = 1;
	}
	else {
		printf("\n");
	}
}

/**
 *	Displays the prompt for the shell
 */
void shellPrompt() {
	// We print the prompt in the form "<user>@<host> <cwd> >"
	char hostn[1204] = "";
	gethostname(hostn, sizeof(hostn));
	printf("myprompt $ ");
}

/**
 * Method to change directory
 */
int changeDirectory(char* args[]) {
	// If we write no path (only 'cd'), then go to the home directory
	if (args[1] == NULL) {
		chdir(getenv("HOME"));
		return 1;
	}
	// Else we change the directory to the one specified by the 
	// argument, if possible
	else {
		if (chdir(args[1]) == -1) {
			printf(" %s: no such directory\n", args[1]);
			return -1;
		}
	}
	return 0;
}

/**
 * Method used to manage the environment variables with different
 * options
 */
int manageEnviron(char* args[], int option) {
	char** env_aux;
	switch (option) {
		// Case 'environ': we print the environment variables along with
		// their values
	case 0:
		for (env_aux = environ; *env_aux != 0; env_aux++) {
			printf("%s\n", *env_aux);
		}
		break;
		// Case 'setenv': we set an environment variable to a value
	case 1:
		if ((args[1] == NULL) && args[2] == NULL) {
			printf("%s", "Not enought input arguments\n");
			return -1;
		}

		// We use different output for new and overwritten variables
		if (getenv(args[1]) != NULL) {
			printf("%s", "The variable has been overwritten\n");
		}
		else {
			printf("%s", "The variable has been created\n");
		}

		// If we specify no value for the variable, we set it to ""
		if (args[2] == NULL) {
			setenv(args[1], "", 1);
			// We set the variable to the given value
		}
		else {
			setenv(args[1], args[2], 1);
		}
		break;
		// Case 'unsetenv': we delete an environment variable
	case 2:
		if (args[1] == NULL) {
			printf("%s", "Not enought input arguments\n");
			return -1;
		}
		if (getenv(args[1]) != NULL) {
			unsetenv(args[1]);
			printf("%s", "The variable has been erased\n");
		}
		else {
			printf("%s", "The variable does not exist\n");
		}
		break;


	}
	return 0;
}

/**
* Method for launching a program. It can be run in the background
* or in the foreground
*/
void launchProg(char** args, int background) {
	int err = -1;

	if ((pid = fork()) == -1) {
		printf("Child process could not be created\n");
		return;
	}
	// pid == 0 implies the following code is related to the child process
	if (pid == 0) {
		// We set the child to ignore SIGINT signals (we want the parent
		// process to handle them with signalHandler_int)	
		signal(SIGINT, SIG_IGN);

		// We set parent=<pathname>/simple-c-shell as an environment variable
		// for the child
		setenv("parent", getcwd(currentDirectory, 1024), 1);

		// If we launch non-existing commands we end the process
		if (execvp(args[0], args) == err) {
			printf("Command not found");
			kill(getpid(), SIGTERM);
		}
	}

	// The following will be executed by the parent

	// If the process is not requested to be in background, we wait for
	// the child to finish.
	if (background == 0) {
		waitpid(pid, NULL, 0);
	}
	else {
		// In order to create a background process, the current process
		// should just skip the call to wait. The SIGCHILD handler
		// signalHandler_child will take care of the returning values
		// of the childs.
		printf("Process created with PID: %d\n", pid);
	}
}

/**
* Method used to manage I/O redirection
*/
void fileIO(char* args[], char* inputFile, char* outputFile, int option, int background) {
	int err = -1;

	int fileDescriptor; // between 0 and 19, describing the output or input file

	if ((pid = fork()) == -1) {
		printf("Child process could not be created\n");
		return;
	}
	if (pid == 0) {
		// printf("option %i\n", option);
		// Option 0: output redirection
		if (option == 0) {
			// We open (create) the file truncating it at 0, for write only
			fileDescriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600);
			// We replace de standard output with the appropriate file
			dup2(fileDescriptor, STDOUT_FILENO);
			close(fileDescriptor);
			// Option 1: input and output redirection
		}
		else if (option == 2) {
			// We open file for read only (it's STDIN)
			fileDescriptor = open(inputFile, O_RDONLY, 0600);
			// We replace de standard input with the appropriate file
			dup2(fileDescriptor, STDIN_FILENO);
			close(fileDescriptor);
			// // Same as before for the output file
			// fileDescriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600);
			// dup2(fileDescriptor, STDOUT_FILENO);
			// close(fileDescriptor);		 
		}

		setenv("parent", getcwd(currentDirectory, 1024), 1);

		if (execvp(args[0], args) == err) {
			printf("err");
			kill(getpid(), SIGTERM);
		}
	}
	waitpid(pid, NULL, 0);
}

char* getDirection(char* args[], int* index) {
	char* direction = (char*)malloc(sizeof(char));
	int k = *index + 1;
	while (args[k] != NULL)
	{
		if (strcmp(args[k], ">") != 0 || strcmp(args[k], "<") != 0 ||
			strcmp(args[k], ">>") != 0) {
			direction = strcat(direction, args[k]);
			if (args[k + 1] != NULL && (strcmp(args[k], ">") != 0 && strcmp(args[k], "<") != 0 &&
				strcmp(args[k], ">>") != 0)) {
				direction = strcat(direction, " ");

			}
		}
		k++;
	}
	*index = k - 1;
	// printf("Sali direction\n");
	return direction;
}

/**
* Method used to handle the commands entered via the standard input
*/
int commandHandler(char* args[]) {
	int i = 0;
	int j = 0;

	int fileDescriptor;
	int standardOut;

	int aux;
	int background = 0;

	char* args_aux[256];

	// We look for the special characters and separate the command itself
	// in a new array for the arguments
	while (args[j] != NULL) {
		if ((strcmp(args[j], ">") == 0) || (strcmp(args[j], "<") == 0) || (strcmp(args[j], "&") == 0) ||
			(strcmp(args[j], ">>") == 0)) {
			break;
		}
		args_aux[j] = args[j];
		j++;
	}

	// int k = 0;
	// while (args_aux[k] != NULL)
	// {
	// 	printf("%s\n", args_aux[k]);
	// 	k++;
	// }

	// 'exit' command quits the shell
	if (strcmp(args[0], "exit") == 0) exit(0);
	// 'pwd' command prints the current directory
	else if (strcmp(args[0], "pwd") == 0) {
		if (args[j] != NULL) {
			// If we want file output
			if ((strcmp(args[j], ">") == 0) && (args[j + 1] != NULL)) {
				fileDescriptor = open(args[j + 1], O_CREAT | O_TRUNC | O_WRONLY, 0600);
				// We replace de standard output with the appropriate file
				standardOut = dup(STDOUT_FILENO); 	// first we make a copy of stdout
													// because we'll want it back
				dup2(fileDescriptor, STDOUT_FILENO);
				close(fileDescriptor);
				printf("%s\n", getcwd(currentDirectory, 1024));
				dup2(standardOut, STDOUT_FILENO);
			}
		}
		else {
			printf("%s\n", getcwd(currentDirectory, 1024));
		}
	}
	// 'true' command clears the screen
	else if (strcmp(args[0], "true") == 0) return 0;
	// 'false' comando
	else if (strcmp(args[0], "false") == 0) return 1;
	// else if (strcmp(args[0], "again") == 0) {
	// 	if(args[1] != NULL){
	// 		Again((int) strtol(args[1], NULL, 10);)
	// 	}
	// }
	// 'cd' command to change directory
	else if (strcmp(args[0], "cd") == 0) changeDirectory(args);
	// 'environ' command to list the environment variables
	else if (strcmp(args[0], "environ") == 0) {
		if (args[j] != NULL) {
			// If we want file output
			if ((strcmp(args[j], ">") == 0) && (args[j + 1] != NULL)) {
				fileDescriptor = open(args[j + 1], O_CREAT | O_TRUNC | O_WRONLY, 0600);
				// We replace de standard output with the appropriate file
				standardOut = dup(STDOUT_FILENO); 	// first we make a copy of stdout
													// because we'll want it back
				dup2(fileDescriptor, STDOUT_FILENO);
				close(fileDescriptor);
				manageEnviron(args, 0);
				dup2(standardOut, STDOUT_FILENO);
			}
		}
		else {
			manageEnviron(args, 0);
		}
	}
	// 'setenv' command to set environment variables
	else if (strcmp(args[0], "setenv") == 0) manageEnviron(args, 1);
	// 'unsetenv' command to undefine environment variables
	else if (strcmp(args[0], "unsetenv") == 0) manageEnviron(args, 2);
	else {
		// If none of the preceding commands were used, we invoke the
		// specified program. We have to detect if I/O redirection,
		// piped execution or background execution were solicited
		while (args[i] != NULL && background == 0) {
			// If background execution was solicited (last argument '&')
			// we exit the loop
			if (strcmp(args[i], "&") == 0) {
				background = 1;
				// If '|' is detected, piping was solicited, and we call
				// the appropriate method that will handle the different
				// executions
			}
			else if (strcmp(args[i], "<") == 0 || strcmp(args[i], ">") == 0 ||
				strcmp(args[i], ">>") == 0) {
				char* directionO;
				char* directionI;
				int option;
				int k = i;
				while (args[k] != NULL)
				{
					// printf("%s\n", args[k]);
					if (strcmp(args[k], "|") != 0) {
						if (strcmp(args[k], "<") == 0) {
							directionI = getDirection(args, &k);
							option = 2;
						}
						if (strcmp(args[k], ">") == 0) {
							directionO = getDirection(args, &k);
							option = 0;
						}
						if (strcmp(args[k], ">>") == 0) {
							directionO = getDirection(args, &k);
							option = 1;
						}
					}
					k++;
				}
				// printf("%i\n", k);
				// args_aux[k] = NULL;
				// char* inputFile;
				// char* outputFile;
				// if (strcmp(args[i], "<") == 0) {
				// 	inputFile = directionI;
				// 	outputFile = NULL;
				// }
				// if (strcmp(args[i], ">") == 0) {
				// 	inputFile = NULL;
				// 	outputFile = directionO;
				// }
				// if (strcmp(args[i], ">>") == 0) {
				// 	inputFile = NULL;
				// 	outputFile = directionO;
				// }
				//printf("%s\n",directionO);
				// if (directionI != NULL)
				// 	printf("%s input 0\n", directionI);
				// if (directionO != NULL)
				// 	printf("%s output 0\n", directionO);
				// printf("hi\n");
				fileIO(args_aux, directionI, directionO, option, background);
				directionI = NULL;
				directionO = NULL;
				free(directionI);
				free(directionO);
				return 0;
			}
			i++;
		}
		// We launch the program with our method, indicating if we
		// want background execution or not
		args_aux[i] = NULL;
		// printf("%i i\n", i);
		// int k = 0;
		// while (args_aux[k] != NULL)
		// {
		// 	printf("%s\n", args[k]);
		// 	k++;
		// }

		launchProg(args_aux, background);

		/**
		 * For the part 1.e, we only had to print the input that was not
		 * 'exit', 'pwd' or 'clear'. We did it the following way
		 */
		 //	i = 0;
		 //	while(args[i]!=NULL){
		 //		printf("%s\n", args[i]);
		 //		i++;
		 //	}
	}
	return 1;
}

/**
* Method used to manage pipes.
*/
int pipeHandler(char* args[]) {
	//TODO: Poner esto mas bonito
	//Para continuar revisando desde donde me quede cuando la llamada venga de un pipe
	int current = 0;
	//Para saber si algun metodo me devuelve -1
	int correctOutput = 0;
	//Para guardar los file descriptors de STDIN y STDOUT cuando haga falta
	int fd;
	//Para ir guardando los pedazos de comandos separados por pipes
	char* newCommandLine[LIMIT];

	int pipes;
	int pipefd[2];

	//Dentro del while me encargo de los caracteres especiales
	while (args[current] != NULL)
	{
		if (strcmp(args[current], "|") == 0) {
			pipes = 1;
			//Terminar con NULL el array que contiene el comando que se va a ejecutar antes del caracter especial
			newCommandLine[current] = NULL;

			//Crear el pipe (lee de la izquierda y escribe en la derecha)
			pipe(pipefd);

			//Ejecuta el comando que escribe en el pipe
			int child2Pid = fork();
			if (child2Pid == 0) {
				//Remplaza la salida actual por el fd de escritura del pipe
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[0]);
				close(pipefd[1]);

				//Ejecuta el comando que estaba antes del pipe
				correctOutput = commandHandler(newCommandLine);
				exit(correctOutput);
			}
			//Espera a que se ejecute el primer comando
			waitpid(child2Pid, NULL, 0);

			//Cierra el fd de escritura(si esto no se hace aqui el comando que lee se queda esperando mas input)
			close(pipefd[1]);

			break;
		}
		else {
			newCommandLine[current] = args[current];
		}

		// printf("pipe %s\n", args[current]);
		current++;
	}
	if (pipes == 1) {
		// printf("entre pipe");
		current++;
		int current1 = 0;
		char* newCommandLine1[LIMIT];
		while (args[current] != NULL)
		{
			newCommandLine1[current1] = args[current];
			current1++;
			current++;
		}


		//A current le resto startPos por si no empece a revisar desde el principio de args 
		newCommandLine[current1] = NULL;

		// int k = 0;
		// while (args[k] != NULL)
		// {
		// 	printf("%s\n", newCommandLine1[k++]);
		// }

		//Ejecuta el comando que escribe en el pipe
		int childPid = fork();
		if (childPid == 0) {
			//Remplaza la salida actual por el fd de escritura del pipe
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			close(pipefd[1]);

			//Ejecuta el comando que estaba antes del pipe
			correctOutput = commandHandler(newCommandLine1);
			exit(correctOutput);
		}
		//Espera a que se ejecute el primer comando
		waitpid(childPid, NULL, 0);

		//Cierra el fd de escritura(si esto no se hace aqui el comando que lee se queda esperando mas input)
		close(pipefd[0]);
	}
	else
	{
		newCommandLine[current] = NULL;
		// current = 0;
		// while (newCommandLine[current] != NULL)
		// {
		// 	printf("newcomman %s\n", newCommandLine[current]);
		// 	current++;
		// }


		//Llegados a este punto el comando actual no tiene caracteres especiales asi que lo ejecuto
		return commandHandler(newCommandLine);
	}
}



/**
* Main method of our shell
*/
int main(int argc, char* argv[], char** envp) {
	char line[MAXLINE]; // buffer for the user input
	char* prevTokens[LIMIT]; // array for the different tokens in the command
	int numTokens;

	no_reprint_prmpt = 0; 	// to prevent the printing of the shell
							// after certain methods
	pid = -10; // we initialize pid to an pid that is not possible

	// We call the method of initialization and the welcome screen
	init();

	// We set our extern char** environ to the environment, so that
	// we can treat it later in other methods
	environ = envp;

	// We set shell=<pathname>/simple-c-shell as an environment variable for
	// the child
	setenv("shell", getcwd(currentDirectory, 1024), 1);

	// Main loop, where the user input will be read and the prompt
	// will be printed
	while (TRUE) {
		// We print the shell prompt if necessary
		if (no_reprint_prmpt == 0) shellPrompt();
		no_reprint_prmpt = 0;

		// We empty the line buffer
		memset(line, '\0', MAXLINE);

		// We wait for user input
		fgets(line, MAXLINE, stdin);
		// printf("%s\n", line);

		// If nothing is written, the loop is executed again
		if ((prevTokens[0] = strtok(line, " \n")) == NULL) continue;

		// We read all the tokens of the input and pass it to our
		// commandHandler as the argument
		numTokens = 1;
		while ((prevTokens[numTokens] = strtok(NULL, " \n")) != NULL) numTokens++;
		// printf("%i tokens\n", numTokens);
		char* tokens[MAXLINE];
		int lastToken = 0;
		for (int i = 0; i < numTokens && strcmp(prevTokens[i], "#") != 0; i++)
		{
			tokens[i] = prevTokens[i];
			lastToken++;
			// printf("%s\n", tokens[i]);
		}
		// printf("%i lasttokens\n", lastToken);
		tokens[lastToken] = NULL;

		// Se manejan las condiciones
		if (strcmp(tokens[0], "if") == 0)
		{
			int startToken = 1;
			int rightOrder = 0;
			int elseExist = 0;
			int countSyntax = 1;
			while (tokens[startToken] != NULL && rightOrder == 0)
			{
				if (strcmp(tokens[startToken], "if") == 0 || strcmp(tokens[startToken], "end") == 0
					|| strcmp(tokens[startToken], "else") == 0)
					rightOrder = 1;
				if (strcmp(tokens[startToken], "then") == 0) {
					countSyntax++;
					startToken++;
					break;
				}
				// printf("startToken %i\n", startToken);
				startToken++;
			}
			// printf("%i\n", rightOrder);
			// printf("Condition1\n");

			while (tokens[startToken] != NULL && rightOrder == 0)
			{
				if (strcmp(tokens[startToken], "if") == 0 || strcmp(tokens[startToken], "then") == 0)
					rightOrder = 1;
				if (strcmp(tokens[startToken], "else") == 0 || strcmp(tokens[startToken], "end") == 0) {
					if (strcmp(tokens[startToken], "else") == 0)
						elseExist = 1;
					countSyntax++;
					startToken++;
					break;
				}
				startToken++;
			}
			// printf("rightOrder %i\n", rightOrder);
			// printf("Condition2\n");

			while (tokens[startToken] != NULL && rightOrder == 0)
			{
				if (strcmp(tokens[startToken], "if") == 0 || strcmp(tokens[startToken], "then") == 0 ||
					strcmp(tokens[startToken], "else") == 0)
					rightOrder = 1;
				if (strcmp(tokens[startToken], "end") == 0) {
					countSyntax++;
					startToken++;
					break;
				}
				startToken++;
			}
			// printf("%i\n", rightOrder);
			// printf("%i\n", rightOrder);
			// printf("%i\n", elseExist);
			// printf("%i\n", countSyntax);
			// printf("Condition3\n");

			if (rightOrder == 0 &&
				(elseExist == 0 && countSyntax == 3 || elseExist == 1 && countSyntax == 4)) {
				startToken = 1;
				char* tokensCondition[LIMIT];
				while (tokens[startToken] != NULL && strcmp(tokens[startToken], "then") != 0) {
					// printf("%d\n", startToken);
					tokensCondition[startToken - 1] = tokens[startToken];
					startToken++;

				}
				if (tokens[startToken] == NULL) continue;
				// printf("Condition\n");
				tokensCondition[startToken - 1] = NULL;
				if (tokensCondition[0] == NULL) continue;
				startToken++;

				int command = pipeHandler(tokensCondition);

				// printf("%i\n", command);

				if (command == 0) {
					char* tokensThen[LIMIT];
					int startThen = 0;
					while (tokens[startToken] != NULL && strcmp(tokens[startToken], "else") != 0 &&
						strcmp(tokens[startToken], "end") != 0)
					{
						tokensThen[startThen] = tokens[startToken];
						startThen++;
						startToken++;
					}
					if (tokens[startToken] == NULL) continue;
					tokensThen[startThen] = NULL;
					if (tokensThen[0] == NULL) continue;
					// printf("then\n");
					// int i = 0;
					// while (tokensThen[i] != NULL)
					// {
					// 	printf("%s", tokensThen[i]);
					// }


					pipeHandler(tokensThen);
				}
				else
				{
					while (tokens[startToken] != NULL && strcmp(tokens[startToken], "else") != 0)
						startToken++;
					startToken++;
					char* tokensElse[LIMIT];
					int startElse = 0;
					// printf("%i\n", command);
					while (tokens[startToken] != NULL && strcmp(tokens[startToken], "end") != 0)
					{
						tokensElse[startElse] = tokens[startToken];
						startElse++;
						startToken++;
					}
					// int k = 0;
					// while (tokensElse[k] != NULL)
					// {
					// 	printf("%s\n", tokensElse[k++]);
					// }
					if (tokens[startToken] == NULL) continue;
					tokensElse[startElse] = NULL;
					if (tokensElse[0] == NULL) continue;

					// int i = 0;
					// while (tokensElse[i] != NULL)
					// {
					// 	printf("%s", tokensElse[i]);
					// }

					// k = 0;
					// while (tokensElse[k] != NULL)
					// {
					// 	printf("%s\n", tokensElse[k++]);
					// }

					// printf("else\n");

					pipeHandler(tokensElse);
				}
			}
		}
		else {
			// int k = 0;
			// while (tokens[k] != NULL)
			// {
			// 	printf("token %s\n", tokens[k]);
			// 	k++;
			// }
			pipeHandler(tokens);
		}
	}
	exit(0);
}
