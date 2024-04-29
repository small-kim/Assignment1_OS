#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <float.h>

#define MAX_THREADS 4
#define TIME_SLICE 2.0f

typedef struct {
    int thread_id;
    float arrival_time;
    float execution_time;
    float remaining_time;
} Thread;

HANDLE thread_handles[MAX_THREADS];
DWORD thread_ids[MAX_THREADS];

DWORD WINAPI FCFS(Thread* threads) {
    float total_time = 0.0f;
    float waiting_time = 0.0f;

    for (int i = 0; i < MAX_THREADS; i++) {
        if (total_time < threads[i].arrival_time) {
            total_time = threads[i].arrival_time;
        }

        waiting_time += total_time - threads[i].arrival_time;
        total_time += threads[i].execution_time;
    }

    float avg_waiting_time = waiting_time / MAX_THREADS;
    printf("Total time for FCFS: %.2f\n", total_time);
    printf("Average waiting time for FCFS: %.2f\n", avg_waiting_time);

    return 0;
}

DWORD WINAPI SJF(Thread* threads) {
    float total_time = 0.0f;
    float waiting_time = 0.0f;
    int executed[MAX_THREADS] = { 0 };

    for (int i = 0; i < MAX_THREADS; i++) {
        int shortest_thread = -1;
        float shortest_time = FLT_MAX;

        for (int j = 0; j < MAX_THREADS; j++) {
            if (!executed[j] && threads[j].arrival_time <= total_time && threads[j].execution_time < shortest_time) {
                shortest_thread = j;
                shortest_time = threads[j].execution_time;
            }
        }

        if (shortest_thread != -1) {
            executed[shortest_thread] = 1;
            total_time += threads[shortest_thread].execution_time;
            waiting_time += total_time - threads[shortest_thread].arrival_time - threads[shortest_thread].execution_time;
        }
    }

    float avg_waiting_time = waiting_time / MAX_THREADS;
    printf("Total time for SJF: %.2f\n", total_time);
    printf("Average waiting time for SJF: %.2f\n", avg_waiting_time);

    return 0;
}

DWORD WINAPI SRTF(Thread* threads) {
    float total_time = 0.0f;
    float waiting_time = 0.0f;
    int completed = 0;

    while (completed < MAX_THREADS) {
        float shortest_remaining_time = FLT_MAX;
        int shortest_index = -1;

        for (int i = 0; i < MAX_THREADS; i++) {
            if (threads[i].remaining_time > 0.0f && threads[i].arrival_time <= total_time && threads[i].remaining_time < shortest_remaining_time) {
                shortest_remaining_time = threads[i].remaining_time;
                shortest_index = i;
            }
        }

        if (shortest_index != -1) {
            total_time++;
            threads[shortest_index].remaining_time--;

            if (threads[shortest_index].remaining_time == 0.0f) {
                completed++;
                waiting_time += total_time - threads[shortest_index].arrival_time - threads[shortest_index].execution_time;
            }
        }
        else {
            total_time++;
        }
    }

    float avg_waiting_time = waiting_time / MAX_THREADS;
    printf("Total time for SRTF: %.2f\n", total_time);
    printf("Average waiting time for SRTF: %.2f\n", avg_waiting_time);

    return 0;
}

DWORD WINAPI RR(Thread* threads) {
    float total_time = 0.0f;
    float waiting_time = 0.0f;
    int current_thread = 0;
    int completed_threads = 0;

    while (completed_threads < MAX_THREADS) {
        if (threads[current_thread].remaining_time > 0.0f) {
            if (threads[current_thread].remaining_time > TIME_SLICE) {
                total_time += TIME_SLICE;
                threads[current_thread].remaining_time -= TIME_SLICE;
            }
            else {
                total_time += threads[current_thread].remaining_time;
                waiting_time += total_time - threads[current_thread].arrival_time - threads[current_thread].execution_time;
                threads[current_thread].remaining_time = 0.0f;
                completed_threads++;
            }
        }

        current_thread = (current_thread + 1) % MAX_THREADS;
    }

    float avg_waiting_time = waiting_time / MAX_THREADS;
    printf("Total time for RR: %.2f\n", total_time);
    printf("Average waiting time for RR: %.2f\n", avg_waiting_time);

    return 0;
}

int main() {
    Thread threads[MAX_THREADS];

    printf("Enter arrival and execution times for each thread:\n");
    for (int i = 0; i < MAX_THREADS; i++) {
        printf("Thread %d:\n", i + 1);
        printf("Arrival time: ");
        scanf_s("%f", &threads[i].arrival_time);
        printf("Execution time: ");
        scanf_s("%f", &threads[i].execution_time);
        threads[i].thread_id = i + 1;
        threads[i].remaining_time = threads[i].execution_time;
    }

    printf("\nFCFS\n");
    thread_handles[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)FCFS, (LPVOID)threads, 0, &thread_ids[0]);
    WaitForSingleObject(thread_handles[0], INFINITE);

    printf("\nSJF\n");
    thread_handles[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SJF, (LPVOID)threads, 0, &thread_ids[1]);
    WaitForSingleObject(thread_handles[1], INFINITE);

    printf("\nSRTF\n");
    thread_handles[2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SRTF, (LPVOID)threads, 0, &thread_ids[2]);
    WaitForSingleObject(thread_handles[2], INFINITE);

    printf("\nRR\n");
    thread_handles[3] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RR, (LPVOID)threads, 0, &thread_ids[3]);
    Sleep(1000);
    WaitForSingleObject(thread_handles[3], INFINITE);

    for (int i = 0; i < MAX_THREADS; i++) {
        CloseHandle(thread_handles[i]);
    }

    return 0;
}
