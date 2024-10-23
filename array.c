#define N 4

void transpose(int arr[N][N], int size) {
    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            // Swap arr[i][j] and arr[j][i]
            int temp = arr[i][j];
            arr[i][j] = arr[j][i];
            arr[j][i] = temp;
        }
    }
}

// Function to reverse the rows of the transposed matrix
void reverseRows(int arr[N][N], int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0, k = size - 1; j < k; j++, k--) {
            // Swap arr[i][j] and arr[i][k]
            int temp = arr[i][j];
            arr[i][j] = arr[i][k];
            arr[i][k] = temp;
        }
    }
}
