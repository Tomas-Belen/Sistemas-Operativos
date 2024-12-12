
Bienvenid@s, en este directorio se encuentra el trabajo práctico referido a la consigna presentada en la cátedra de Sistemas Operativos, en el marco del 2.º proyecto de la materia. El proyecto, elaborado por Belén Tomás, Ferreyra Joaquín Ignacio, Figueroa Jeremías y Ramírez Alexis Diego, presenta en los distintos archivos el código necesario para ejecutar un sistema de semáforos nombrados en espacio de usuario.
### Implementacion de semaforos
En primer lugar, decidimos implementar los semáforos en un archivo separado en la seccion kernel, `semaphore.c`, siguiendo la recomendación de mantener el código modular y fácil de localizar durante la evaluación. Basamos nuestra implementación en los conceptos vistos en clase, donde se discutió que los semáforos pueden implementarse utilizando spinlocks. Por lo tanto, reutilizamos funciones ya definidas en `spinlock.c` para manejar las regiones críticas.
Nuestra estructura de semáforo consta de dos elementos principales: Un valor que representa el estado del semáforo. Un spinlock, que protege las operaciones críticas asociadas al semáforo. Cabe aclarar que definimos un arreglo de semaforos sobre el cual buscar a aquellos que estuviesen disponibles (con un limite maximo definido en 512).
Adicionalmente, desarrollamos dos funciones auxiliares:
1. `int get_sem(int value);` que busca un semáforo disponible en un arreglo predefinido.
2. `void sem_array_init(void);` que inicializa el arreglo de semáforos, configurando sus valores en -1. 

Estas funciones adicionales nos permitieron trabajar con múltiples semáforos de manera eficiente y organizada. Optamos por un arreglo de semáforos porque queríamos iterar fácilmente entre ellos, y comenzar con un valor de -1 nos pareció una forma lógica de indicar que un semáforo aún no ha sido configurado.

`int sem_open (int sem, int value);`: Abre un semáforo dado su índice (sem) e inicializa su valor a value. Verifica primero si el índice es válido y si el semáforo está cerrado (valor -1). Si está cerrado, lo abre asignando el valor y devuelve 1.

`int sem_up (int sem);`: Incrementa el valor del semáforo indicado. Si el valor es 0, despierta a los procesos que están esperando en ese semáforo (mediante wakeup). Si el semáforo está cerrado (valor negativo), da un error y no incrementa.

`int sem_down(int sem);`: Disminuye el valor del semáforo indicado. Si el valor es 0, pone a dormir el proceso que está intentando hacer esta operación, bloqueándolo hasta que otro proceso libere el semáforo (haciéndole un sem_up). Si el semáforo está cerrado (valor negativo), devuelve un error.

`int sem_close(int sem);`: Cierra el semáforo en la posición indicada por el índice sem. Verifica si el semáforo ya está cerrado o si el índice es inválido. Si el semáforo está abierto, lo cierra asignándole el valor -1 y devuelve 1; si ya está cerrado o el índice es inválido, devuelve 0.

Todas estas funciones comparten una estructura general que incluye:
- Manejo de errores para asegurar la robustez del sistema.
- Operaciones críticas protegidas por acquire y release del spinlock.
- Un retorno adecuado para indicar éxito o fallo.
En particular, en sem_down utilizamos un while en lugar de un if para evitar problemas de concurrencia en escenarios donde múltiples procesos intentan acceder simultáneamente al semáforo.


### Syscall de demostración: pingpong.c
Para demostrar el funcionamiento de los semáforos, implementamos una syscall en espacio de usuario llamada `pingpong.c`, que imprime "ping" y "pong" de manera sincronizada.
El archivo pingpong.c maneja dos procesos creados con fork():
    • El proceso hijo imprime "pong".
    • El proceso padre imprime "ping".
Cada proceso obtiene su semáforo correspondiente utilizando get_sem. La sincronización se logra mediante un ciclo donde el proceso hijo realiza sem_down sobre su semáforo, imprime "pong", y hace sem_up sobre el semáforo del padre; mientras que el proceso padre realiza la operación inversa (imprimiendo ping en el medio). Esto asegura que las impresiones ocurran en el orden correcto. Finalmente, el proceso padre utiliza wait para esperar al hijo, y ambos semáforos son cerrados con sem_close.

### Integración de la syscall en xv6

Por otra parte, la integración de la syscall en xv6 requirió varias modificaciones de la documentacion recibida:
    1. Declarar la syscall en `defs.h` y agregar su implementación en `sysproc.c`.
    2. Asignar un número único a la syscall en `syscall.h`.
    3. Mapear el número de syscall a su función correspondiente en `syscall.c`.
    4. Exponer la syscall al espacio de usuario mediante `user.h`.
    5. Incluirla en el Makefile para asegurar su correcta compilación.

Con respecto a las decisiones de diseño tomadas por el grupo, la más destacable fue manejar y delegar la inicialización del arreglo de semáforos en el archivo main.c de la sección kernel, y de esta forma evitar problemas de concurrencia.
En detalles menores, queda indicado de forma más específica y a disposición del lector, las descripciones y aclaraciones en comentarios dentro del mismo código implementado. 
Esperamos que cumpla con la consigna solicitada y sea de su agrado. Muchas gracias.

Por último, cabe aclarar que el tag del proyecto púlido hasta al último detalle será al correspondiente a pingpong_v0.1.0
