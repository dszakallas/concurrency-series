# Concurrency & Parallelism

TODO This is so far down I have to extract it and start completely elsewhere

So far blogposts mainly focused on new features of the JavaScript language,
Node.js APIs or 3rd party libraries to deal with asynchrony. No matter whether
we are using callbacks, streams, promises or the fancy syntactic features in
newer versions of JavaScript, namely ES2015 generators or ES2016 async
functions; we are doing the same basic thing: breaking our code into internally
synchronous chunks then ordering these chunks by their data dependencies to be
(potentially) asynchronously executed by the engine. Essentially, we are
creating a data flow.

### Threads

*Thread* is the default unit of CPU usage, code executed in a single thread is
what we usually refer to as *sequential* or *synchronous* execution.

Threads are supported by nearly all operating systems (hence the multithreaded
qualifier) and can be created with system calls. They have their own call
stacks, virtual CPU and (often) local storage but share the application's heap,
data, codebase and resources (such as file handles) with the other threads in
the same process. They also serve as the unit of scheduling in the kernel. For
this reason, we call them *kernel threads*, clarifying that they are native to
the operating system and scheduled by the kernel, which distinguishes them from
user-space threads, also called *green threads*, which are scheduled by some
user space scheduler such as a library or VM.

![Process and threads](assets/kernel_proc_thread.png)

Most operating system kernels use a [preemptive scheduler](preemption), as does
the Linux, macOS and Windows kernel. Consequently, when talking about (kernel)
threads in this article, we will assume that they are preemptively scheduled,
which distinguishes them from their non-preemptive (cooperative) counterparts,
called *fibers*.

### CPU, I/O and busy-waiting

Programs usually don't only consist of numeric, arithmetic and logic
computations, in fact a lot of times all they do is merely write something to
the file system, issue HTTP requests or access peripheries such as the console
or an external device. While the first kind of workload is CPU intensive, the
latter requires performing I/O in the majority of the time.

| CPU bound                   | I/O bound                                     |
|-----------------------------|-----------------------------------------------|
| scientific computation      | reading from / writing to disk                |
| (in-memory) data analysis   | accessing camera, microphone, other devices   |
| simulations                 | reading from / writing to network sockets     |
|                             | reading from stdin                            |

Doing I/O is a kernel space operation, initiated with a system call by the
program, so it results in a privilege context switch. When an I/O operation is
requested with a blocking system call, we call it *synchronous I/O* or *blocking
I/O*. Only when the system call has returned can the user code execution
continue, hence the term "blocking".

Modern operating systems map a kernel thread to each user-space thread, allowing
another thread to run when a thread makes a blocking system call. This is called
one-to-one mapping. This model improves concurrency over many-to-one mapping,
where all threads in a process share a common kernel thread, thus every thread
has to wait when one does blocking I/O.

Busy-waiting is the act of repeatedly checking

It is important to separate in our minds the term busy-wait and blocked by I/O. Busy-waiting

[preemption]: https://en.wikipedia.org/wiki/Preemption_(computing)

### Threads

The traditional, thread-based concurrency model focuses on the use of threads to introduce concurrency. These can be either kernel threads or green-threads. Long-running independent computations can be run in separate threads to maximize
utilization of the processors and reduce overall execution time. In desktop GUI applications, *worker threads* are often created to execute CPU or I/O heavy background tasks without tasks.

#### Example

In this example workflow, Task1 creates a single result consumed by Task2, Task3
and Task4. The output of all of these is simultaneously needed for Task5 to start.

![Data dependencies for the example](assets/ex1_dd.png)

In what order should we execute these tasks on one thread?
Any of its dependency graph's [topological orders](topo-sort) is correct.


![Single threaded schedule](assets/ex1_st.png)

![ALternative Single threaded schedule](assets/ex1_st_alt.png)

This means the instead of running 2, 3 and 4 after each other, we can run them in
parallel.

![A multithreaded schedule](assets/ex1_mt.png)

(Insert Java implementation here)

[topo-sort]: https://en.wikipedia.org/wiki/Topological_sorting

#### (Elonyei)

- fast communication with shared memory
- helps gain advantage of multi-processor architecture

#### Drawbacks

The use of shared memory while being the fastest communication channel between different threads, makes the application susceptible to data races, which is usually mitigated by means of locking, see mutexes, semaphores, however with the use of smart data structures this can be minimized.

We have long history of thread-based web servers. These web servers allocate a thread for each incoming web request. This model seems natural and very easy to
understand, however offers some drawbacks.
scheduling overhead, memory consumption.

### What is I/O?

Input/output, abbreviated as I/O, is the act of talking to a device, file system, network.

We separate computation and waiting for I/O, because the second doesn't require the processor to do anything.
In the thread-based model if a thread is waiting for an I/O interrupt we say it is blocked. This is what we usually refer to as synchronous I/O operation. While being blocked by an I/O call the thread can't proceed until it completes. However no busy loop, OS-es schedule other threads.

Sleep, I/O interrupt thread gets blocked

### Node.js specific model

The authors of Node.js realized that web

Reactor

Uses threadpool to manage background I/O tasks for network and file system operations.
Application code controlled by the developer is a single threaded proactor.

#### Pros

Web, keep-alive connections, I/O heavy workloads. no context switching

#### Drawbacks

Not general purpose: biased again I/O heavy workloads. Cooperative multitasking Devastating consequences if misused,

Unintuitive and not maintainable for computation heavy workloads

### Actor model

erlang/ elixir
akka
orleans
caf
celluloid
pulsar

### Concurrent sequential processes

> It is hard to do control structures that depend on libraries. Rob Pike

green threads https://research.swtch.com/dogma






Without further abstract nonsense, let's dive into illustration right away. I will
use the Node.js filesystem API functions throughout this post as I assume you are
already familiar with them, and they provide both async and sync interfaces making
it easier to draw up the basic concepts in a concise manner.

### Ex. 1

*Write a function


As we will see, the nature of writing asynchronous code is two-fold. F
