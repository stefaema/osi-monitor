#include "metrics.h"

double get_memory_usage()
{
    FILE* fp;
    char buffer[MAX_LINE_LENGTH];
    unsigned long long total_mem = 0, free_mem = 0;

    // Se intenta abrir el archivo /proc/meminfo en modo lectura
    fp = fopen(MEMORYINFO_PATH, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error al abrir %s\n", MEMORYINFO_PATH);
        perror("");
        return -1.0;
    }

    // fgets() lee una línea del archivo y la almacena en buffer
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        // Si el contenido de buffer coincide con "MemTotal: %llu kB", se almacena el valor %llu en total_mem
        if (sscanf(buffer, "MemTotal: %llu kB", &total_mem) == 1)
        {
            continue; // MemTotal encontrado
        }
        // Si el contenido de buffer coincide con "MemAvailable: %llu kB", se almacena el valor %llu en free_mem
        if (sscanf(buffer, "MemAvailable: %llu kB", &free_mem) == 1)
        {
            break; // MemAvailable encontrado, podemos dejar de leer
        }
    }

    fclose(fp);

    // Verificar si se encontraron ambos valores
    if (total_mem == 0 || free_mem == 0)
    {
        fprintf(stderr, "Error al leer la información de memoria desde /proc/meminfo\n");
        return ERROR_D;
    }

    // Calcular el porcentaje de uso de memoria
    double used_mem = total_mem - free_mem;
    double mem_usage_percent = (used_mem / total_mem) * 100.0;

    return mem_usage_percent;
}

double get_cpu_usage()
{
    // Definimos variables estáticas para almacenar los valores de tiempo de CPU de la última lectura
    static CPUstats_t prev_cpu_stats = {0};
    // Definimos variables locales para almacenar los valores de tiempo de CPU de la lectura actual
    // unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    CPUstats_t current_cpu_stats;
    // Definimos variables para almacenar los valores de tiempo de CPU calculados
    unsigned long long totald, idled;
    // Definimos una variable para almacenar el porcentaje de uso de CPU
    double cpu_usage_percent;

    // Abrir el archivo /proc/stat
    FILE* fp = fopen(STAT_PATH, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error al abrir %s\n", STAT_PATH);
        perror("");
        return ERROR_D;
    }

    char buffer[MAX_LINE_LENGTH];
    // leer lineas del archivo (4*MAX_LINE_LENGTH) por linea
    if (fgets(buffer, sizeof(buffer), fp) == NULL)
    {
        fprintf(stderr, "Error al leer %s\n", STAT_PATH);
        perror("");
        fclose(fp);
        return ERROR_D;
    }
    fclose(fp);

    // Analizar los valores de tiempo de CPU
    int found =
        sscanf(buffer, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu", &current_cpu_stats.user, &current_cpu_stats.nice,
               &current_cpu_stats.system, &current_cpu_stats.idle, &current_cpu_stats.iowait, &current_cpu_stats.irq,
               &current_cpu_stats.softirq, &current_cpu_stats.steal);
    if (found < 8)
    {
        fprintf(stderr, "Error al parsear %s\n", STAT_PATH);
        // no hay errno, no se usa perror
        return ERROR_D;
    }

    // Calcular las diferencias entre las lecturas actuales y anteriores
    unsigned long long prev_idle_total = prev_cpu_stats.idle + prev_cpu_stats.iowait;
    unsigned long long idle_total = current_cpu_stats.idle + current_cpu_stats.iowait;

    unsigned long long prev_non_idle = prev_cpu_stats.user + prev_cpu_stats.nice + prev_cpu_stats.system +
                                       prev_cpu_stats.irq + prev_cpu_stats.softirq + prev_cpu_stats.steal;
    unsigned long long non_idle = current_cpu_stats.user + current_cpu_stats.nice + current_cpu_stats.system +
                                  current_cpu_stats.irq + current_cpu_stats.softirq + current_cpu_stats.steal;

    unsigned long long prev_total = prev_idle_total + prev_non_idle;
    unsigned long long total = idle_total + non_idle;

    totald = total - prev_total;
    idled = idle_total - prev_idle_total;

    if (totald == 0)
    {
        fprintf(stderr, "Totald es cero, no se puede calcular el uso de CPU!\n");
        return ERROR_D;
    }

    // Calcular el porcentaje de uso de CPU
    cpu_usage_percent = ((double)(totald - idled) / totald) * 100.0;

    // Actualizar los valores anteriores para la siguiente lectura
    prev_cpu_stats = current_cpu_stats;

    return cpu_usage_percent;
}

long long get_context_switches()
{

    FILE* fp;
    char buffer[MAX_LINE_LENGTH];
    unsigned long long context_switches = 0;
    int found;

    // Se intenta abrir el archivo /proc/stat en modo lectura
    fp = fopen(STAT_PATH, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error al abrir %s\n", STAT_PATH);
        perror("");
        return ERROR;
    }

    // fgets() lee una línea del archivo y la almacena en buffer

    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        found = sscanf(buffer, "ctxt %llu", &context_switches);
        // Si el contenido de buffer coincide con "ctxt %llu", se almacena el valor %llu en context_switches
        if (found)
            break; // ctxt encontrado, podemos dejar de leer
    }

    // Cerrar el archivo
    fclose(fp);
    // Verificar si se encontro el valor
    if (!found)
    {
        fprintf(stderr, "Error al leer la información de cambios de contexto desde %s\n", STAT_PATH);
        return ERROR;
    }

    return context_switches;
}

long get_processes_running()
{
    FILE* fp;
    char buffer[MAX_LINE_LENGTH];
    unsigned long processes_running = 0;
    int found;

    // Se intenta abrir el archivo /proc/stat en modo lectura
    fp = fopen(STAT_PATH, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error al abrir %s\n", STAT_PATH);
        perror("");
        return ERROR;
    }

    // fgets() lee una línea del archivo y la almacena en buffer
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        found = sscanf(buffer, "procs_running %lu", &processes_running);
        // Si el contenido de buffer coincide con "procs_running %lu", se almacena el valor %lu en processes_running
        if (found)
            break; // procs_running encontrado, podemos dejar de leer
    }

    fclose(fp);

    // Verificar si se enconto el valor
    if (processes_running == 0 || !found)
    {
        fprintf(stderr, "Error al leer la información de procesos en ejecución desde %s\n", STAT_PATH);
        return ERROR;
    }

    return processes_running;
}

long get_rx_multicast_packets()
{
    net_info_t ethernet_info = {0};
    net_info_t wireless_info = {0};
    int found = 0;
    FILE* fp;
    char buffer[MAX_LINE_LENGTH * 4];

    // Abrir el archivo /proc/net/dev en modo lectura
    fp = fopen(NETDEV_PATH, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error al abrir %s\n", NETDEV_PATH);
        perror("");
        return ERROR;
    }

    // Leer cada línea del archivo
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (strstr(buffer, ":"))
        {
            char interface_name[16];
            net_info_t pivot_info = {0};

            int ret = sscanf(buffer,
                "%15s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
                interface_name, &pivot_info.rx_bytes, &pivot_info.rx_packets, &pivot_info.rx_errors,
                &pivot_info.rx_dropped, &pivot_info.rx_fifo, &pivot_info.rx_frame, &pivot_info.rx_compressed,
                &pivot_info.rx_multicast, &pivot_info.tx_bytes, &pivot_info.tx_packets, &pivot_info.tx_errors,
                &pivot_info.tx_dropped, &pivot_info.tx_fifo, &pivot_info.tx_colls, &pivot_info.tx_carrier,
                &pivot_info.tx_compressed);

            if (ret == 17)
            {
                if (strstr(interface_name, "en") != NULL)
                {
                    found = 1;
                    ethernet_info.rx_multicast += pivot_info.rx_multicast;
                }
                else if (strstr(interface_name, "wl") != NULL)
                {
                    found = 1;
                    wireless_info.rx_multicast += pivot_info.rx_multicast;
                }
            }
        }
    }

    fclose(fp);
    if (!found)
    {
        fprintf(stderr, "Error al leer la información de interfaces desde %s\n", NETDEV_PATH);
        return ERROR;
    }
    return wireless_info.rx_multicast + ethernet_info.rx_multicast;
}


long get_lost_packets_info()
{
    net_info_t ethernet_info = {0};
    net_info_t wireless_info = {0};
    int found = 0;
    FILE* fp;
    char buffer[MAX_LINE_LENGTH * 4];

    // Abrir el archivo /proc/net/dev en modo lectura
    fp = fopen(NETDEV_PATH, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error al abrir %s\n", NETDEV_PATH);
        perror("");
        return ERROR;
    }

    // Leer cada línea del archivo
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (strstr(buffer, ":"))
        {
            char interface_name[16];
            net_info_t pivot_info = {0};

            int ret = sscanf(buffer,
                "%15s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
                interface_name, &pivot_info.rx_bytes, &pivot_info.rx_packets, &pivot_info.rx_errors,
                &pivot_info.rx_dropped, &pivot_info.rx_fifo, &pivot_info.rx_frame, &pivot_info.rx_compressed,
                &pivot_info.rx_multicast, &pivot_info.tx_bytes, &pivot_info.tx_packets, &pivot_info.tx_errors,
                &pivot_info.tx_dropped, &pivot_info.tx_fifo, &pivot_info.tx_colls, &pivot_info.tx_carrier,
                &pivot_info.tx_compressed);

            if (ret == 17)
            {
                if (strstr(interface_name, "en") != NULL)
                {
                    found = 1;
                    ethernet_info.rx_dropped += pivot_info.rx_dropped;
                    ethernet_info.rx_errors += pivot_info.rx_errors;
                }
                else if (strstr(interface_name, "wl") != NULL)
                {
                    found = 1;
                    wireless_info.rx_dropped += pivot_info.rx_dropped;
                    wireless_info.rx_errors += pivot_info.rx_errors;
                }
            }
        }
    }

    fclose(fp);
    if (!found)
    {
        fprintf(stderr, "Error al leer la información de interfaces desde %s\n", NETDEV_PATH);
        return ERROR;
    }
    return ethernet_info.rx_dropped + ethernet_info.rx_errors + wireless_info.rx_dropped + wireless_info.rx_errors;
}


double calculate_disk_health(const char* device_name)
{
    FILE* fp;
    char buffer[MAX_LINE_LENGTH];
    DiskStat_t disk_stat = {0};

    // Abrir el archivo /proc/diskstats en modo lectura
    fp = fopen(DISKSTATS_PATH, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error al abrir %s\n", DISKSTATS_PATH);
        perror("");
        return ERROR_D; // Retornar un valor de error
    }

    int found = 0;
    // Leer cada línea del archivo
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (strstr(buffer, device_name))
        {
            DiskStat_t temp_disk_stat = {0};
            int major, minor;
            // Parsear la línea para obtener las métricas relevantes
            int ret =
                sscanf(buffer, "%u %u %s %llu %llu %llu", &major, &minor, temp_disk_stat.device_name,
                       &temp_disk_stat.reads_completed, &temp_disk_stat.writes_completed, &temp_disk_stat.io_time);

            if (ret == 6)
            {
                found = 1;
                disk_stat = temp_disk_stat;
            }
            break; // Salir después de encontrar el dispositivo
        }
    }

    fclose(fp);

    if (!found)
    {
        fprintf(stderr, "Error al leer la información de disco desde /proc/diskstats\n");
        return ERROR_D; // Retornar un valor de error si no se encuentra el dispositivo
    }

    // Calcular la métrica de salud del disco (por ejemplo, un simple puntaje)
    unsigned long long total_operations = disk_stat.reads_completed + disk_stat.writes_completed;
    double health_score = 100.0; // Suponemos que la salud comienza en 100
    double read_write_ratio = (double)disk_stat.reads_completed / disk_stat.writes_completed;
    // Penalizar por errores
    if (total_operations > 0)
    {
        health_score -= (read_write_ratio * 2.0); // Penalizar por una relación de lectura/escritura alta
    }

    // Asegurar que la puntuación no sea menor que 0
    if (health_score < 0)
    {
        health_score = 0.0;
    }

    return health_score;
}
