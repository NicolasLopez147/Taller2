#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

// Los datos que tiene cada registro
struct Datos
{
    int idOrigen;
    int idDestino;
    int hora;
    float mediaViaje;
    float standard_deviation_travel_time;
    float geometric_mean_travel_time;
    float geometric_standard_deviation_travel_time;
    long sig;
};
// Estructura del archivo salidaIndex
struct index
{
    int idOrigen;
    long apuntador;
};

