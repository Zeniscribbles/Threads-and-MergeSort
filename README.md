# Threads-and-MergeSort: Parallel Merge Sort using Pthreads

## Objective
This student assignment involves rewriting a traditional merge sort program, `merge468.c`, to utilize threads for parallel merging using the **Pthreads API**. The numbers to be sorted are stored in an array `a[]` of size `N`, where `N` is assumed to be a power of 2.

The goal is to perform the merging of subarrays in parallel using threads. For example, if `N = 16`, the program will follow these steps:

## Steps

1. **Initial Step**: Create 8 threads where each thread merges two elements into a subarray of size 2. These 8 threads can run in parallel.
2. **Wait**: Wait until all the threads are finished.
3. **Next Step**: Create 4 threads where each thread merges two subarrays of size 2 into a subarray of size 4. These 4 threads can run in parallel.
4. **Wait**: Wait until all the threads are finished.
5. **Next Step**: Create 2 threads where each thread merges two subarrays of size 4 into a subarray of size 8. These 2 threads can run in parallel.
6. **Wait**: Wait until all the threads are finished.
7. **Final Step**: Create 1 thread to merge two subarrays of size 8 into a sorted array of size 16.

```
        EXAMPLE:
         Initial values:
         19  62  1  28  15  26  61  56  11  54 
         57  20  7  18  53  48  3  46  49  12 
         63  10  45  40  59  38  41  4  55  2 
         37  32

         *** Merging subarrays of size 1
         *** Merging subarrays of size 2
         *** Merging subarrays of size 4
         *** Merging subarrays of size 8
         *** Merging subarrays of size 16

         Output:
         1  2  3  4  7  10  11  12  15  18 
         19  20  26  28  32  37  38  40  41  45 
         46  48  49  53  54  55  56  57  59  61
```

# More Information on Threads, Multithreading, and Pthreads API

## 1. **Thread Creation with Pthreads**
   - Use `pthread_create` to create new threads.
   - Always pass `NULL` if you're not using custom thread attributes.
   - Example:
     ```c
     pthread_t thread;
     pthread_create(&thread, NULL, function_name, (void *)arg);
     ```

## 2. **Thread Synchronization**
   - **Mutexes**:
     - To avoid race conditions, use `pthread_mutex_t` for mutual exclusion (mutex).
     - Example usage:
       ```c
       pthread_mutex_t lock;
       pthread_mutex_init(&lock, NULL);
       pthread_mutex_lock(&lock);
       // Critical section
       pthread_mutex_unlock(&lock);
       pthread_mutex_destroy(&lock); // Clean up
       ```

   - **Condition Variables**:
     - Used for signaling between threads, often in combination with a mutex.
     - Example:
       ```c
       pthread_cond_t cond;
       pthread_cond_init(&cond, NULL);
       pthread_cond_wait(&cond, &mutex);  // Wait for a condition
       pthread_cond_signal(&cond);        // Signal a waiting thread
       pthread_cond_destroy(&cond);       // Clean up
       ```

## 3. **Avoid Deadlocks**
   - Always acquire locks in the same order across all threads.
   - If a thread needs multiple locks, acquire them all at once, or carefully plan the locking strategy to avoid circular waits.

## 4. **Thread Join and Detach**
   - **Joinable Threads**: Use `pthread_join` to wait for a thread to finish. This ensures the main thread doesn't continue execution until the other thread completes.
     ```c
     pthread_join(thread, NULL);
     ```
   - **Detached Threads**: Detached threads run independently. Use `pthread_detach` if you don't need to join.
     ```c
     pthread_detach(thread);
     ```

## 5. **Passing Arguments to Threads**
   - Since `pthread_create` only accepts a `void *` argument, wrap multiple parameters in a `struct` and pass the pointer.
   - Example:
     ```c
     typedef struct {
         int param1;
         float param2;
     } thread_args;

     thread_args args = {42, 3.14};
     pthread_create(&thread, NULL, thread_function, (void *)&args);
     ```

## 6. **Thread Safety**
   - Global and shared data should be protected with mutexes to ensure that multiple threads don’t modify them simultaneously.
   - Always initialize shared resources before creating threads.

## 7. **Thread Termination**
   - Threads can terminate in the following ways:
     - Returning from the thread function.
     - Calling `pthread_exit()`.
     - The main thread can cancel a thread using `pthread_cancel()`.

## 8. **Use Thread Pooling for Efficiency**
   - Instead of creating and destroying threads frequently, use a thread pool, where a fixed number of threads are reused for tasks. This avoids the overhead of constantly creating threads.

## 9. **Beware of Stack Size**
   - By default, each thread has its own stack. Be mindful of large stack allocations in each thread, as they can lead to memory exhaustion.
   - You can control the stack size with `pthread_attr_setstacksize`.

## 10. **Minimize Context Switching**
   - The more threads you create, the more the operating system needs to switch between them. Keep the number of threads reasonable to avoid excessive context switching, which can reduce performance.

## 11. **Thread Safety with I/O**
   - Standard I/O functions like `printf` are generally thread-safe, but it’s best to avoid shared I/O (like `stdout`) across multiple threads to avoid jumbled output.
   - Protect shared resources with mutexes when performing I/O operations from multiple threads.

## 12. **Use Memory Barriers for Data Synchronization**
   - When multiple threads access shared data, even with mutexes, memory barriers (`pthread_barrier_t`) can ensure threads only proceed after all threads reach a certain point.
   - Example:
     ```c
     pthread_barrier_t barrier;
     pthread_barrier_init(&barrier, NULL, NUM_THREADS);
     pthread_barrier_wait(&barrier);
     ```

## 13. **Debugging Multithreaded Programs**
   - **Race conditions** and **deadlocks** can be hard to debug. Tools like `Helgrind` (part of Valgrind) can help detect threading issues.
   - Use logging and conditional breakpoints to trace thread execution.

## 14. **Thread Performance Tuning**
   - Performance scales with the number of available cores, but too many threads can create overhead.
   - Benchmark and profile your application to find the optimal number of threads.
   - On modern systems, a good rule of thumb is to keep the number of threads close to the number of CPU cores.

## 15. **Always Free Resources**
   - Free dynamically allocated memory used for thread arguments.
   - Destroy mutexes, condition variables, and other thread-related resources when they’re no longer needed to avoid memory leaks.

## 16. **Compiling with Pthreads**
   - When compiling a program that uses pthreads, you need to link with the pthread library.
   - Use the `-pthread` flag:
     ```bash
     gcc -pthread -o my_program my_program.c
     ```

