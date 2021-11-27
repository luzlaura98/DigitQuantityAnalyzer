/*
#APL 3 - Ejercicio 2
*/
#include <pthread.h>
#include <unistd.h> //sleep
#include "utils.h"

EntradaMain inputMain;
int *digitsRes;
char **files;
int *asignados; // cantidad de files asignados a cada hilo

void initDigits(int **p, int cantFiles)
{
    *p = (int *)malloc(cantFiles * CANT_DIGITS * sizeof(int));
    checkErrorAllocate(*p, errno);
    for (int i = 0; i < cantFiles * CANT_DIGITS; i++)
        (*p)[i] = 0;
}

void initAsignados(int **p, int cantFiles, int cantHilos)
{
    *p = (int *)malloc(cantHilos * sizeof(int));
    checkErrorAllocate(*p, errno);
    int *pH = *p; //para iterar vector
    int cant = cantFiles / cantHilos;
    int resto = cantFiles % cantHilos;
    for (int i = 0; i < cantHilos; i++)
    {
        int inc = 0;
        if (i < resto)
            inc++;
        *pH = cant + inc;
        pH++;
    }
}

/**
 * Asigna en files el path de los archivos de directorio.
 * Devuelve la cantidad de archivos total.
 * Si es -1, hubo un error.
*/
int initFiles(const char *directorio, char ***files)
{
    const int SPACE_QTY = 2000; //s/profe

    //Reservo espacio para 10 files, y veo si voy necesitando mÃ¡s.
    struct st_temp **temp_struct = malloc(SPACE_QTY * sizeof(char **));
    checkErrorAllocate(temp_struct, errno);
    *files = (char **)temp_struct;

    //Variables
    DIR *d;
    struct dirent *dir;
    int cont = 0;
    char **p = *files;

    if (d = opendir(directorio))
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_type == DT_REG && endswith(dir->d_name, ".txt"))
            { /* If the entry is a regular file and it's txt format. */
                cont++;
                if (cont % SPACE_QTY == 1 && cont != 1)
                {
                    size_t size = ((cont / SPACE_QTY) + 1) * sizeof(char **);
                    *files = (char **)realloc((*files), size);
                    checkErrorAllocate(*files, errno);
                    if (*files == NULL)
                        return -1;
                    p = *files + cont - 1;
                }
                //agregar file
                printf("file|%d|%s|\n", cont, dir->d_name);
                *p = (char *)malloc((strlen(dir->d_name) + 1) * sizeof(char));
                checkErrorAllocate(*p, errno);
                strcpy(*p, dir->d_name);
                p++;
            }
        }
        closedir(d);
    }
    else
    {
        int errnum = errno;
        switch (errnum) //https://man7.org/linux/man-pages/man3/opendir.3.html
        {
        case ENOENT: printf("El directorio no existe o es vacÃ­o.\n");
            break;
        case ENOMEM: printf("Insuficiente memoria.\n");
            break;
        case EACCES: printf("Permisos denegados.\n");
            break;
        default: printf("CÃ³digo de error(errno)[%d]\n", errnum);
            break;
        }
    }
    return cont;
}

void *threadFunction(void *args)
{
    InfoHilo *pAux = (InfoHilo *)args;
    InfoHilo infoHilo = *pAux;

    int ce = asignados[infoHilo.nro];
    char auxFullFilepath[MAX_LENGTH_PATH_FILE];

    if (SLOW == TRUE)
        sleep(SLOW_TIME_IN_SECONDS);

    for (int i = 0; i < ce; i++)
    {
        //concateno
        strcpy(auxFullFilepath, inputMain.directorio);
        strcat(auxFullFilepath, "/");
        strcat(auxFullFilepath, infoHilo.files[i]);

        // Leo archivo
        FILE *fp;
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        fp = fopen(auxFullFilepath, "r");
        if (fp == NULL)
        { //errno is thhread-safe. http://www.unix.org/whitepapers/reentrant.html
            int errnum = errno;
            if (errnum == EACCES)
                fprintf(stderr, "\nHilo %d: %s\nerror[PERMISO] al abrir.\n", infoHilo.nro + 1, infoHilo.files[i]);
            else if (errnum == ENOENT)
                fprintf(stderr, "\nHilo %d: %s\nerror[NO EXISTE] al abrir.\n", infoHilo.nro + 1, infoHilo.files[i]);
            else
                printf("\nHilo %d: %s\nerror al abrir. Error(errno):%d\n", infoHilo.nro + 1, infoHilo.files[i], errnum);
        }
        else
        {
            int *pResFile = infoHilo.pRes + i * CANT_DIGITS;
            while ((read = getline(&line, &len, fp)) != -1)
            {
                char *p = line;
                while (*p != '\n' && *p != '\0')
                {
                    if (*p >= '0' && *p <= '9')
                    {
                        int digit = (*p) - '0';
                        pResFile[digit]++;
                    }
                    p++;
                }
            }
            printf("\nHilo %d: %s\n[0]=%d,[1]=%d,[2]=%d,[3]=%d,[4]=%d,[5]=%d,[6]=%d,[7]=%d,[8]=%d,[9]=%d\n",
                   infoHilo.nro + 1, infoHilo.files[i],
                   pResFile[0],
                   pResFile[1],
                   pResFile[2],
                   pResFile[3],
                   pResFile[4],
                   pResFile[5],
                   pResFile[6],
                   pResFile[7],
                   pResFile[8],
                   pResFile[9]);
            fclose(fp);
        }

        if (SLOW == TRUE)
            sleep(SLOW_TIME_IN_SECONDS);
    }
}

/**
 * Suma en el vector de CANT_DIGITS posiciones
 * el total de todos los files analizados.
*/
void sumTotal(int *v, int cantFiles)
{
    for (int i = 0; i < cantFiles; i++)
        for (int j = 0; j < CANT_DIGITS; j++)
            v[j] += digitsRes[i * CANT_DIGITS + j];
}

int main(int argc, char **argv)
{
    checkParametros(argc, argv, &inputMain); //separo los params en el struct opc
    int cantFiles = initFiles(inputMain.directorio, &files);
    if (cantFiles <= 0)
    {
        printf("Cantidad de archivos vÃ¡lidos a analizar = 0\n");
        return EXIT_FAILURE;
    }
    else if (inputMain.nivel > cantFiles)
    {
        printf("El nivel de paralelismo solicitado(%d) excede la cantidad de archivos a analizar (%d)\n", inputMain.nivel, cantFiles);
        return EXIT_FAILURE;
    }
    DIVIDER;
    printf("cantidad de archivos: %d\n", cantFiles);
    printf("nivel de paralelismo: %d\n", inputMain.nivel);
    DIVIDER;

    initDigits(&digitsRes, cantFiles);
    initAsignados(&asignados, cantFiles, inputMain.nivel);

    //creo hilos
    int filesInc = 0;
    pthread_t hilos[inputMain.nivel];
    for (int i = 0; i < inputMain.nivel; i++)
    {
        InfoHilo *pInfoHilo = malloc(sizeof(InfoHilo));
        checkErrorAllocate(pInfoHilo, errno);
        pInfoHilo->nro = i;
        pInfoHilo->files = files + filesInc;
        pInfoHilo->pRes = digitsRes + filesInc * CANT_DIGITS;
        filesInc += asignados[pInfoHilo->nro];

        int ret = pthread_create(&hilos[i], NULL, threadFunction, (void *)pInfoHilo);
        if (ret == 0)
            printf("> Hilo %d creado (%d archivos asignados)\n", i + 1, asignados[pInfoHilo->nro]);
        else
            printf("> Hilo %d no pudo ser creado\n", i + 1);
    }

    for (int n = 0; n < inputMain.nivel; n++)
        pthread_join(hilos[n], NULL);

    //mostrar resultado final
    int resTotal[CANT_DIGITS] = {0};
    sumTotal(&resTotal[0], cantFiles);
    DIVIDER;
    printf("Total:\n[0]=%d,[1]=%d,[2]=%d,[3]=%d,[4]=%d,[5]=%d,[6]=%d,[7]=%d,[8]=%d,[9]=%d\n",
           resTotal[0],
           resTotal[1],
           resTotal[2],
           resTotal[3],
           resTotal[4],
           resTotal[5],
           resTotal[6],
           resTotal[7],
           resTotal[8],
           resTotal[9]);
    DIVIDER;

    //liberar memoria
    char **pFiles = files;
    for (int i = 0; i < cantFiles; i++)
    {
        free(*pFiles);
        pFiles++;
    }
    free(files);
    free(asignados);
    free(digitsRes);
    exit(EXIT_SUCCESS);

    return 0;
}