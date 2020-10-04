
#define MAXLINE 256

/* A line from a makefile is either a target line, an action line, 
or a comment or line to ignore
     "target" line
        - Starts with a target, followed by a colon followed by a 
        space-separated list of dependencies
        - A target is a word that follows the Rule of valid file names  
        (assume that only valid file names are used in testing)
        - The colon has a space on either side of it
        - assume the line will not exceed MAXLINE
        - The list of dependencies follow the same Rule as the target

     "action" line
        - Begins with a tab
        - Contains a line that can be executed. First word is the relative path
        to the executable, remaining tokens are argument to the executable.
        - relative path means that if the executable is in the current working 
        directory, the path begins with "./" (Do not assume "." is in the path)
     
     Comment or empty line
        - contains only spaces and/or tabs
        - contains 0 or more spaces or tabs followed by a '#'.  Any other 
          characters after the '#' are ignored.
 */

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

/* Print the list of rules to stdout in makefile format */
void print_rules(Rule *rules);

/* Read from the open file fp, and creat the linked data structure
   that represents the Makefile contained in the file.
 */
Rule *parse_file(FILE *fp);

/* Return an array of strings where each string is one word in line. The final
   element of the array will contain NULL.
 */
char **build_args(char *line);

/* Return 1 if line contains only space characters, and 0 otherwise */
int is_comment_or_empty(char *line);

/* Concatenates args into a single string and store it in buffer, up to size.
   Return a pointer to buffer.
 */
char *args_to_string(char **args, char *buffer, int size);

/* Evaluate the rule in rules with the target "target"
   If target is NULL, evaluate the first rule in rules.
   If pflag is 1, then create a new process to evaluate each dependency. The
   parent process will wait until all child processes have terminated before
   checking dependecy modfied times to decide whether to execute the actions
 */
void run_make(char *target, Rule *rules, int pflag);

// helper function for parse_file: to find the rule with given target
// if there is no such a rule, return NULL.
Rule *find_rule(Rule *first_rule, char *target);

// helper function for parse_file: to add a new rule and return updated last_rule
// return last_rule for furthur use.
Rule *add_new_rule(Rule *curr_rule, char **array, Rule *first_rule, Rule *last_rule);

// helper function for parse_file: to add a new Action to the actions linked list.
void add_action(Rule *node, char **array);

// helper function similar to build_args, in order to build arrays for target line
char **build_target(char *line);

//helper function for run_make: to compare the last modified time between target and dependency
//return 1 if dep_time is newer than target_time, 0 otherwise.
int compare_time(struct timespec target_time, struct timespec dep_time);