# Thread-Synchronization
`wait_for_all_threads()` synchronizes pthreads to begin almost simultaneously in a multi-threaded environment, free from race condition(as far as I have tested). The difference of threads starting time is the execution time from [`Line 35`](https://github.com/stamilselvan/Thread-Synchronization/blob/master/main.c#L35) to [`Line 46`](https://github.com/stamilselvan/Thread-Synchronization/blob/master/main.c#L46) in each thread. 

`set_core_affinity()` attaches a thread to a defined CPU.

###Sample Output
Console output of the ARM Cortex A9 quad core processor is given below.
```
Thread:0 running on core:0 
Thread:1 running on core:1 
Thread:2 running on core:2 
Thread:3 running on core:3 
Thread:3  Sync1 time: Thu Nov 26 16:57:49 2015
Thread:0  Sync1 time: Thu Nov 26 16:57:49 2015
Thread:1  Sync1 time: Thu Nov 26 16:57:49 2015
Thread:2  Sync1 time: Thu Nov 26 16:57:49 2015
Thread:3  Sync2 time: Thu Nov 26 16:57:58 2015
Thread:0  Sync2 time: Thu Nov 26 16:57:58 2015
Thread:1  Sync2 time: Thu Nov 26 16:57:58 2015
Thread:2  Sync2 time: Thu Nov 26 16:57:58 2015
Threads over 
```
