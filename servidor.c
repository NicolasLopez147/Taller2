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

#include <pthread.h>

#include "datos.h"
// Se define el puerto y el tamaño maximo de la cola de conecciones
#define PORT 3536
#define BACKLOG 32


void configuracionServidor(int *clientfd, int* serverfd,struct sockaddr_in *serverP, struct sockaddr_in *client){
    // Parametro de setsockopt
    int r ,opt = 1;

    struct sockaddr_in server;
    serverP = &server;
    

    // Se crea el socket servidor
    *serverfd = socket(AF_INET,SOCK_STREAM,0);
    if (*serverfd < 0){
        perror("Error en socket");
        exit(-1);
    }
    // Se configura el servidor
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(server.sin_zero,8);

    // Reutiliza los recursos abiertos en ejecuciones pasadas
    setsockopt(*serverfd,SOL_SOCKET,SO_REUSEADDR,(const char *)&opt,sizeof(int));

    // Se configura el servidor
    r = bind(*serverfd,(struct sockaddr *)&server,sizeof(struct sockaddr));
    if (r < 0 ){
        perror("Error en bind");
        exit(-1);
    }
    // Se configura como servidor
    r = listen(*serverfd,BACKLOG);
    if (r < 0 ){
        perror("Error en LISTEN");
        exit(-1);
    }
    
}

void aceptarCliente(int *clientfd, int* serverfd , struct sockaddr_in *client, int tamano){
    
    //Acepta a un cliente y lo guarda en la estructura cliente
    *clientfd = accept(*serverfd, (struct sockaddr *)&client, &tamano);
    if (*clientfd < 0){
        perror("Error en el accept");
        exit(-1);
    }
    int r;
    r = send(*clientfd,"OK",2,0);
    if (r < 0 ){
        perror("Error en send");
        exit(-1);
    }
}


int hash(int x)
{
    return x;
}

int main(){

    // Estructuras de configuracion de servidor y cliente
    struct sockaddr_in server,client;
    // Tamaño de una estructura sockaddr_in
    socklen_t tamano = sizeof(client);

    // Arreglo con los descriptores de los hilos y los parametros de la funcion 
    pthread_t hilo[BACKLOG] ;
    struct Parametros datosh[BACKLOG];

    FILE *lectura;
    FILE *escritura;
    // Tamaño calcula el tamaño de los datos
    int tamanoBuff = sizeof(struct Datos);
    int cantidad = 0,r;

    // Descriptor del socket servidor y cliente
    int clientfd, serverfd;


    // Leer datos de busqueda de la estructura
    struct Datos buffer, *bufferP;
    bufferP = &buffer;

    // Configura el servidor y acepta los clientes
    configuracionServidor(&clientfd,&serverfd,&server,&client);
    
    //___________________________________________________
    
    aceptarCliente(&clientfd,&serverfd,&client,tamano);
    //___________________________________________________
    while (1)
    {    
        // Se reciben todos los datos
        while (cantidad < tamanoBuff){
            r = recv (clientfd,bufferP+cantidad,tamanoBuff,0);
            cantidad = cantidad+r;
        }
        if (r < 0 ){
            perror("Error en recv");
            exit(-1);
        }
        
        printf("Cantidad de bytes recibidos %d i %d\n",cantidad,0);
        cantidad = 0;
        printf("El origen: %d, el destino: %d, la hora: %d\n",bufferP->idOrigen,bufferP->idDestino,bufferP->hora);
        

        //_________________________________________________________
        int origen = bufferP->idOrigen;
        int destino = bufferP->idDestino;
        int hora = bufferP->hora;

        if ((lectura = fopen("salidaHash", "rb")) == NULL)
        {
            perror("Hubo un error leyendo el archivo hash\n");
            exit(EXIT_FAILURE);
        }

        struct index indice;
        int hashOrigen = hash(origen);

        fseek(lectura, hashOrigen * sizeof(struct index), SEEK_SET);
        fread(&indice, sizeof(struct index), 1, lectura);
        
        if (indice.apuntador == -1)
        {
            //     printf("No hay registros con idOrigen %d\n", origen);
            bufferP->idOrigen = -1; // Indica que no se encontraron registros
        }                           // else {
        //     printf("El primer registro con idOrigen %d se encuentra en la posicion %ld del archivo indexado\n", indice.idOrigen, indice.apuntador);
        // }

        fclose(lectura);

        // Busqueda del registro adecuado en el archivo indexado
        if ((lectura = fopen("salidaIndex", "rb")) == NULL)
        {
            perror("Hubo un error leyendo el archivo index\n");

            exit(EXIT_FAILURE);
        }

        if (bufferP->idOrigen != -1)
        {
            fseek(lectura, (indice.apuntador - 1) * sizeof(struct Datos), SEEK_SET);
            fread(&buffer, sizeof(struct Datos), 1, lectura);

            // printf("Se encontro el registro %d %d %d\n", bufferP->idOrigen, bufferP->idDestino, bufferP->hora);

            while ((bufferP->idOrigen != origen) || (bufferP->idDestino != destino) || (bufferP->hora != hora))
            {
                if (bufferP->sig == -1)
                {
                    printf("No hay registros con los parametros indicados\n");
                    bufferP->idOrigen = -1; // Indica que no se encontraron registros
                    break;
                }

                // Leer registro siguiente
                fseek(lectura, (bufferP->sig - 1) * sizeof(struct Datos), SEEK_SET);
                fread(&buffer, sizeof(struct Datos), 1, lectura);

                // printf("Se encontro el registro %d %d %d\n", bufferP->idOrigen, bufferP->idDestino, bufferP->hora);
            }
        }
        fclose(lectura);

        //_________________________________________________________
        // if ((escritura= fopen("registro.log","wb+")) == NULL){
        //     perror("No se pudo abrir alguno de los archivos\n");
        //     exit(EXIT_FAILURE);
        // }
        // fwrite(bufferP,tamanoBuff,1,escritura);
        // fclose(escritura);
        //_________________________________________________________
        
        
        // Se envia el tiempo promedio
        while (cantidad < tamanoBuff){
            r = send(clientfd,bufferP+cantidad,tamanoBuff,0);
            cantidad = cantidad+r;
        }
        cantidad = 0;
        if (r < 0 ){
           perror("Error en send");
           exit(-1);
        }
        
        // Se cierra ambos sockets
        
        printf("Todo resulto bien\n");
    }
    close(clientfd);
    close(serverfd);
    
}