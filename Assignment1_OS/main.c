#define HAVE_STRUCT_TIMESPEC

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 4
#define TIME_QUANTUM 2

typedef struct {
	int thread_id;
	int arrival_time;
	int burst_time;
	int remaining_time;
} ThreadArgs;

int compare_arrival_time(const void* a, const void* b) {
	return ((ThreadArgs*)a)->arrival_time - ((ThreadArgs*)b)->arrival_time;
}

// FCFS funtion
void fcfs(ThreadArgs thread_args[], int n) {
	qsort(thread_args, n, sizeof(ThreadArgs), compare_arrival_time);
	
	int current_time = 0;
    float total_waiting_time = 0, total_turnaround_time = 0;
    printf("Process\tArrival Time\tBurst Time\tWaiting Time\tTurnaround Time\n");
    for (int i = 0; i < n; i++) {
        if (current_time < thread_args[i].arrival_time) {
            current_time = thread_args[i].arrival_time;
        }
        int waiting_time = current_time - thread_args[i].arrival_time;
        int turnaround_time = waiting_time + thread_args[i].burst_time;
        total_waiting_time += waiting_time;
        total_turnaround_time += turnaround_time;
        printf("%d\t%d\t\t%d\t\t%d\t\t%d\n", thread_args[i].thread_id, thread_args[i].arrival_time, thread_args[i].burst_time, waiting_time, turnaround_time);
        current_time += thread_args[i].burst_time;
    }
    printf("Average Waiting Time: %.2f\n", total_waiting_time / n);
    printf("Average Turnaround Time: %.2f\n", total_turnaround_time / n);
}

// SJF function
void sjf(ThreadArgs thread_args[], int n) {
    // Sorting threads based on arrival time
    qsort(thread_args, n, sizeof(ThreadArgs), compare_arrival_time);

    int current_time = 0;
    float total_waiting_time = 0, total_turnaround_time = 0;
    printf("Process\tArrival Time\tBurst Time\tWaiting Time\tTurnaround Time\n");
    for (int i = 0; i < n; i++) {
        if (current_time < thread_args[i].arrival_time) {
            current_time = thread_args[i].arrival_time;
        }
        int waiting_time = current_time - thread_args[i].arrival_time;
        int turnaround_time = waiting_time + thread_args[i].burst_time;
        total_waiting_time += waiting_time;
        total_turnaround_time += turnaround_time;
        printf("%d\t%d\t\t%d\t\t%d\t\t%d\n", thread_args[i].thread_id, thread_args[i].arrival_time, thread_args[i].burst_time, waiting_time, turnaround_time);
        current_time += thread_args[i].burst_time;
    }
    printf("Average Waiting Time: %.2f\n", total_waiting_time / n);
    printf("Average Turnaround Time: %.2f\n", total_turnaround_time / n);
}

// SRTF function
void srtf(ThreadArgs thread_args[], int n) {
    // Sorting threads based on arrival time
    qsort(thread_args, n, sizeof(ThreadArgs), compare_arrival_time);

    int current_time = 0, completed = 0;
    float total_waiting_time = 0, total_turnaround_time = 0;
    printf("Process\tArrival Time\tBurst Time\tWaiting Time\tTurnaround Time\n");
    while (completed < n) {
        int shortest_index = -1, shortest_burst = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (thread_args[i].remaining_time > 0 && thread_args[i].arrival_time <= current_time && thread_args[i].burst_time < shortest_burst) {
                shortest_index = i;
                shortest_burst = thread_args[i].burst_time;
            }
        }
        if (shortest_index == -1) {
            current_time++;
            continue;
        }
        printf("%d\t%d\t\t%d\t\t%d\t\t%d\n", thread_args[shortest_index].thread_id, thread_args[shortest_index].arrival_time, thread_args[shortest_index].burst_time, current_time - thread_args[shortest_index].arrival_time, current_time - thread_args[shortest_index].arrival_time + thread_args[shortest_index].burst_time);
        total_waiting_time += current_time - thread_args[shortest_index].arrival_time;
        total_turnaround_time += current_time - thread_args[shortest_index].arrival_time + thread_args[shortest_index].burst_time;
        current_time += thread_args[shortest_index].burst_time;
        thread_args[shortest_index].remaining_time = 0;
        completed++;
    }
    printf("Average Waiting Time: %.2f\n", total_waiting_time / n);
    printf("Average Turnaround Time: %.2f\n", total_turnaround_time / n);
}

// RR function
void rr(ThreadArgs thread_args[], int n) {
    // Sorting threads based on arrival time
    qsort(thread_args, n, sizeof(ThreadArgs), compare_arrival_time);

    int current_time = 0, completed = 0;
    float total_waiting_time = 0, total_turnaround_time = 0;
    printf("Process\tArrival Time\tBurst Time\tWaiting Time\tTurnaround Time\n");
    while (completed < n) {
        for (int i = 0; i < n; i++) {
            if (thread_args[i].remaining_time > 0 && thread_args[i].arrival_time <= current_time) {
                int execution_time = (thread_args[i].remaining_time >= TIME_QUANTUM) ? TIME_QUANTUM : thread_args[i].remaining_time;
                printf("%d\t%d\t\t%d\t\t%d\t\t%d\n", thread_args[i].thread_id, thread_args[i].arrival_time, thread_args[i].burst_time, current_time - thread_args[i].arrival_time, current_time - thread_args[i].arrival_time + execution_time);
                total_waiting_time += current_time - thread_args[i].arrival_time;
                total_turnaround_time += current_time - thread_args[i].arrival_time + execution_time;
                current_time += execution_time;
                thread_args[i].remaining_time -= execution_time;
                if (thread_args[i].remaining_time == 0) {
                    completed++;
                }
            }
        }
    }
    printf("Average Waiting Time: %.2f\n", total_waiting_time / n);
    printf("Average Turnaround Time: %.2f\n", total_turnaround_time / n);
}

int main() {
    // Thread parameters
    ThreadArgs thread_args[NUM_THREADS];

    // Getting arrival time and burst time from user input
    printf("Enter arrival time and burst time for each thread:\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        printf("Thread %d:\n", i + 1);
        printf("Arrival time: ");
        scanf_s("%d", &thread_args[i].arrival_time);
        printf("Burst time: ");
        scanf_s("%d", &thread_args[i].burst_time);
        thread_args[i].thread_id = i + 1;
        thread_args[i].remaining_time = thread_args[i].burst_time;
    }

    // Performing scheduling algorithms
    printf("\nFirst-Come, First-Served (FCFS) Scheduling:\n");
    fcfs(thread_args, NUM_THREADS);

    printf("\nShortest Job First (SJF) Scheduling:\n");
    sjf(thread_args, NUM_THREADS);

    printf("\nShortest Remaining Time First (SRTF) Scheduling:\n");
    srtf(thread_args, NUM_THREADS);

    printf("\nRound Robin (RR) Scheduling:\n");
    rr(thread_args, NUM_THREADS);

    return 0;
}

