#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct action_node {
    char **args;  // an array of strings suitable to be passed to execvp
    struct action_node *next_act;
} Action;

typedef struct dep_node {
    struct rule_node *rule;
    struct dep_node *next_dep;
} Dependency;

typedef struct rule_node{
    char *target;
    Dependency *dependencies;
    Action *actions;
    struct rule_node *next_rule;
} Rule;

char *change_start(char *string){
    return string + 1;
}

void change_args(Action *node){
    char *args[5];
    for (int i = 0; i < 5; i++) {
        args[i] = "hi";
    }
    node->args = args;
}

int main(int argc, char **argv){
    Rule a = malloc(sizeof(Rule));
    if (a.dependencies == NULL) {
        printf("yes null");
    }
    return 0;
}

