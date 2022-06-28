#include <string.h>
#include "datos.h"
#define numIDs 1160 + 1 // 1 <= idOrigen <= 1160

// Funcion hash
int hash(int x)
{
    return x;
}

int main()
{
    // Descriptores del archivo csv y de los archivos binarios
    FILE *entrada;
    FILE *salidaIndex;
    FILE *salidaHash;

    long numRegistros = 0;

    struct index tablaHash[numIDs];

    if ((entrada = fopen("archivo.csv", "rt")) == NULL || (salidaIndex = fopen("salidaIndex", "wb+")) == NULL || (salidaHash = fopen("salidaHash", "wb+")) == NULL)
    {
        perror("No se pudo abrir alguno de los archivos\n");
        exit(EXIT_FAILURE);
    }

    // Inicializar tablaHash
    for (int i = 0; i < numIDs; i++)
    {
        tablaHash[i].idOrigen = -1;
        tablaHash[i].apuntador = -1;
    }

    // Lectura linea por linea
    int tamano = 200;
    char *linea = malloc(tamano);

    if (fgets(linea, tamano, entrada) == NULL)
    { // Se salta la primera linea del .csv
        perror("Archivo vacio o error de lectura\n");
        exit(EXIT_FAILURE);
    }

    while (fgets(linea, tamano, entrada) != NULL)
    {
        struct Datos registro;
        char *delim = ",";
        // Copia la linea
        char *lineaCopia = strdup(linea);
        numRegistros++;

        // idOrigen
        char *token = strtok(lineaCopia, delim);
        if (token != NULL)
        {
            registro.idOrigen = atoi(token);

            int idHash = hash(atoi(token));
            if (tablaHash[idHash].idOrigen == -1)
            { // Es el primer registro con determinado idOrigen
                tablaHash[idHash].idOrigen = atoi(token);
                tablaHash[idHash].apuntador = numRegistros;
            }
        }

        // idDestino
        token = strtok(NULL, delim);
        if (token != NULL)
        {
            registro.idDestino = atoi(token);
        }

        // hora
        token = strtok(NULL, delim);
        if (token != NULL)
        {
            registro.hora = atoi(token);
        }

        // mediaViaje
        token = strtok(NULL, delim);
        if (token != NULL)
        {
            registro.mediaViaje = atof(token);
        }

        // standard_deviation_travel_time
        token = strtok(NULL, delim);
        if (token != NULL)
        {
            registro.standard_deviation_travel_time = atof(token);
        }

        // geometric_mean_travel_time
        token = strtok(NULL, delim);
        if (token != NULL)
        {
            registro.geometric_mean_travel_time = atof(token);
        }

        // geometric_standard_deviation_travel_time
        token = strtok(NULL, delim);
        if (token != NULL)
        {
            registro.geometric_standard_deviation_travel_time = atof(token);
        }

        // sig
        registro.sig = -1;

        // Guardar registro
        fwrite(&registro, sizeof(struct Datos), 1, salidaIndex);
        printf("Se guardo el registro %ld con idOrigen %d\n", numRegistros, registro.idOrigen);
    }

    // Guardar tabla hash
    for (int i = 0; i < numIDs; i++)
    {
        fwrite(&(tablaHash[i]), sizeof(struct index), 1, salidaHash);
    }

    free(linea);
    long *enlistador = malloc(numIDs * sizeof(long));

    // Inicializar enlistador
    for (int i = 0; i < numIDs; i++)
    {
        *(enlistador + i) = -1;
    }

    // Enlazado de registros con el mismo idOrigen, leyendo de abajo hacia arriba
    struct Datos tmp;
    for (int i = 1; i <= numRegistros; i++)
    {
        fseek(salidaIndex, -i * sizeof(struct Datos), SEEK_END);
        fread(&tmp, sizeof(struct Datos), 1, salidaIndex);
        if (*(enlistador + tmp.idOrigen) != -1)
        {
            tmp.sig = *(enlistador + tmp.idOrigen);
        }
        *(enlistador + tmp.idOrigen) = numRegistros - i + 1;

        fseek(salidaIndex, -i * sizeof(struct Datos), SEEK_END);
        fwrite(&tmp, sizeof(struct Datos), 1, salidaIndex);

        printf("Se guardo el registro %ld con idOrigen %d y sig %ld\n", numRegistros - i + 1, tmp.idOrigen, tmp.sig);
    }

    // free(enlistador);     // crea segfault?

    fclose(entrada);
    fclose(salidaIndex);
    fclose(salidaHash);

    exit(EXIT_SUCCESS);
}