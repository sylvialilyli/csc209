#include <stdio.h>
#include <stdlib.h>
#include<string.h>

void read_image(int num_rows, int num_cols, 
                int arr[num_rows][num_cols], FILE *fp);
                
void print_image(int num_rows, int num_cols, int arr[num_rows][num_cols]);

// Remember to include the function prototype for count_cells

int count_cells(int num_rows, int num_cols, int arr[num_rows][num_cols]);

int main(int argc, char **argv) {
    // Print a message to stderr and exit with an argument of 1 if there are
    // not the right number of parameters, or the second argument is not -p
    if ((argc < 2) | (argc > 3) |
		    ((argc == 3) && (strcmp(argv[2], "-p") != 0))) {
	    fprintf(stderr, "Usage: count_cells <imagefile.txt> [-p]");
	    exit(1);
    } 
    FILE *fp;
    fp = fopen(argv[1], "r");
    
    //print to a error message and exit with 1 if the input file directory is not valid
    if (!fp) {
        printf("There is no such file\n");
        exit(1);
    }

    int row, column;
    int counts;
    fscanf(fp, "%d %d", &row, &column);
    int a[row][column];
    read_image(row, column, a, fp);
    counts = count_cells(row, column, a);
    if (argc == 3) {
	    print_image(row, column, a);
    }
    printf("Number of Cells is %d\n", counts);
    return 0;
}
