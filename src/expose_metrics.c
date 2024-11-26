#include "expose_metrics.h"

/** Mutex para sincronización de hilos */
pthread_mutex_t lock;

/** Métrica de Prometheus para el uso de CPU */
static prom_gauge_t* cpu_usage_metric;

/** Métrica de Prometheus para el uso de memoria */
static prom_gauge_t* memory_usage_metric;

/* Métrica de Prometheus para la cantidad de cambios de contexto */
static prom_gauge_t* context_switches_metric;

/* Métrica de Prometheus para la cantidad de procesos en ejecución */
static prom_gauge_t* processes_running_metric;

/* Métrica de Prometheus para la cantidad de paquetes multicast recibidos */
static prom_gauge_t* rx_multicast_packets_metric;

/* Métrica de Prometheus para la cantidad de paquetes perdidos */
static prom_gauge_t* lost_packets_metric;

/* Métrica de Prometheus para la salud del disco */
static prom_gauge_t* disk_health_metric;

void update_cpu_gauge()
{
    double usage = get_cpu_usage();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(cpu_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de CPU\n");
    }
}

void update_memory_gauge()
{
    double usage = get_memory_usage();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(memory_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de memoria\n");
    }
}

void update_context_switches_gauge()
{
    long long switches = get_context_switches();
    if (switches >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(context_switches_metric, (double)switches, NULL); // Conversión explícita
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener la cantidad de cambios de contexto\n");
    }
}

void update_processes_running_gauge()
{
    long processes = get_processes_running();
    if (processes >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(processes_running_metric, (double)processes, NULL); // Conversión explícita
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener la cantidad de procesos en ejecución\n");
    }
}

void update_lost_packets_gauge()
{
    long packets = get_lost_packets_info();
    if (packets >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(lost_packets_metric, (double)packets, NULL); // Conversión explícita
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener la cantidad de paquetes perdidos\n");
    }
}

void update_rx_multicast_packets_gauge()
{
    long packets = get_rx_multicast_packets();
    if (packets >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(rx_multicast_packets_metric, (double)packets, NULL); // Conversión explícita
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener la cantidad de paquetes multicast recibidos\n");
    }
}

void update_disk_health_gauge()
{
    double health = calculate_disk_health("sda");
    if (health >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(disk_health_metric, health, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener la salud del disco\n");
    }
}

void* expose_metrics(void* arg)
{
    (void)arg; // Argumento no utilizado
    printf("Iniciando el manejador HTTP\n");
    // Aseguramos que el manejador HTTP esté adjunto al registro por defecto
    promhttp_set_active_collector_registry(NULL);
    printf("Iniciando el servidor HTTP en el puerto 8000\n");
    // Iniciamos el servidor HTTP en el puerto 8000
    struct MHD_Daemon* daemon = promhttp_start_daemon(MHD_USE_SELECT_INTERNALLY, 8000, NULL, NULL);
    if (daemon == NULL)
    {
        fprintf(stderr, "Error al iniciar el servidor HTTP\n");
        return NULL;
    }
    printf("Servidor HTTP iniciado correctamente\n");
    // Mantenemos el servidor en ejecución
    while (1)
    {
        sleep(1);
    }

    // Nunca debería llegar aquí
    MHD_stop_daemon(daemon);
    return NULL;
}

int init_metrics()
{
    printf("Inicializando mutex para las mestricas\n");
    // Inicializamos el mutex
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        fprintf(stderr, "Error al inicializar el mutex\n");
        return EXIT_FAILURE;
    }
    printf("Mutex inicializado correctamente\n");
    printf("Inicializando registro de coleccionistas\n");
    // Inicializamos el registro de coleccionistas de Prometheus
    if (prom_collector_registry_default_init() != 0)
    {
        fprintf(stderr, "Error al inicializar el registro de Prometheus\n");
        return EXIT_FAILURE;
    }
    printf("Registro de coleccionistas inicializado correctamente\n");

    // Creamos la métrica para el uso de CPU
    printf("Creando metrica de uso de CPU\n");
    cpu_usage_metric = prom_gauge_new("cpu_usage_percentage", "Porcentaje de uso de CPU", 0, NULL);
    if (cpu_usage_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de uso de CPU\n");
        return EXIT_FAILURE;
    }
    printf("Metrica de uso de CPU creada correctamente\n");
    printf("Creando metrica de uso de memoria\n");

    // Creamos la métrica para el uso de memoria
    memory_usage_metric = prom_gauge_new("memory_usage_percentage", "Porcentaje de uso de memoria", 0, NULL);
    if (memory_usage_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de uso de memoria\n");
        return EXIT_FAILURE;
    }
    printf("Metrica de uso de memoria creada correctamente\n");

    // Creamos la métrica de cambios de contexto
    printf("Creando metrica de cambios de contexto\n");
    context_switches_metric = prom_gauge_new("context_switches", "Cantidad de cambios de contexto", 0, NULL);
    if (context_switches_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de cambios de contexto\n");
        return EXIT_FAILURE;
    }
    printf("Metrica de cambios de contexto creada correctamente\n");

    // Creamos la métrica de procesos en ejecución
    printf("Creando metrica de procesos en ejecución\n");
    processes_running_metric = prom_gauge_new("processes_running", "Cantidad de procesos en ejecucion", 0, NULL);
    if (processes_running_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de procesos en ejecución\n");
        return EXIT_FAILURE;
    }
    printf("Metrica de procesos en ejecución creada correctamente\n");

    // Creamos la métrica de paquetes multicast recibidos
    printf("Creando metrica de paquetes multicast recibidos\n");
    rx_multicast_packets_metric =
        prom_gauge_new("rx_multicast_packets", "Cantidad de paquetes multicast recibidos", 0, NULL);
    if (rx_multicast_packets_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de paquetes multicast recibidos\n");
        return EXIT_FAILURE;
    }
    printf("Metrica de paquetes multicast recibidos creada correctamente\n");

    // Creamos la métrica de paquetes perdidos
    printf("Creando metrica de paquetes perdidos\n");
    lost_packets_metric = prom_gauge_new("lost_packets", "Cantidad de paquetes perdidos", 0, NULL);
    if (lost_packets_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de paquetes perdidos\n");
        return EXIT_FAILURE;
    }
    printf("Metrica de paquetes perdidos creada correctamente\n");

    // Creamos la métrica de salud del disco
    printf("Creando metrica de salud del disco\n");
    disk_health_metric = prom_gauge_new("disk_health_percentage", "Porcentaje de salud del disco", 0, NULL);
    if (disk_health_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de salud del disco\n");
        return EXIT_FAILURE;
    }
    printf("Metrica de salud del disco creada correctamente\n");

    // Registramos las métricas en el registro por defecto
    if (prom_collector_registry_must_register_metric(cpu_usage_metric) == NULL ||
        prom_collector_registry_must_register_metric(memory_usage_metric) == NULL ||
        prom_collector_registry_must_register_metric(context_switches_metric) == NULL ||
        prom_collector_registry_must_register_metric(processes_running_metric) == NULL ||
        prom_collector_registry_must_register_metric(rx_multicast_packets_metric) == NULL ||
        prom_collector_registry_must_register_metric(lost_packets_metric) == NULL ||
        prom_collector_registry_must_register_metric(disk_health_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar las métricas\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void destroy_mutex()
{
    pthread_mutex_destroy(&lock);
}
