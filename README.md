*This project has been created as part of the 42 curriculum by jamoons.*

# Codexion

## Description

**Codexion** is a multithreaded concurrency project written in C.

The program simulates a group of coders competing for shared hardware dongles. Each coder requires two adjacent dongles before it can compile. After compiling, the coder continues through debugging and refactoring before requesting the dongles again.

Every coder runs in its own POSIX thread. A separate monitor thread observes the simulation and detects when a coder exceeds its `time_to_burnout`.

The project focuses on thread synchronization, resource contention, scheduling, race-condition prevention, deadlock prevention, precise timing, and safe communication between concurrent threads.

Two scheduler modes are supported:

* `fifo` — First In, First Out. The earliest request has the highest priority.
* `edf` — Earliest Deadline First. The request with the nearest burnout deadline has the highest priority.

Requests are stored in a custom priority queue implemented as a binary heap.

## Instructions

### Compilation

The project uses a `Makefile` and is compiled with:

```bash
-Wall -Wextra -Werror -pthread
```

Available Makefile commands:

```bash
make
```

Compiles the project and creates the `codexion` executable.

```bash
make clean
```

Removes object files.

```bash
make fclean
```

Removes object files and the executable.

```bash
make re
```

Fully recompiles the project.

### Execution

Run the program using:

```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

Example:

```bash
./codexion 5 2000 200 200 200 10 0 fifo
```

The arguments are:

* `number_of_coders` — Number of coder threads and dongles.
* `time_to_burnout` — Maximum time in milliseconds a coder may go without starting a new compilation.
* `time_to_compile` — Time in milliseconds spent compiling.
* `time_to_debug` — Time in milliseconds spent debugging.
* `time_to_refactor` — Time in milliseconds spent refactoring.
* `number_of_compiles_required` — Number of compilations each coder must complete.
* `dongle_cooldown` — Time in milliseconds before a released dongle can be used again.
* `scheduler` — Scheduling policy. Must be exactly `fifo` or `edf`.

All numeric arguments except `dongle_cooldown` must be greater than `0`.

`dongle_cooldown` may be `0`.

Invalid numbers, negative values, values outside the supported integer range, malformed arguments, invalid scheduler names, and incorrect argument counts are rejected before the simulation starts.

## Scheduler

Codexion uses a custom binary heap to manage pending compilation requests.

Each request contains:

* the coder ID;
* the request arrival time;
* the coder's burnout deadline.

### FIFO

With the `fifo` scheduler, the request with the earliest arrival time receives the highest priority.

If two requests have the same arrival time, coder ID is used as a deterministic tie-breaker.

### EDF

With the `edf` scheduler, the request with the earliest burnout deadline receives the highest priority.

The deadline is calculated from the coder's most recent compilation start time and `time_to_burnout`.

If deadlines are equal, arrival time is used as the next comparison. Coder ID provides the final deterministic tie-breaker.

The scheduler also checks for resource conflicts so that a higher-priority request only blocks another coder when both requests require at least one of the same dongles.

## Resources

### References & Documentation

Resources used while developing the project include:

* **POSIX Threads Programming** — Lawrence Livermore National Laboratory:
  https://hpc-tutorials.llnl.gov/posix/
* **Linux Programmer's Manual**

  * `man pthreads`
  * `man pthread_create`
  * `man pthread_join`
  * `man pthread_mutex_lock`
  * `man pthread_mutex_unlock`
  * `man pthread_cond_wait`
  * `man pthread_cond_broadcast`
  * `man gettimeofday`
* **Modern Operating Systems** by Andrew S. Tanenbaum — concepts including mutual exclusion, race conditions, deadlocks, and concurrent resource management.

### AI Usage Disclosure

AI assistance, including ChatGPT and Gemini, was used during development for the following purposes:

* **Debugging and analysis:** helping analyze race conditions, lock ordering, thread traces, burnout detection, scheduling behavior, and concurrency-related bugs.
* **Code review:** reviewing synchronization logic and identifying possible deadlocks, data races, and unsafe shared-state access.
* **Norminette assistance:** helping restructure functions to remain within 42 Norminette limits while preserving program behavior.
* **Testing:** suggesting edge cases and commands for Helgrind, DRD, Valgrind, parser validation, cooldown behavior, and burnout scenarios.
* **Documentation:** assisting with the structure and wording of this README.

The implementation, integration, debugging decisions, testing, and understanding of the project remain part of the development process performed by the student.

## Thread Synchronization Mechanisms

Codexion uses POSIX mutexes and condition variables to coordinate access to shared resources and simulation state.

### Global State Mutex

The simulation contains a global `state_lock`.

It protects shared simulation and scheduler state, including operations involving:

* the simulation `running` flag;
* the request heap;
* coder compilation timing;
* compilation progress;
* scheduler decisions;
* changes that determine whether a coder may begin compiling.

Scheduler decisions are performed while `state_lock` is held so that checking resource availability and reserving resources cannot be interrupted by another coder making a conflicting scheduling decision.

### Dongle Mutexes

Every dongle contains its own `pthread_mutex_t`.

The dongle mutex protects the mutable state belonging to that specific dongle:

* `in_use`
* `available_at`

Before either of these fields is read or modified, the corresponding dongle mutex is acquired.

When two dongle mutexes are needed, Codexion determines a consistent order and always locks the lower-ordered resource first.

The general lock hierarchy is:

```text
state_lock
    |
    +--> first dongle mutex
            |
            +--> second dongle mutex
```

Using a consistent hierarchy prevents different threads from acquiring the same locks in conflicting orders.

The dongle mutexes are only held while inspecting or updating dongle state. They are not kept locked for the entire compilation duration.

Logical ownership during compilation is represented using the dongle's `in_use` field.

### Condition Variable

A global `pthread_cond_t` named `state_cond` is used together with `state_lock`.

Coder threads wait on this condition variable when they cannot currently compile.

Waiting with `pthread_cond_wait` releases `state_lock` while the thread sleeps and reacquires it before returning.

The condition variable is broadcast when simulation state changes, including situations such as:

* simulation startup;
* dongles becoming available;
* resource state changing;
* simulation termination.

This allows waiting threads to sleep instead of continuously busy-waiting for resources.

### Start Synchronization

Coder threads and the monitor thread wait until the simulation start time has been initialized.

The start time is set while `state_lock` is held, after which `state_cond` is broadcast to wake the waiting threads.

This provides a common reference point for timestamps and burnout calculations.

### Print Mutex

A global `print_lock` protects output.

Before printing a simulation event, the thread acquires `print_lock`.

This prevents multiple threads from writing parts of different log messages to `stdout` at the same time.

Typical output follows the format:

```text
<timestamp> <coder_id> <state>
```

For example:

```text
200 3 is compiling
```

### Monitor Thread

A dedicated monitor thread continuously checks whether any coder has exceeded `time_to_burnout`.

A coder's burnout timer is based on the beginning of its most recent compilation.

Access to this timing state is synchronized using `state_lock`.

If burnout is detected, the monitor:

1. changes the global simulation state to stopped;
2. wakes threads waiting on `state_cond`;
3. prints the burnout event using synchronized output.

The monitor checks frequently so burnout can be reported within the timing tolerance required by the project.

## Blocking Cases Handled

### Deadlock Prevention

Codexion uses deterministic lock ordering.

When two dongles must be inspected or modified, their mutexes are always acquired in the same order.

This prevents situations where two threads each hold one mutex while permanently waiting for the other.

The project also maintains a consistent hierarchy between the global simulation lock and individual dongle locks:

```text
state_lock -> dongle mutexes
```

The reverse ordering is avoided.

### Dongle Duplication Prevention

A dongle cannot be logically owned by two coders simultaneously.

The scheduler checks `in_use` while the dongle state is protected by its mutex.

When a coder is selected to compile, both required dongles are marked as in use before the global scheduler lock is released.

This makes the resource reservation atomic from the scheduler's point of view.

### One-Coder Case

When only one coder exists, its left and right dongles refer to the same physical dongle.

Because compilation requires two distinct dongles, that coder can never begin compiling.

The monitor eventually detects that the coder has exceeded `time_to_burnout` and stops the simulation.

### Dongle Cooldown

After compilation finishes, each released dongle receives an `available_at` timestamp:

```text
current_time + dongle_cooldown
```

Even though `in_use` becomes false, another coder cannot use that dongle until the cooldown timestamp has been reached.

A cooldown value of `0` makes the resource immediately reusable.

### Fair Scheduling and Starvation Handling

Coders do not simply race to acquire dongle mutexes.

Instead, each coder submits a request to the scheduler.

The scheduler determines priority using either FIFO or EDF and checks whether higher-priority conflicting requests exist before allowing a coder to compile.

This allows resource allocation to follow the selected scheduling policy instead of depending only on operating-system thread scheduling.

### Burnout Detection

The start of a compilation resets the coder's burnout reference time.

The monitor compares the current time against the coder's most recent compilation start time.

If the elapsed time exceeds `time_to_burnout`, the simulation is stopped and the coder's burnout is logged.

### Simulation Completion

Each coder tracks its completed compilation count.

The simulation ends successfully after every coder reaches `number_of_compiles_required`, unless a coder burns out first.

### Race-Condition Prevention

Shared mutable state is only accessed while protected by the synchronization primitive responsible for that state.

The implementation has been tested using:

```bash
valgrind --tool=helgrind ./codexion ...
```

and:

```bash
valgrind --tool=drd ./codexion ...
```

to detect thread synchronization errors.

### Memory Management

Dynamic allocations are released when the simulation ends or initialization fails.

Memory behavior has been checked using:

```bash
valgrind --leak-check=full --show-leak-kinds=all ./codexion ...
```

with all allocated heap blocks released before program termination.
