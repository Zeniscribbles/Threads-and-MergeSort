
//////////////////////////////////////////////////////////////////////////////////////
///         University of Hawaii - College of Engineering
///                       Amanda Eckardt
///                     ECE 468 Fall 2024
///
///                   Threads and MergeSort
/// 
///                One for you, and one for you...
///
///@brief:  This is an implementation of merge sort assuming the
///         data is an array a[] of length N, and N is a power of 2.
///         The merging is done in a for-loop. Each
///         pass through the for-loop causes subarrays of the same size
///         to be merged together.  The mergings within a pass should 
///         be done in parallel using one thread per merge. However,
///         the main program must wait for all the threads to complete
///         before doing another pass of the for-loop.  Thus, mergings
///         of subarrays of size 1 can be done in parallel, then
///         mergings of subarrays of size 2 can be done in parallel and
///         so forth.
/// 
///         Use the pthread API
///
///@example Initial values:
///         19  62  1  28  15  26  61  56  11  54 
///         57  20  7  18  53  48  3  46  49  12 
///         63  10  45  40  59  38  41  4  55  2 
///         37  32 
///         *** Merging subarrays of size 1
///         *** Merging subarrays of size 2
///         *** Merging subarrays of size 4
///         *** Merging subarrays of size 8
///         *** Merging subarrays of size 16
///
///         Output:
///         1  2  3  4  7  10  11  12  15  18 
///         19  20  26  28  32  37  38  40  41  45 
///         46  48  49  53  54  55  56  57  59  61 
///         62  63 
///
///@see https://man7.org/linux/man-pages/man7/pthreads.7.html
///@see https://www.geeksforgeeks.org/merge-sort-using-multi-threading/
/////////////////////////////////////////////////////////////////////////////////////
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define N 32         // Number of elements in array
#define THREAD_MAX 8 // Number of threads

// Global variables -- be careful 
int a[N];     // Array to sort 
int temp[N];  // Temporary storage 

// Struct to hold parameters for merge function
typedef struct {
    int first;   // Starting index of the first subarray
    int midpt;   // Middle point dividing two subarrays
    int last;    // Last index of the second subarray
} merge_params;

void genvalues(int n);                      // Initializes array a[] 
void prnvalues(int n);                      // Prints array a[] 
void *merge_thread(void *arg);              // Thread wrapper for merge function
void merge(int first, int midpt, int last); // Merges subarrays 


/// @brief  The main program uses a function called merge() which
///         merges subarrays in sorted order.  Rewrite the program
///         so that it uses threads to do the merging of the
///         subarrays. In particular, each call to merge() creates
///         a thread.  
int main() {

int arrsize;     // Size of subarrays to merge 
int numarr;      // Number of subarrays 
int newarrsize;  // New subarray size 
int newnumarr;   // New number of subarrays 
int i;
int first;       // Parameters used to merge two subarrays 
int midpt;       //   The first subarray starts at "first" and ends 
int last;        //   at "first+midpt-1".  The second subarray starts 
                 //   at "first+midpt" and ends at "last-1" 

pthread_t threads[THREAD_MAX];  // Thread array to keep track of created threads
int thread_count = 0;           // Current thread count

// Initialize the array a[] with random values
genvalues(N);    // Initialize a[] with random values 
printf("Initial values:\n");
prnvalues(N);    // Display the values 

arrsize = 1;     // Start with subarrays of size 1 (individual elements)

while (arrsize < N) {
   printf("*** Merging subarrays of size %d\n",arrsize);
   arrsize = 2*arrsize;  // merge subarrays to double subarray size 
   
   thread_count = 0;     // Reset thread count for each pass of the loop
   for (i = 0; i < N; i += arrsize) {
        first = i;       // Define the first, midpt, and last indices for the subarrays to be merged
        midpt = first +(arrsize/2);
        if (first + arrsize < N){
            last = first + arrsize;
        }else{
            last = N;    // Handle the case where the subarray exceeds array bounds

        } 
        
        // Allocate memory to hold the parameters for the thread
        merge_params *params = malloc(sizeof(merge_params));
        params->first = first;
        params->midpt = midpt;
        params->last  = last;

        // Create a new thread to perform the merge operation
        pthread_create(&threads[thread_count], NULL, merge_thread, (void *)params);
        thread_count++;

         // If the maximum number of threads is reached, wait for them to finish
         if (thread_count == THREAD_MAX) {
            for (int t = 0; t < thread_count; t++) {
                pthread_join(threads[t], NULL);  // Wait for each thread to complete
            }
            thread_count = 0;  // Reset thread count for the next set of merges
        }

   }

   // Ensure any remaining threads finish before moving to the next pass of the loop
    for (int t = 0; t < thread_count; t++) {
        pthread_join(threads[t], NULL);  // Wait for the remaining threads
    }
}

    printf("\nOutput:\n");
    prnvalues(N); /* Display the sorted values */
    
    return EXIT_SUCCESS;

}

///@brief merge_thread: This is a wrapper function that allows the merge operation to be run in a thread.
///       The function takes a pointer to a merge_params struct, extracts the merge parameters,
///       and then calls the merge() function.
///@param void *arg
void *merge_thread(void *arg) {
   
    merge_params *params = (merge_params *)arg;        // Extract the parameters passed to the thread.
    merge(params->first, params->midpt, params->last); // Perform the merge operation using the extracted parameters
    free(arg);           // Free the memory allocated for the parameters
    pthread_exit(NULL);  // Exit the thread
}


/// @brief  merge: Merges subarrays (a[first], ..., a[midpt-1])
///         and (a[midpt],..., a[last-1]) into
///         subarray temp[first],..., temp[last-1] in increasing order.
///         Then it copies temp[first],..., temp[last-1] back into
///         a[first],..., a[last-1].
///
///@param first
///@param midpt
///@param last
void merge(int first, int midpt, int last)
{
    int leftptr;   // Pointers used in array a[ ] 
    int rightptr;
    int k;         /// pointer used in array temp[ ] 
    int delay;

    
    // Do not delete the next three lines. They cause the function to
    // delay by a amount that is proportional to the subarray it is merging
    if (last-first > 0){
        delay = last - first;
    } else{
        delay = 1;
    } 
    usleep(delay*250000);

    leftptr  = first; // Start at the beginning of the left array    
    rightptr = midpt; // Start at the beginning of the right subarray

    // Merge values in the two arrays of a[] into temp[] 
    for (k = first; k < last; k++) {
        if (leftptr >= midpt) {
            temp[k] = a[rightptr++];     // Left subarray exhausted, take from right

        } else if (rightptr >= last) {
            temp[k] = a[leftptr++];      // Right subarray exhausted, take from left

        } else if (a[leftptr] < a[rightptr]) {
            temp[k] = a[leftptr++];      // Take the smaller element from the left subarray

        } else {
            temp[k] = a[rightptr++];     // Take the smaller (or equal) element from the right subarray
        }
    }

    // Copy temp[] back to a[] 
    for (k = first; k < last; k++){
        a[k] = temp[k];
    }

}


///@brief genvalues: Initializes array a[] with random values.
/// 
///@param n
void genvalues(int n){
    int k;
    int i;
    int current; 

    k = 2*n;    // Using 2*n as the range for generating random values

    current = 0;
    for (i=0; i<n; i++) {
        current = (current*73 + 19)%k;
        a[i] = current;
    }
}

///@brief: prnvalues: Prints the values in the array a[]
///
///@param n
void prnvalues(int n){

    int i;
    for (i = 0; i < n; i++) {
        printf(" %d ", a[i]);
        if ((i + 1) % 10 == 0){ 
            printf("\n");
        }
    }

    printf("\n");

}

