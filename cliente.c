#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "datos.h"

// Se define el puerto y el tamaño maximo de la cola de conecciones
#define PORT 3536
#define IP "127.0.0.1"

// Limita los posibles valores que puede tomar los id de origen y de llegada
int idLugar(int *id)
{
    while (*id < 1 || *id > 1160)
    {
        printf("El id ingresado no es valido; debe ser un valor entre 1 y 1160. Ingrese nuevamente el valor\n");
        scanf("%d", id);
    }
}

// Limita la hora ingresada
int formatoHora(int *hora)
{
    while (*hora < 0 || *hora > 23)
    {
        printf("La hora ingresada no es valida; debe ser un valor entre 0 y 23. Ingrese nuevamente el valor\n");
        scanf("%d", hora);
    }
}

int main()
{

    char a;
    // Tamaño calcula el tamaño de los datos
    int tamano = sizeof(struct Datos);

    // Descriptor del socket cliente
    int clientefd, r;
    // EStructuras de configuracion de cliente
    struct sockaddr_in client;

    // Se crea el socket cliente
    clientefd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientefd < 0)
    {
        perror("Error creando el socket\n");
        exit(-1);
    }

    // Se configura el cliente
    client.sin_family = AF_INET;
    client.sin_port = htons(PORT);
    inet_aton(IP, &client.sin_addr);

    // Espera a que el servidor este listo para poder conectarse
    do
    {
        r = connect(clientefd, (struct sockaddr *)&client, (socklen_t)sizeof(struct sockaddr));
    } while (r < 0);

    // Envia una confirmacion de la coneccion con el servidor
    char confirmacion[3];
    r = recv(clientefd, confirmacion, 2, 0);
    confirmacion[3] = 0;

    printf("Mensaje coneccion con el servidor %s\n", confirmacion);
    if (confirmacion[0] == 'N')
    {
        exit(-1);
    }
    printf("Oprima cualquier tecla para continuar\n");
    scanf("%s", &a);

    int cantidad = 0, opc = 0;

    do
    {
        // Crea una estructura que guardara los datos
        struct Datos datos;
        struct Datos *buffer = &datos;

        printf("Bienvenido\n\n");
        printf("1. Ingresar origen\n");
        printf("2. Ingresar destino\n");
        printf("3. Ingresar hora\n");
        printf("4. Buscar tiempo de viaje medio\n");
        printf("5. Salir\n");
        scanf("%d", &opc);
        int origen;
        int destino;
        int hora;
        switch (opc)
        {
        case 1:
            printf("Ingrese el ID del origen ");
            scanf("%d", &origen);
            idLugar(&origen); // Revision de valores
            printf("\nEl id ingresado fue %d\n", origen);
            datos.idOrigen = origen;
            break;

        case 2:
            printf("Ingrese el ID del destino ");
            scanf("%d", &destino);
            idLugar(&destino); // Revision de valores
            printf("\nEl id ingresado fue %d\n", destino);
            datos.idDestino = destino;
            break;

        case 3:
            printf("Ingrese hora del dia ");
            scanf("%d", &hora);
            formatoHora(&hora); // Revision de valores
            printf("\nLa hora ingresada fue %d\n", hora);
            datos.hora = hora;
            break;

        case 4:

            // Se envian todos los datos para hacer la consulta
            while (cantidad < tamano)
            {

                r = send(clientefd, buffer + cantidad, tamano, 0);
                cantidad = cantidad + r;
            }
            cantidad = 0;
            if (r < 0)
            {
                perror("Error en send\n");
                exit(-1);
            }
            // printf("Bytes enviados %d iteracion %d\n",cantidad,1);
            // Se reciben los datos del servidor
            while (cantidad < tamano)
            {
                r = recv(clientefd, buffer + cantidad, tamano, 0);
                cantidad = cantidad + r;
            }
            // Se conprueba la consulta
            if (cantidad == tamano)
            {
                printf("Resultado consulta recibido correctamente\n");
                printf("Oprima cualquier tecla para continuar\n");
                scanf("%s", &a);
            }
            else
            {
                printf("La consulta no se recibio correctamente\n");
                printf("Oprima cualquier tecla para continuar\n");
                scanf("%s", &a);
                exit(-1);
            }

            cantidad = 0;
            if (r < 0)
            {
                perror("Error en recv");
                exit(-1);
            }

            if (datos.idOrigen == -1)
            {
                // printf("No hay registros con los parametros indicados\n");
                printf("NA\n");
                break;
            }
            printf("Se encontro el registro %d %d %d ", datos.idOrigen, datos.idDestino, datos.hora);
            printf("con un tiempo de viaje medio de %f\n", datos.mediaViaje);
            break;

        case 5:
            printf("Adios\n");
            // Se cierra el socket
            close(clientefd);
            break;

        default:
            printf("Opcion incorrecta\n");
            break;
        }

    } while (opc != 5);
}