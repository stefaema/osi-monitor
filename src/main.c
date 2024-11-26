/**
 * @file main.c
 * @brief Entry point of the system with configurable parameters using Linux-style arguments.
 */

#include "expose_metrics.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

/**
 * @brief Main function of the program.
 *
 * This function initializes metrics, creates a thread to expose metrics via HTTP,
 * and enters a loop to update metrics at a configurable interval.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return int Returns EXIT_SUCCESS on successful execution, otherwise EXIT_FAILURE.
 */
unsigned int sleep_time = 1; // Default sleep time
char disk_device[32] = "sda"; // Default disk device

void signal_handler(int signal) {
    if (signal == SIGUSR1) {
        printf("\n[MONITORING STATUS] courtesy of a SIGUSR1 signal\n");
        printf("CPU and memory metrics are being updated every %d seconds.\n", sleep_time);
        printf("Disk device being monitored: %s\n", disk_device); 
    }
}
int main(int argc, char *argv[]) {

    if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
        fprintf(stderr, "Failed to register signal handler for SIGUSR1.\n");
        return EXIT_FAILURE;
    }
    else {
        printf("\nSIGUSR1 setup OK.\n");
    }

    // Define long options
    struct option long_options[] = {
        {"sleep", required_argument, NULL, 's'},
        {"io_device", required_argument, NULL, 'i'},
        {0, 0, 0, 0} // Sentinel to mark the end of the array
    };

    int opt;
    bool parsing_error = false;

    while ((opt = getopt_long(argc, argv, "s:i", long_options, NULL)) != -1) {
        switch (opt) {
            case 's': // --sleep
                sleep_time = (unsigned int) atoi(optarg);
                if (sleep_time <= 0) {
                    fprintf(stderr, "Invalid sleep value: must be a positive integer. Using default value: 1 second.\n");
                    sleep_time = 1; // Use default value
                    parsing_error = true;
                }
                break;

            case 'i': // --io_device
                strncpy(disk_device, optarg, sizeof(disk_device) - 1);
                disk_device[sizeof(disk_device) - 1] = '\0'; // Ensure null-terminated string
                if (strlen(disk_device) == 0) {
                    fprintf(stderr, "Invalid device name. Using default device: sda.\n");
                    strncpy(disk_device, "sda", sizeof(disk_device));
                    parsing_error = true;
                }
                break;
            default:
                fprintf(stderr, "Invalid option. Using default values: sleep_time=1, io_device=sda.\n");
                sleep_time = 1;
                strncpy(disk_device, "sda", sizeof(disk_device));
                disk_device[sizeof(disk_device) - 1] = '\0'; // Ensure null-terminated string
                parsing_error = true;
                break;
        }
    }

    if (parsing_error) {
        fprintf(stderr, "Proceeding with default configuration: sleep_time=%d, io_device=%s.\n", sleep_time, disk_device);
    }

    // Prompt the user if required

    printf("Starting monitoring program with the following settings:\n");
    printf("  SLEEP_TIME: %d seconds\n", sleep_time);
    printf("  DISK_DEVICE: %s\n", disk_device);

    // Initialize metrics
    if (init_metrics() != EXIT_SUCCESS) {
        fprintf(stderr, "Failed to initialize metrics.\n");
        return EXIT_FAILURE;
    }

    // Create a thread to expose metrics via HTTP
    pthread_t tid;
    if (pthread_create(&tid, NULL, expose_metrics, NULL) != 0) {
        fprintf(stderr, "Error creating the HTTP server thread.\n");
        return EXIT_FAILURE;
    }

    printf("Monitoring program is ready. Scrape with Prometheus and visualize with Grafana.\n");

    // Main loop to update metrics at the specified interval
    while (true) {
        update_cpu_gauge();
        update_memory_gauge();
        update_context_switches_gauge();
        update_processes_running_gauge();
        update_rx_multicast_packets_gauge();
        update_lost_packets_gauge();
        update_disk_health_gauge(disk_device); // Use configurable disk device
        sleep(sleep_time);
    }

    destroy_mutex();
    return EXIT_SUCCESS;
}
