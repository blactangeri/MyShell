/*MYSHELL
 * @author Xin Dong
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <queue>
#include <fstream>

using namespace std;

typedef struct node {
    char path[100];
    char **arguments;
    struct node *next;
} list;

char **shortArray(char *str[], int);
char **dividePipe(char **);
void printNode();
void addNode(char *str);
void delNode(char *str);
void addNode2(char *str, char **);
void delNode2(char *str, char **);
void printNode2();
int countSpace(char *str);
char *substring(char *, int, int);
void returnMod(char *str);
void revOrder(char *strp[], int);
void recordHistory(string);
void printHistory();
int myexecu(list *);
void runCmdWithPipe(char **argv, int);
char *rmspace(char *);

list *head;
list *arghead;
vector<string> history;

char *pwd = getenv("PWD");
char *hostname = getenv("HOSTNAME");
char *user = getenv("USER");

//main function
int main(int argc, char *argv[]) {
    if (argc > 2) {
        cout << "Too many arguments!" << endl;
        exit(-1);
    }

    cout << "                           Welcome to Xin's shell!\n\n\n";

    if (argc == 1) {

        int status;
        pid_t pid2;
        head = NULL;

        char *sp = " ";

        printf("[%s@%s:%s]#", user, hostname, pwd);

        //preloaded path
        addNode("/bin/");
        addNode("/usr/bin/");

        while (1) {
            arghead = NULL;
            fflush(stdin);
            char cmd2[4096];
            fgets(cmd2, 4096, stdin);

            char *cmd = rmspace(cmd2);
            int arg_count = countSpace(cmd) + 1;
            char *arg_vector[arg_count];

            recordHistory(cmd);

            //readin all the arguments
            char *arg = strtok(cmd, sp);
            int count = 0;
            while (arg) {
                arg_vector[count] = arg;
                arg = strtok(NULL, sp);
                count++;
            }

            count = arg_count;
            returnMod(arg_vector[count - 1]);
            arg_vector[count] = (char *) 0;

            if (strcmp(arg_vector[0], "exit") == 0) {
                printf("Closing...\n");
                exit(-1);
            }

            if (strcmp(arg_vector[0], "cd") == 0) {
                if (chdir(arg_vector[1]) < 0) {
                    fprintf(stderr, "error: %s\n", strerror(errno));
                };

                printf("[%s@%s:%s]#", user, hostname, pwd);

                continue;
            }

            if (strcmp(arg_vector[0], "history") == 0) {
                printHistory();
                continue;
            }


            if ((pid2 = fork()) < 0) {
                fprintf(stderr, "error: %s\n", strerror(errno));
                return -1;

            }
            if (pid2 == 0) {
                runCmdWithPipe(arg_vector, count);
                exit(0);
            }
            else {
                wait(&status);
                printf("[%s@%s:%s]#", user, hostname, pwd);
            }
        }
    }
    else if (argc == 2) {
        int status;
        pid_t pid2;
        head = NULL;

        char *sp = " ";

        printf("[%s@%s:%s]#", user, hostname, pwd);

        //preloaded path
        addNode("/bin/");
        addNode("/usr/bin/");

        FILE *fp;
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        fp = fopen(argv[1], "r");
        if (fp == NULL) exit(EXIT_FAILURE);

        while ((read = getline(&line, &len, fp)) != -1) {
            arghead = NULL;

            char *cmd = rmspace(line);
            int arg_count = countSpace(cmd) + 1;
            char *arg_vector[arg_count];

            //readin all the arguments
            char *arg = strtok(cmd, sp);
            int count = 0;
            while (arg) {
                arg_vector[count] = arg;
                arg = strtok(NULL, sp);
                count++;
            }

            count = arg_count;
            returnMod(arg_vector[count - 1]);
            arg_vector[count] = (char *) 0;

            recordHistory(cmd);

            if (strcmp(arg_vector[0], "exit") == 0) {
                printf("Closing...\n");
                exit(-1);
            }

            if (strcmp(arg_vector[0], "cd") == 0) {

                if (chdir(arg_vector[1]) < 0) {
                    fprintf(stderr, "error: %s\n", strerror(errno));
                };

                continue;
            }

            if (strcmp(arg_vector[0], "history") == 0) {
                printHistory();
                continue;
            }


            if ((pid2 = fork()) < 0) {
                fprintf(stderr, "error: %s\n", strerror(errno));
                return -1;

            }
            if (pid2 == 0) {
                runCmdWithPipe(arg_vector, count);
                exit(0);
            }
            else {
                wait(&status);
            }
        }
    }
}


// function declarations
// reverse the order of a linked list
void revOrder(char *strp[], int num) {
    num--;
    int i = 0;
    for (i = 0; i < num / 2; i++) {
        char *temp = strp[i];
        strp[i] = strp[num - i];
        strp[num - i] = temp;
    }

}

char **shortArray(char *str[], int len) {
    char **sarray = (char **) malloc((len + 1) * sizeof(char *));
    int i = 0;
    for (; i < len; i++) {
        sarray[i] = str[i];
    }
    sarray[i] = (char *) 0;
    return sarray;
}

// iterate through a linked list and print values
void printNode() {
    list *it = head;
    while (it != NULL) {

        printf("%s:", it->path);
        it = it->next;
    }
    printf("\n");
}

// print argument list
void printNode2() {
    list *it = arghead;
    while (it != NULL) {

        printf("%s:", it->path);
        it = it->next;
    }
    printf("\n");
}


// add node to the path list
void addNode(char *str) {

    list *next = (list *) malloc(sizeof(list));
    strncpy(next->path, str, 100);
    next->next = NULL;
    if (head == NULL) {
        head = next;
        return;
    }
    list *it = head;

    while (it->next != NULL) {
        it = it->next;
        // printf("Deadhere\n");
    }
    it->next = next;
}

// add node to the argument list
void addNode2(char *str, char **arg) {

    list *next = (list *) malloc(sizeof(list));
    strncpy(next->path, str, 100);
    next->arguments = arg;
    next->next = NULL;
    if (arghead == NULL) {
        arghead = next;
        return;
    }
    list *it = arghead;

    while (it->next != NULL) {
        it = it->next;
        //printf("Deadhere\n");
    }
    it->next = next;
}

// delete node from path
void delNode(char *str) {
    //: printf("test");
    list *it = head;
    if (head == NULL) {
        fprintf(stderr, "error: path %s not found\n", str);
//        printf("No node!");return;
        return;

    }
    if (strcmp(head->path, str) == 0) {

        head = head->next;
        free(it);
    }

    else {
        while (it->next != NULL && strcmp(it->next->path, str) != 0) {
            it = it->next;
        }

        if (it->next == NULL || strcmp(it->next->path, str) != 0) {
            fprintf(stderr, "error: path %s not found\n", str);
//             printf("Path Not Found!\n");
        }
            /* if(it==NULL)
                 printf("Path Not Found!\n");*/
        else {
            it->next = it->next->next;
            free(it->next);
        }
    }
}

// delete node from argument list
void delNode2(char *str, char **arg) {
    list *it = head;
    if (head == NULL) {
        printf("No node!");
        return;
    }
    if (strcmp(head->path, str) == 0) {
        head = head->next;
        free(it);
    }

    else {
        while (strcmp(it->next->path, str) != 0 && it != NULL) {
            it = it->next;
        }
        if (it == NULL)
            printf("Path Not Found!\n");
        else {
            it->next = it->next->next;
            free(it->next);
        }
    }
}

//function to take a substring out from the previous string
char *substring(char *str, int a, int b) {
    char *ret = (char *) malloc((b - a + 2) * sizeof(char));
    int j = 0;
    int i = 0;
    for (i = a; i <= b; i++) {
        ret[j] = str[i];
        j++;
    }
    ret[j] = '\0';
    return ret;
}

//function to count the number of spaces in the commands
int countSpace(char *str) {
    int i = 0;
    int j = 0;
    for (; str[i] != '\0'; i++) {
        if (str[i] == ' ' && str[i - 1] != ' ' && str[i + 1] != '\n') {
            j++;
        }
    }
    return j;
}

//to modify endofline to endoffile
void returnMod(char *str) {
    while (1) {
        if (*str == '\n') {
            *str = '\0';
            return;
        }
        str++;
    }
}

void runCmdWithPipe(char **argv, int count) {
    int status;
    pid_t pid;
    int pipeflag = 0;
    int i, j, k;
    int count2;
    for (i = 0; i < count; i++) {
        if (strcmp(argv[i], "|") == 0) {
            pipeflag++;
        }
    }

    int total[pipeflag + 2];
    total[0] = 0;
    total[pipeflag + 1] = count;
    int l = 0, m = 1;

    for (; l < count; l++) {
        if (strcmp(argv[l], "|") == 0) {
            total[m] = l + 1;
            m++;
        }
    }
    char **addr;
    char **newaddr;
    int o = 0;
    int n = 0;

    if (pipeflag == 0) {
        newaddr = &argv[total[n]];
        o = total[n + 1] - total[n];
        addr = shortArray(newaddr, o);
        addNode2(argv[total[n]], addr);
    }
    else {
        for (; n < pipeflag + 1; n++) {
            newaddr = &argv[total[n]];
            o = total[n + 1] - total[n] - 1;
            if (n == pipeflag) o++;
            addr = shortArray(newaddr, o);
            addNode2(argv[total[n]], addr);
        }
    }

    int p = 0;
    int numPipes = pipeflag;
    int pipefds[2 * numPipes];

    for (i = 0; i < (numPipes); i++) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("Can not pipe");
            exit(1);
        }
    }

    while (arghead) {
        pid = fork();
        if (pid == 0) {
            if (arghead->next) {
                if (dup2(pipefds[p * 2 + 1], 1) < 0) {
                    exit(1);
                }
            }

            if (p != 0) {
                if (dup2(pipefds[(p - 1) * 2], 0) < 0) {
                    perror(" dup2");
                    exit(1);
                }
            }

            for (i = 0; i < 2 * numPipes; i++) {
                close(pipefds[i]);
            }

            if (myexecu(arghead) < 0) {
                fprintf(stderr, "error: %s\n", strerror(errno));

                perror(arghead->path);
                exit(1);
            }
        }
        else if (pid < 0) {
            perror("error");
            exit(1);
        }
        arghead = arghead->next;
        p++;
    }
    for (i = 0; i < 2 * numPipes; i++) {
        close(pipefds[i]);
    }

    for (i = 0; i < numPipes + 1; i++)
        wait(&status);
}

//
char *rmspace(char *raw) {
    int temp = 0;
    while (raw[temp] == ' ') { temp++; }
    return &raw[temp];
    /*
    char * temp=raw;
    while(*temp!=' '){
        temp++;

    }
    return temp;*/
}

//function to handle each pipeline
int myexecu(list *top) {

    list *it = top;
    char **ars = top->arguments;
    int count = 0;
    while (ars[count] != (char *) 0) {
        count++;
    }
    int breakpoint = count;

    int tcounter = 0;
    while (ars[tcounter] != (char *) 0) {
        if (strcmp(ars[tcounter], ">") == 0 || strcmp(ars[tcounter], "<") == 0 || strcmp(ars[tcounter], "2>") == 0) {
            breakpoint = tcounter;
            break;
        }
        tcounter++;
    }

    int fileopen;
    int fileread;

    char *argv2[breakpoint + 1];
    for (tcounter = 0; tcounter < breakpoint; tcounter++) {
        argv2[tcounter] = ars[tcounter];
    }
    argv2[breakpoint] = (char *) 0;

    for (tcounter = 0; tcounter < count; tcounter++) {
        if (strcmp(ars[tcounter], ">") == 0) {

            tcounter++;
            if ((fileopen = open(ars[tcounter], (O_CREAT | O_RDWR), 0644)) < 0) {
                fprintf(stderr, "error: %s\n", strerror(errno));
                exit(-1);
            }
            else {
                if (dup2(fileopen, 1) < 0) {
                    fprintf(stderr, "error: %s\n", strerror(errno));
                    exit(-1);
                }
            }
        }

        if (strcmp(ars[tcounter], "<") == 0) {
            tcounter++;
            if ((fileopen = open(ars[tcounter], O_RDONLY)) < 0) {
                fprintf(stderr, "error: %s\n", strerror(errno));
                exit(-1);
            }

            else {
                if (dup2(fileopen, 0) < 0) {
                    fprintf(stderr, "error: fail to redirect\n");
                    exit(-1);
                }
            }
        }

        if (strcmp(ars[tcounter], "2>") == 0) {

            tcounter++;
            if ((fileopen = open(ars[tcounter], (O_CREAT | O_RDWR), 0644)) < 0) {
                fprintf(stderr, "error: %s\n", strerror(errno));
                exit(-1);
            }
            else {
                if (dup2(fileopen, 2) < 0) {
                    fprintf(stderr, "error: %s\n", strerror(errno));
                    exit(-1);
                }
            }
        }
    }

    //system provided functions
    int retv = 0;
    if (head == NULL) {
        fprintf(stderr, "error: No path to execute command\n");
        exit(0);
    }

    list *iter = head;
    char *command = iter->path;
    strcat(command, top->path);

    while ((retv = execv(command, argv2) == -1) && iter != NULL) {
        iter = iter->next;
        command = iter->path;
        if (command == NULL) {
            fprintf(stderr, "Command %s not found.\n", ars[0]);
            exit(0);
        }
        strcat(command, ars[0]);
    }
    close(fileopen);
    exit(0);
}

void recordHistory(string s) {
    if (history.size() > 100) history.erase(history.begin());
    if (!s.empty() && s[s.size() - 1] == '\n') s.erase(s.size() - 1);
    history.push_back(s);
}

void printHistory() {
    for (int i = 0; i < history.size(); ++i) {
        cout << history[i] << endl;
    }
    printf("[%s@%s:%s]#", user, hostname, pwd);

}