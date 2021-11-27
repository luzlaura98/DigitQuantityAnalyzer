# DigitQuantityAnalyzer

## Ejercicio 2:
Se desea desarrollar un programa capaz de contar la cantidad de números del 0 al 9 que aparece en
todos los archivos de texto (con extensión .txt) que se encuentra en un determinado directorio. Para
ello deberá recibir un número N que indique el nivel de paralelismo a nivel de threads y el path al
directorio. Cada hilo deberá leer un archivo y contabilizar la cantidad de número que leyó.
Adicionalmente al final se deberá mencionar la cantidad de números leídos totales.
El criterio es que se individualice a cada carácter entre letra y número, en caso de pertenecer al
conjunto numérico se contabilizará. Por ejemplo la palabra: “1hola451”, sumaría dos apariciones al
número 1, una aparición al número 4 y una aparición al número 5.
La salida por pantalla debe ser:
Thread 1: Archivo leído test.txt. Apariciones 0=${cantCeros}, 1=${cantUnos}, etc
Thread 2: Archivo leído prueba.txt. Apariciones 0=${cantCeros}, 1=${cantUnos}, etc
Thread 1: Archivo leído pepe.txt. Apariciones 0=${cantCeros}, 1=${cantUnos}, etc
Finalizado lectura: Apariciones total: 0=${cantTotalCeros}, 1=${cantTotalUnos}, etc

### Criterios de corrección 📋
Compila sin errores con el makefile entregado
Funciona correctamente según enunciado
Existe algún tipo de ayuda para la ejecución de los procesos (-h, --help)
Valida que N sea un número entero mayor o igual a 1
Si el número N es mayor a la cantidad de archivos los threads sobrante no ejecutan
Finalizan correctamente todos los hilos
No hay pérdida de información
