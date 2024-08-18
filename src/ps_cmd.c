
#include <sys/sysctl.h>
#include <stdlib.h>
#include <stdio.h> // Add the necessary header file
#include <libproc.h> // Include the necessary header file
#include <libproc.h> // Include the necessary header file
#include "ps_cmd.h"

static void print_processes(FILE *fp, int nprocs, struct kinfo_proc *procs)
{
    fprintf(fp,"%-10s %-20s %-50s\n", "PID", "Name", "Path");
    for (int i = 0; i < nprocs; i++)
    {        
        char pathbuf[PROC_PIDPATHINFO_MAXSIZE];

        fprintf(fp, "%-10d %-20s ", procs[i].kp_proc.p_pid, procs[i].kp_proc.p_comm);

        if (proc_pidpath(procs[i].kp_proc.p_pid, pathbuf, sizeof(pathbuf)) > 0) {
            fprintf(fp,"%-50s\n", pathbuf);
        } else {
            fprintf(fp,"(Unavailable)\n");
        }
    }
}

void ps_cmd(FILE *fp)
{
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};
    size_t miblen = 4;

    struct kinfo_proc *procs = NULL;
    size_t size;

    // Get the size of the process information
    if (sysctl(mib, miblen, NULL, &size, NULL, 0) < 0)
    {
        perror("sysctl: size");
        exit(EXIT_FAILURE);
    }

    // Allocate memory to store the process information
    procs = malloc(size);
    if (procs == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Get the process information
    if (sysctl(mib, miblen, procs, &size, NULL, 0) < 0)
    {
        perror("sysctl: get procs");
        free(procs);
        exit(EXIT_FAILURE);
    }

    int nprocs = size / sizeof(struct kinfo_proc);
    print_processes(fp, nprocs, procs);

    free(procs);
}