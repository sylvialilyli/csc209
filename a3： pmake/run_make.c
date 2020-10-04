#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "pmake.h"


void run_make(char *target, Rule *rules, int pflag) {
    // TODO
    Rule *rule_to_run = rules;
    //find the rule if target is not Null
    if (target != NULL) {
        rule_to_run = find_rule(rules, target);
        //if rule not found
        if (rule_to_run == NULL) {
            fprintf(stderr, "Can not find the corresponding rule.");
            exit(1);
        }
    }

    //update target and dependencies
    struct stat targetStat, depState;
    int change = 0;
    Dependency *curr_dep = rule_to_run->dependencies;
    int target_exist = stat(rule_to_run->target, &targetStat);
    if (target_exist < 0) {
        change = 1;
    }
    //recursive way
    if (pflag == 0) {
        while (curr_dep != NULL) {
            Rule *curr_dep_rule = curr_dep->rule;
            run_make(curr_dep_rule->target, rules, 0);
            if (target_exist == 0) {
                stat(curr_dep_rule->target, &depState);
                change += compare_time(targetStat.st_mtim, depState.st_mtim);
            }
            curr_dep = curr_dep->next_dep;
        }
    }
    // parallel way
    else if (pflag == 1) {
        int r = 1;
        int i = 0;
        while (curr_dep != NULL) {
            if (r > 0) {
                r = fork();
                i++;
            }
            if (r < 0) {
                perror("fork");
                exit(1);
            } else if(r == 0) {
                Rule *curr_dep_rule = curr_dep->rule;
                run_make(curr_dep_rule->target, rules, 1);
            }
            curr_dep = curr_dep->next_dep;
        }
        if (target_exist == 0 && r > 0) {
            for (int j = 0; j < i; j++) {
                pid_t pid;
                int status;
                if( (pid = wait(&status)) == -1) {
                    perror("wait");
                }
            }
            while (curr_dep != NULL) {
                Rule *curr_dep_rule = curr_dep->rule;
                stat(curr_dep_rule->target, &depState);
                change += compare_time(targetStat.st_mtim, depState.st_mtim);
                curr_dep = curr_dep->next_dep;
            }
        }
    }

    //determine if perform action or not
    if (change != 0) {
        Action *curr_act = rule_to_run->actions;
        int re;
        while (curr_act != NULL) {
            if ((re = fork()) > 0) {
                int status;
                if (wait(&status) != -1) {
                    if (WIFEXITED(status)) {
                        if (WEXITSTATUS(status) == -1) {
                            perror("execvp");
                            exit(1);
                        }
                    }
                } else {
                   perror("wait");
                   exit(1);
                }
            } else if (re == 0) {
                execvp((curr_act->args)[0], curr_act->args);
            } else {
                perror("fork");
                exit(1);
            }
            curr_act = curr_act->next_act;
        }
    }

    return ;
}

int compare_time(struct timespec target_time, struct timespec dep_time) {
    if ((dep_time.tv_sec) > (target_time.tv_sec)) {
        return 1;
    } else if (((dep_time.tv_sec) == (target_time.tv_sec)) &&
               ((dep_time.tv_nsec) > (target_time.tv_nsec))) {
        return 1;
    }
    return 0;
}

