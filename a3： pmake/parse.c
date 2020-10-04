#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "pmake.h"


/* Print the list of actions */
void print_actions(Action *act) {
    while(act != NULL) {
        if(act->args == NULL) {
            fprintf(stderr, "ERROR: action with NULL args\n");
            act = act->next_act;
            continue;
        }
        printf("\t");

        int i = 0;
        while(act->args[i] != NULL) {
            printf("%s ", act->args[i]) ;
            i++;
        }
        printf("\n");
        act = act->next_act;
    }    
}

/* Print the list of rules to stdout in makefile format. If the output
   of print_rules is saved to a file, it should be possible to use it to 
   run make correctly.
 */
void print_rules(Rule *rules){
    Rule *cur = rules;
    
    while(cur != NULL) {
        if(cur->dependencies || cur->actions) {
            // Print target
            printf("%s : ", cur->target);
            
            // Print dependencies
            Dependency *dep = cur->dependencies;
            while(dep != NULL){
                if(dep->rule->target == NULL) {
                    fprintf(stderr, "ERROR: dependency with NULL rule\n");
                }
                printf("%s ", dep->rule->target);
                dep = dep->next_dep;
            }
            printf("\n");
            
            // Print actions
            print_actions(cur->actions);
        }
        cur = cur->next_rule;
    }
}

/* Create the rules data structure and return it.
   Figure out what to do with each line from the open file fp
     - If a line starts with a tab it is an action line for the current rule
     - If a line starts with a word character it is a target line, and we will
       create a new rule
     - If a line starts with a '#' or '\n' it is a comment or a blank line 
       and should be ignored. 
     - If a line only has space characters ('', '\t', '\n') in it, it should be
       ignored.
 */
Rule *parse_file(FILE *fp) {

    // TODO
    char line[MAXLINE + 1];
    Rule *first_rule = malloc(sizeof(Rule));
    Rule *curr_rule = first_rule;
    Rule *last_rule = first_rule;
    char **split;
    while (fgets(line, MAXLINE + 1, fp) != NULL) {
        line[strlen(line) - 1] = '\0'; //eliminate '\n' at the end of the line
        if (is_comment_or_empty(line) == 0) {
            char first = line[0];
            if (first == '\t') {
                split = build_args(line);
                add_action(curr_rule, split);
            }
            else {
                split = build_target(line);
                char *target_here = split[0];
                Rule *temp = find_rule(first_rule, target_here);
                if (temp != NULL) {
                    curr_rule = temp;
                }
                else if (curr_rule->target != NULL) {
                    last_rule->next_rule = malloc(sizeof(Rule));
                    curr_rule = last_rule->next_rule;
                    last_rule = curr_rule;
                }
                last_rule = add_new_rule(curr_rule, split, first_rule, last_rule);

                int i = 0;
                while (split[i] != NULL) {
                    i++;
                }
                for (int j = 0; j < i; j ++) {
                    free(split[j]);
                }
                free(split);
            }
        }
    }
    if ((first_rule->target) != NULL) {
        return first_rule;
    }

    return NULL;
}

Rule *find_rule(Rule *first_rule, char *target) {
    Rule *temp = first_rule;
    while (temp!=NULL) {
        if (temp->target != NULL) {
            if (strcmp((temp->target), target) ==0) {
                break;
            }
        }
        temp = temp->next_rule;
    }
    return temp;
}

void add_action(Rule *node, char **array) {
    Action *curr_action;
    if (node->actions == NULL) {
        node->actions = malloc(sizeof(Action));
        curr_action = node->actions;
    } else {
        curr_action = node->actions;
        while ((curr_action->next_act) != NULL) {
            curr_action = curr_action->next_act;
        }
        curr_action->next_act = malloc(sizeof(Action));
        curr_action = curr_action->next_act;
    }
    curr_action->args = array;
}

Rule *add_new_rule(Rule *curr_rule, char **array, Rule *first_rule, Rule *last_rule) {
    if ((curr_rule->target) == NULL) {
        int j = strlen(array[0]) + 1;
        curr_rule->target = malloc(j);
        strncpy(curr_rule->target, array[0], j);
        (curr_rule->target)[j-1] = '\0';
    }

    curr_rule->dependencies = malloc(sizeof(Dependency));
    Dependency *curr_dep = curr_rule->dependencies;
    int i = 2;
    while (array[i] != NULL) {
        Rule *temp = find_rule(first_rule, array[i]);
        if (temp != NULL) {
            curr_dep->rule = temp;
        } else {
            curr_dep->rule = malloc(sizeof(Rule));
            int j = strlen(array[i]) + 1;
            (curr_dep->rule)->target = malloc(j);
            strncpy((curr_dep->rule)->target, array[i], j);
            ((curr_dep->rule)->target)[j-1] = '\0';
            last_rule->next_rule = curr_dep->rule;
            last_rule = last_rule->next_rule;
        }
        i++;
        if (array[i] != NULL) {
            curr_dep->next_dep = malloc(sizeof(Dependency));
        }
        curr_dep = curr_dep->next_dep;
    }
    if (i == 2) {
        free(curr_rule->dependencies);
        curr_rule->dependencies = NULL;
    }
    return last_rule;
}
