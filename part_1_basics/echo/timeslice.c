#ifdef __linux__

#include <sched.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    pid_t pid;
    struct timespec tp;

    pid = getpid();

    if(!sched_rr_get_interval(pid, &tp)) {
        printf("%ld s %ld ns", tp.tv_sec, tp.tv_nsec);
    }
}

#else

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    fprintf(stderr, "currently on written for linux :(\n");
    exit(1);
}

#endif
