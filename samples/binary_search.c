#include <stdio.h>
#include <stdlib.h>

#define SIZE 100000

int binary_search(int arr[], int n, int target) {
    int low = 0, high = n - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid] == target) return mid;
        if (arr[mid] < target) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

int main() {
    int *data = malloc(SIZE * sizeof(int));
    for (int i = 0; i < SIZE; i++) data[i] = i * 2;

    int hits = 0;
    // Perform millions of searches to trigger block caching
    for (int i = 0; i < 10000000; i++) {
        if (binary_search(data, SIZE, i % (SIZE * 2)) != -1) {
            hits++;
        }
    }

    printf("Search hits: %d\n", hits);
    return 0;
}
