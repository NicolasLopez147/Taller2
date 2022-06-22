#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h> 
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>


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

// Parametos de la funcion aceptarCliente
struct Parametros
{
    int *clientfd;
    int *serverfd;
    struct sockaddr_in *client;
    int tamano;
};