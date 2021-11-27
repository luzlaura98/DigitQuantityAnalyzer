#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //str...
#include <dirent.h> //dir
#include <errno.h>  //get error fopen

#define AYUDA "------------------Ayuda------------------\n\
# Modo de empleo: [OPCIÓN] [DIRECTORIO]\n\
#   -n      nivel de paralelismo, número de threads:\n\
# Se desea desarrollar un programa capaz de contar la cantidad de \n\
# números del 0 al 9 que aparece en todos los archivos de texto (con \n\
# extensión .txt) que se encuentra en un determinado directorio. Para \n\
# ello deberá recibir un número N que indique el nivel de paralelismo \n\
# a nivel de threads y el path al directorio. Cada hilo deberá leer \n\
# un archivo y contabilizar la cantidad de número que leyó. \n\
# Adicionalmente al final se deberá mencionar la cantidad de números \n\
# leídos totales. El criterio es que se individualice a cada carácter \n\
# entre letra y número, en caso de pertenecer al conjunto numérico \n\
# se contabilizará. Por ejemplo la palabra: “1hola451”, sumaría dos \n\
# apariciones al número 1, una aparición al número 4 y una aparición \n\
# al número 5. \n\
# Ejemplo:\n\
#           ./e2 -n4 /home/user/mi_directorio"

//Constantes
#define CANT_PARAMS 3 // base + filepath + n // [FICHERO] -n [NIVEL]
#define REASONABLE_THREAD_MIN 1
#define MAX_LENGTH_PATH_FILE FILENAME_MAX
#define CANT_DIGITS 10
#define DIVIDER printf("-----------------------\n")
#define SLOW TRUE
#define SLOW_TIME_IN_SECONDS 1

typedef struct
{
    char directorio[MAX_LENGTH_PATH_FILE];
    int nivel;
} EntradaMain;

typedef struct
{
    int nro;
    char ** files;//vector, primera pos de files asignado a ese hilo.
    int* pRes; //puntero a vector correspondiente de res.
} InfoHilo;

typedef enum
{
    FALSE,
    TRUE
} boolean;

int extraerNumero(char *input)
{
    char *endp;
    int num = strtol(input, &endp, 10);
    if (*endp != '\0')
        num = -1;
    return num;
}

boolean existeDirectorio(const char *directorio)
{
    DIR *dir = opendir(directorio);
    if (dir)
    {
        closedir(dir); //existe
        return TRUE;
    }
    return FALSE;
}

/* Agrega en resultPath el path absoluto.
Retorna si el path es valido.*/
boolean cargarPathAbsoluto(char *resultPath, const char *path)
{
    if (existeDirectorio(path) == TRUE)
    {
        strcpy(resultPath, realpath(path, NULL)); //sea relativo o no trae el path absoluto
        if (resultPath != NULL)
            return TRUE;
    }
    return FALSE;
}

/**
 * path -n2
 * path -n 2
 * -n2 path
 * -n 2 path
 * -2 path
*/
void checkParametros(int cantParams, char **pParams, EntradaMain *pEntrada)
{
    if (cantParams == 2 && (strcmp(pParams[1], "--help") == 0 || strcmp(pParams[1], "-h") == 0))
    {
        printf(AYUDA);
        exit(EXIT_SUCCESS);
    }
    if (cantParams != CANT_PARAMS && cantParams != (CANT_PARAMS + 1))
    {
        printf("Cantidad de parámetros (%d) invalido.\n", cantParams);
        exit(EXIT_FAILURE);
    }
    int i = 1;
    boolean continuaSig = FALSE;
    while (i < cantParams && (pEntrada->directorio[0] == '\0' || pEntrada->nivel == 0))
    {
        if (continuaSig == TRUE && pEntrada->nivel == 0)
        {
            int nivel = extraerNumero(pParams[i]);
            if (nivel >= REASONABLE_THREAD_MIN)
                pEntrada->nivel = nivel;
            else
            {
                printf("El nivel de threads no es válido: <<%s>>\n", pParams[i]);
                exit(EXIT_FAILURE);
            }
        }
        else if (pParams[i][0] == '-')
        {
            boolean tieneN = FALSE;
            if (strlen(pParams[i]) >= 2)
            {
                if (pParams[i][1] >= 'a' && pParams[i][1] <= 'z')
                {
                    if (pParams[i][1] == 'n')
                    {
                        tieneN = TRUE;
                        if (pParams[i][2] == '\0')
                            continuaSig = TRUE;
                    }
                    else
                    {
                        printf("Opción incorrecta -- <<%c>>\n", pParams[i][1]);
                        exit(EXIT_FAILURE);
                    }
                }
                if (continuaSig == FALSE)
                {
                    int nivel = -1;
                    if (tieneN == TRUE) // -n7
                        nivel = extraerNumero(pParams[i] + 2);
                    else // -7
                        nivel = extraerNumero(pParams[i] + 1);

                    if (nivel >= REASONABLE_THREAD_MIN)
                        pEntrada->nivel = nivel;
                    else
                    {
                        short inc = 1;
                        if (tieneN == TRUE)
                            inc++;
                        printf("El nivel de threads no es válido: <<%s>>\n", pParams[i] + inc);
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else
            {
                printf("Opción y argumento faltante\n");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            if (!cargarPathAbsoluto(pEntrada->directorio, pParams[i]))
            {
                printf("Directorio de entrada no válido: <<%s>>\n", pParams[i]);
                exit(EXIT_FAILURE);
            }
        }
        i++;
    }
    if (i != cantParams) //params demás
    {
        printf("Cantidad de parámetros invalido: i=%d.\n", i);
        exit(EXIT_FAILURE);
    }
}

void checkErrorAllocate(void* puntero, int errornum){
   if (puntero == NULL)
    {
        int errornum = errno;
        if (errornum == ENOMEM)
        {
            printf("No se puede continuar, insuficiente memoria.");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("No se puede continuar, error(errno):%d.", errornum);
            exit(EXIT_FAILURE);
        }
    }
}

boolean endswith(char *str, char *substr)
{
    if (!str || !substr) return FALSE;

    size_t length_of_string = strlen(str);
    size_t length_of_substring = strlen(substr);

    if (length_of_substring > length_of_string)
        return FALSE;

    if (strcmp(str + length_of_string - length_of_substring, substr) == 0)
        return TRUE;

    return FALSE;
}

#endif