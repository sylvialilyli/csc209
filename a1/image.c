#include <stdio.h>
    
/* Reads the image from the open file fp into the two-dimensional array arr 
 * num_rows and num_cols specify the dimensions of arr
 */
void read_image(int num_rows, int num_cols, 
                int arr[num_rows][num_cols], FILE *fp) {
     for (int i = 0; i < num_rows; i++) {
	     for (int j = 0; j < num_cols; j++) {
		     fscanf(fp, "%d", &arr[i][j]);
	     }
     } 
}

/* Print to standard output the contents of the array arr */
void print_image(int num_rows, int num_cols, int arr[num_rows][num_cols]) {
	for (int i = 0; i < num_rows; i++) {
		for (int j = 0; j < num_cols; j++) {
			printf("%d ", arr[i][j]);
		}
		printf("\n");
	}

}

int count_cell(int row, int col, int num_rows, int num_cols, int arr[num_rows][num_cols]) {
	int count = 0;
	if ((row >= 0 && row < num_rows) && (col >= 0 && col < num_cols)) {
		if (arr[row][col] == 255) {
			arr[row][col] = 0;
			count_cell((row+1), col, num_rows, num_cols, arr);//down
			count_cell((row-1), col, num_rows, num_cols, arr);//up
			count_cell(row, (col+1), num_rows, num_cols, arr);//right
			count_cell(row, (col-1), num_rows, num_cols, arr);//left
			count = 1;
		}
	}
	return count;
}

/* TODO: Write the count_cells function */
int count_cells(int num_rows, int num_cols, int arr[num_rows][num_cols]) {
	int counts = 0;
	for (int i = 0; i < num_rows; i++) {
		for (int j = 0; j < num_cols; j++) {
		     counts += count_cell(i, j, num_rows, num_cols, arr);	
		}
	}
	return counts;
}
