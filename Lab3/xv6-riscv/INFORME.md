
# INFORME LAB3 SO

[TOC]

## Primera parte: Analisis de la documentacion de xv6-riscv

#### ¿Qué política de planificación utiliza *xv6-riscv* para elegir el próximo proceso a ejecutarse?

La política de planificación utiliza `xv6-riscv` es RR (Round Robin), la historia de como nos dimos cuenta fue al ejecutar una ronda de *cpubench* en paralelo con una ronda de *iobench* pudimos ver la intercalación que sucedia con los process id (PID).
Esto sirvio como confirmacion de lo que habiamos encontrado en el libro **xv6: a simple, Unix-like teaching operating system** dentro del cual (especificamente en el capitulo 7.10: "Real World") se identifica a esta politica de planificacion como la que esta implementada.  

#### ¿Cúales son los estados en los que un proceso puede permanecer en *xv6-riscv* y qué los hace cambiar de estado?

Los distintos estados posibles de un proceso son:  sin utilizar  (UNUSED), usado (USED), durmiendo (SLEEPING), apto para correr o listo (RUNNABLE),corriendo (RUNNING), o zombie. Lo que los hace cambiar de estado son funciones definidas en proc.c: 

- `void scheduler(void);`: RUNNABLE -> RUNNING; 
- `void yield(void);`: RUNNING -> RUNNABLE; 
- `void sleep(void *chan, struct spinlock *lk);`: RUNNING -> SLEEPING; 
- `void wakeup(void *chan);`: SLEEPING -> RUNNABLE; 
- `int wait(uint64 addr);`: ZOMBIE -> UNNUSED; 
- `void exit(int status);`: RUNNING -> ZOMBIE; 
- `static struct proc* allocproc(void);`: UNNUSED -> USED;
- `int fork(void);`: UNNUSED -> RUNNABLE.



#### ¿Qué es un *quantum*? ¿Dónde se define en el código? ¿Cuánto dura un *quantum* en *xv6-riscv*?

Un *quantum* es un segmento de tiempo que el scheduler define, dentro del cual cada proceso va a ejecutarse. Al terminar el *quantum* asignado a un proceso, este se detiene y cede el control del cpu para que otro proceso empieze a correr, el cual va a tener el mismo quantum para trabajar.
Dentro del codigo de `xv6-riscv`, la duracion del *quantum* esta definida en la seccion de kernel donde se inicializa el temporizador, en `start.c`, y la duracion de un *quantum* en es 1.000.000 de ticks (1 decima de segundo).


#### ¿En qué parte del código ocurre el cambio de contexto en *xv6-riscv*? ¿En qué funciones un proceso deja de ser ejecutado? ¿En qué funciones se elige el nuevo proceso a ejecutar?

Los cambios de contexto ocurren en la seccion donde estan definidas las funciones concernientes a los procesos: `proc.c`. 
En gral. todas las funciones que cambian los procesos a un estado de no ejecucion son: "yield"; "sleep"; y "exit". Podemos observar que todas ellas hace uso internamente de la funcion `sched`, la cual se encarga de efectivamente hacer el cambio de contexto.
La funcion "scheduler" elige el nuevo proceso a ejecutar tomando aquel que esta listo.
Un detalle a tener en cuenta surge al ver que las funciones concernientes al cambio de contexto o que lo efectuan usan la función *yield* la cuál desplanifica al proceso en ejecución actual, para que luego la función *sched* proceda a la elección del proceso que continuará para ejecutarse de modo que la función *scheduler* vuelva del trap que se hizo para pasar al modo kernel y elegir a que nuevo proceso pasar a ejecutar.


#### ¿El cambio de contexto consume tiempo de un *quantum*?

Un cambio de contexto si consume tiempo de un *quantum* pues en `xv6-riscv` esta definido de manera general a todos los procesos. El gran lector se preguntara *cómo es que se dieron cuenta de está cuestión?* Fácil, fueron demasiadas ejecuciones de distintas rondas de procesos de tipo cpubench donde ciertamente el tiempo que se usa en el cambio de contexto en *xv6* al estar emulado en *qemu* es tan pequeña que no es muy notable a simple vista. 

## Segunda Parte: Medicion de operaciones de cómputo y de entrada/salida en xv6-riscv

### Experimento 1: Planificacion de programas iobound y cpubound

Para responder apropiadamente a esta pregunta, comenzaremos exponiendo las diferentes situaciones de ejemplo que utilizamos para analizar el comportamiento de los programas dados, y los resultados de nuestro analisis según la metrica. Primero y principal, exponemos las situaciones mas comunes para cada programa, que es la ejecucion en solitario:

1. **cpubench N &**

| PID | Ciclos | benchmark  | metric   | elapsed ticks |
| --- | ------ | ---------- | -------- | ------------- |
| 7   | 2      | [cpubench] | 14166400 | 38            |
| 8   | 3      | [cpubench] | 15196038 | 35            |
| 9   | 4      | [cpubench] | 15231542 | 35            |
| 10  | 5      | [cpubench] | 15338057 | 35            |
| 11  | 6      | [cpubench] | 15125028 | 35            |
| 12  | 7      | [cpubench] | 15155461 | 35            |
| 13  | 8      | [cpubench] | 14817000 | 36            |
| 14  | 9      | [cpubench] | 14919474 | 36            |

2. **iobench N &**

| PID | N | benchmark | metric | elapsed ticks |
| --- | - | --------- | ------ | ------------- |
| 4   | 2 | [iobench] | 3663   | 279           |
| 4   | 3 | [iobench] | 3685   | 278           |
| 4   | 4 | [iobench] | 3506   | 279           |
| 4   | 5 | [iobench] | 3688   | 278           |
| 4   | 6 | [iobench] | 2841   | 365           |
| 4   | 7 | [iobench] | 3498   | 297           |
| 4   | 8 | [iobench] | 2815   | 366           |
| 4   | 9 | [iobench] | 3737   | 273           |

Ahora que tenemos un comportamiento definido para cada programa por separado, procedemos a mostrar nuestros resultados con casos mas complejos:


3. **cpubench N & ; cpubench N & ; cpubench N &**

| PID | Ciclos | Benchmark  | metric  | Elapsed_ticks |
| --- | ------ | ---------- | ------- | ------------- |
| 17  | 2      | [cpubench] | 4283698 | 125           |
| 22  | 3      | [cpubench] | 4199877 | 115           |
| 27  | 4      | [cpubench] | 4743264 | 113           |
| 32  | 5      | [cpubench] | 4801103 | 112           |
| 37  | 6      | [cpubench] | 4904302 | 110           |
| 42  | 7      | [cpubench] | 4799974 | 113           |
| 47  | 8      | [cpubench] | 5044440 | 106           |
| 52  | 9      | [cpubench] | 4399675 | 124           |

Aqui notamos que, cuando tenemos mas de una instancia de un programa que utiliza mucho el cpu `cpubench N &`, su rendimiento disminuye considerablemente, pues la metrica muestra una eficiencia de 2/3 menor al caso 1. Como cpubench hace una gran cantidad de operaciones, va a necesitar mas tiempo del nucleo, y al tener varias instancias de este programa, va a darse una mayor cantidad de cambios de contexto en los que todas las instancias van a estar esperando por mas tiempo real por el CPU, lo que deriva en la perdida de rendimiento mencionada. 

4. **iobench N & ; iobench N & ; iobench N &**

| PID | N | benchmark | metric | elapsed ticks |
| --- | - | --------- | ------ | ------------- |
| 4   | 2 | [iobench] | 4951   | 208           |
| 4   | 3 | [iobench] | 4869   | 210           |
| 4   | 4 | [iobench] | 5062   | 198           |
| 4   | 5 | [iobench] | 4501   | 229           |
| 4   | 6 | [iobench] | 4377   | 232           |
| 4   | 7 | [iobench] | 4322   | 234           |
| 4   | 8 | [iobench] | 4553   | 224           |
| 4   | 9 | [iobench] | 4638   | 218           |

Aqui podemos observar el efecto contrario: Tenemos mas instancias de un proceso que no utiliza mucho tiempo de CPU, y un rendimiento promedio que aumento, por lo que vemos una mejora respecto al caso 2. En esta situacion los cambios de contexto se dan entre procesos que hacen pocas operaciones, por lo tanto el rendimiento con cada cambio de contexto mejora.


5. **cpubench N & ; iobench N & ; iobench N & ; iobench N &**

| PID | Ciclos | Benchmark  | metric   | Elapsed ticks | Benchmark | metric | Elapsed ticks |
| --- | ------ | ---------- | -------- | ------------- | --------- | ------ | ------------- |
| 8   | 2      | [cpubench] | 14037135 | 40            | [iobench] | 8412   | 139           |
| 16  | 3      | [cpubench] | 14037136 | 38            | [iobench] | 9453   | 139           |
| 24  | 4      | [cpubench] | 13076536 | 41            | [iobench] | 9645   | 149           |
| 34  | 5      | [cpubench] | 12097991 | 45            | [iobench] | 8406   | 137           |
| 60  | 6      | [cpubench] | 12623501 | 42            | [iobench] | 8544   | 149           |
| 71  | 7      | [cpubench] | 13209351 | 40            | [iobench] | 9080   | 119           |
| 80  | 8      | [cpubench] | 13977666 | 38            | [iobench] | 9931   | 122           |
| 90  | 9      | [cpubench] | 13146509 | 40            | [iobench] | 9728   | 133           |

Estamos en presencia en un caso muy interesante. Aqui tenemos instancias de `cpubench` y `iobench` en espera por el CPU, pero son mas las instancias del segundo que del primero. En este caso, rapidamente deducimos que la eficiencia de ambos disminuye, pero no a un nivel tan drastico como en el caso 3 para `cpubench`. Lo interesante es que, a diferencia del caso 4, por mas que tengamos varias instancias de `iobench`, su eficiencia no aumente, consecuencia logica de tener entre los procesos en espera a uno que use mucho el cpu, pues su presencia disminuye la eficiencia respecto al caso 4. 

 
6. **iobench N &; cpubench N & ; cpubench N & ; cpubench N &**

| PID | Ciclos | Benchmark  | metric | Elapsed ticks |  Benchmark | metric | Elapsed ticks |
| --- | ------ | ---------- | ------ | ------------- | ---------  | ------ | ------------- |
| 8   | 2      | [cpubench] | 373283 | 1435          | [iobench]  | 554    | 2653          |
| 16  | 3      | [cpubench] | 380053 | 1508          | [iobench]  | 550    | 2798          |
| 24  | 4      | [cpubench] | 338212 | 1620          | [iobench]  | 664    | 2860          |
| 34  | 5      | [cpubench] | 346509 | 1497          | [iobench]  | 680    | 2797          |
| 60  | 6      | [cpubench] | 385628 | 1559          | [iobench]  | 645    | 2720          |
| 71  | 7      | [cpubench] | 402959 | 1461          | [iobench]  | 685    | 2597          |
| 80  | 8      | [cpubench] | 388192 | 1511          | [iobench]  | 617    | 2642          |
| 90  | 9      | [cpubench] | 400351 | 1485          | [iobench]  | 685    | 2618          |

En este ejemplo podemos notar como sin lugar a dudas la presencia de muchas instancias de `cpubench` en paralelo afecta negativamente a todos los procesos que esperan por el cpu, pues tanto los del mismo tipo como los `iobench` sufren una perdida de rendimiento considerable. Esto ocurre porque la instancia de `iobench` esta mas tiempo esperando por el cpu, pues al tener mas instancias de un proceso cpubound, van a darse mas cambios de contexto con procesos cpubound, entonces el proceso iobound va a estar mas tiempo esperando, y su rendimiento va a decaer.  

Con todas las explicaciones dadas y los datos recopilados, ahora estamos en condiciones de responder a las interrogantes presentadas para este experimento

##### ¿Los procesos se ejecutan en paralelo? ¿En promedio, qué proceso o procesos se ejecutan primero? Hacer una observación cualitativa.

Si se ejecutan procesos en paralelo. Podemos observar que en general, el planificador que tiene `xv6-riscv` ejecuta primero los procesos iobound, ya que e tardan menos tiempo pues ejecutan muchas menos operaciones que los procesos iobound.
Con esto no se quiere decir "que son los procesos `iobenchs` priorizados" sino que al tener la característica de realizar operaciones E/S, tienden a bloquearse hasta completar las operaciones puntuales de E/S para luego seguir con su ejecución. A paartir de que los procesos iobenchs se bloquean, el planificador alterna entre los demás procesos `cpubenchs` para darles su espacio de ejecución y que hagan todas sus operaciones dentro del quanto establecido.  

Sí, lo sabemos es una conclusión muy rápida que puede ocasionar paralisis de cualquier tipo al lector pero tenemos evidencia (y mucha...). Por ahora centremosnos en observar los siguientes gráficos que nos permitirian ver una realidad más acertada de lo que pasa:

![Gráfico](https://bitbucket.org/sistop-famaf/so24lab3g24/raw/f61cd5989902a3026cee03e8cf299597a593a006/xv6-riscv/Graficos/cpubench_e_iobench.png)

![Gráfico](https://bitbucket.org/sistop-famaf/so24lab3g24/raw/f61cd5989902a3026cee03e8cf299597a593a006/xv6-riscv/Graficos/iobench_y_varios_cpubench.png)

Estos gráficos nos muestran mejor, la forma en que se ejecutan los procesos, donde si vemos por la metrica (la cuál es cantidad de operaciones por ticks) vemos que un iobench tiene menos cantidad de operaciones por ticks debido al poco tiempo que tiene en la cpu y en comparación, los cpubenchs al ocupar más tiempo dentro de la CPU, logran hacer mucho más operaciones en menos tiempo. 

##### ¿Cambia el rendimiento de los procesos iobound/cpubound con respecto a la cantidad y tipo de procesos que se estén ejecutando en paralelo? ¿Por qué?

El rendimiento de los procesos iobound/cpubound SI cambia con respecto a la cantidad y tipo de procesos que se estan ejecutando en paralelo. Mas especificamente, el rendimiento mejora en los procesos iobound si tenemos mas procesos iobound en paralelo, contrariamente a los cpubound, cuyo rendimiento se ve afectado en cualquier situacion donde hayan mas de una instancia de otro proceso. En tercer lugar, los procesos iobound si tienden a ser menos eficientes si tenemos procesos cpubound en paralelo. Este último caso lo podemos notar en los siguientes gráficos: 

![Gráfico](https://bitbucket.org/sistop-famaf/so24lab3g24/raw/f61cd5989902a3026cee03e8cf299597a593a006/xv6-riscv/Graficos/cpubench_e_iobench.png)

![Gráfico](https://bitbucket.org/sistop-famaf/so24lab3g24/raw/f61cd5989902a3026cee03e8cf299597a593a006/xv6-riscv/Graficos/iobench_y_varios_cpubench.png)

Aquí si observamos la métrica respecto del iobenchs de este gráfico respecto al gráfico del experimento cpubench N &; iobench N &; iobench N &; iobench N &; (el que fue presentado en la primer pregunta de está sección) podemos ver que la metrica ha sido incrementada en este punto, y es que, los procesos `iobenchs`al bloquearse para esperar la terminación de las operaciones de E/S, ceden la *CPU* provocando que se ejecuten de manera directa los procesos `cpubench`lo que hace que los iobenchs deban esperar a que se terminen de ejecutar todos los procesos de `cpubench`para lograr terminar su ejecución. Por lo tanto, podríamos concluir que, si bien entre procesos de una misma naturaleza no provocan cambios significativos en cuanto su rendimiento, cuando rodeamos a un proceso `cpubench`de varios procesos `iobench` este no tiende a sufrir cambios tan drásticos en su comportamiento como si un proceso iobench.



##### ¿Es adecuado comparar la cantidad de operaciones de cpu con la cantidad de operaciones iobound?

Podriamos concluir que no debido a que al tratarse de tipos de procesos distintos, por lo general los procesos iobound aprovenchan mejor el tiempo de CPU que se les asigna por su propia naturaleza de realizar menos operaciones. Por el contrario, los procesos cpubound necesitan  mas tiempo de CPU para terminar de realizar su trabajo. Por ello observamos que en todas las situaciones en las que tenemos muchos procesos esperando su turno para usar el nucleo su rendimiento tiende a disminuir de manera notable. 

### Experimento 2: Cambiando el Quantum

##### ¿Fue necesario modificar las métricas para que los resultados fueran comparables? ¿Por qué?
En nuestro casi SI fue necesario cambiar ligeramente las metricas a fin de no perder informacion, pues notamos que al disminuir el quantum del planificador, la cantidad de ticks que le tomaban a los programas en ejecutarse era mayor, lo cual desembocaba en una metrica 10 veces menor, o incluso 100 veces menor. Esto nos llevo a aumentar con un quantum de 10000 y 1000 en 10 y en 100 respectivamente, asi obtener resultados comparables, para ver porcentualmente que diferencias encontrabamos.

##### ¿Qué cambios se observan con respecto al experimento anterior? ¿Qué comportamientos se mantienen iguales?

El cambio mas notorio observado al disminuir el tamaño del quantum es que a todos los procesos en general les tomaba mas ticks completar sus operaciones en todos los experimentos (motivo que nos llevo a cambiar las metricas). A continuacion los ejemplos puestos en comparacion con el quantum del experimento 1, en formato de graficos provenientes de tablas del mismo tipo:

![Gráfico](https://bitbucket.org/sistop-famaf/so24lab3g24/raw/f61cd5989902a3026cee03e8cf299597a593a006/xv6-riscv/Graficos/cpubench.png)

![Gráfico](https://bitbucket.org/sistop-famaf/so24lab3g24/raw/f61cd5989902a3026cee03e8cf299597a593a006/xv6-riscv/Graficos/iobench.png)

![Gráfico](https://bitbucket.org/sistop-famaf/so24lab3g24/raw/f61cd5989902a3026cee03e8cf299597a593a006/xv6-riscv/Graficos/varios_cpubench.png)

![Gráfico](https://bitbucket.org/sistop-famaf/so24lab3g24/raw/f61cd5989902a3026cee03e8cf299597a593a006/xv6-riscv/Graficos/varios_iobench.png)


Si bien es una buena forma comparar ciertos experimentos entre si con un quanto más chico, para sacar mejores conclusiones respecto a su mejora o no. 
En el primer caso de ejecutar un cpubench solo, vemos que tiende a bajar un poco la cantidad de operaciones por ticks que realiza, aunque si evaluamos un poco está tendencia, se debe a que a menor quanto, menos operaciones puede hacer, pero claro está que al ser un único proceso es bastante normal que sucede eso.
En el caso de iobench, comparar la cantidad de operaciones por ticks con ambos quanto nos permite ver que este ya no sufre de tan mal rendimiento a medida que uno va disminuyendo el quanto, por lo que tiende a mejorar considerablemente cuando está siendo ejecutando por si solo.
Cuando juntamos varios procesos a ejecutar en paralelo y estos son de la misma naturaleza, no sufren de empeorar su rendimiento, sino más bien tienden a mantener la consistencia de la tendencia que hablamos anteriormente (y más si se trata de un proceso iobench).
El tema interesante se encuentra cuando intercalamos los procesos:
    *Si tomamos el ejemplo del experimento de `cpubench N &; iobench N &; iobench N &; iobench N &;`es notorio ver como ambos procesos tienden a mejorar considerablemente su rendimiento al ejecutar en paralelo, dado a que como los iobench se bloquean antes de terminar su quanto, permite que la ejecución del proceso cpubench sea efectiva y concisa logrando que este no tarde tanto.
    *Si tomamos el ejemplo contrario al anterior, es decir, el experimento `iobench N &; cpubench N &; cpubench N &; cpubench N &;` vemos exactamente la misma mejora en sus metricas.
Esto nos permite concluir que *a menor quanto, los procesos tienden a tener mejores rendimientos.*

##### ¿Con un quantum más pequeño, se ven beneficiados los procesos iobound o los procesos cpubound?
Con todos los analisis hechos, concluimos que los procesos `iobench` se ven beneficiados con un quantum mas pequeño, pues no tienden a sufrir una perdida de rendimiento tan bestial como si les pasa a los procesos `cpubench`. Aunque si bien los procesos `cpubench` todavía tienden a tener un rendimiento excesivamente más alto que los iobench, estos tienden a mejorar su performance con un quanto más chico respecto a los experimentos con quanto más grande.

## Tercera parte: Asignacion de prioridad a los procesos

Nuestro objetivo en este punto fue comenzar a implementar el planificador *Multi-Level Feedback Queue* o MLFQ, concretamente asignando prioridad a los procesos en general. 
En primer lugar, definimos  en el modulo `proc.h` un maximo de prioriades *N_PRIO* = 3. Por consiguiente, cada proceso tendria una prioridad entre 0 y 2 numericamente.
En segundo lugar, agregamos dentro de la estructura del proceso 3 campos nuevos: `int priority`, que le asigna la prioridad a un proceso; `uint64 time_ejecuted`, que guarda la cantidad de elecciones de un proceso por parte del CPU; y `uint64 used_full_quanto` que verifica si un proceso utilizo todo su quantum asignado (0 => `false`; 1 => `true`).
Por otro lado, para garantizar que las reglas para la inicalizacion y descenso/ascenso de un proceso se cumplan, modificamos la seccion `proc.c` de la siguiente manera:
1. En la funcion `static struct proc* allocproc(void);` asignamos la prioridad del proceso como *N_PRIO-1* (es decir 2); para seguir con la regla de que "cuando un proceso se inicia, su prioridad sera maxima"; y designamos que la cantidad de elecciones al inicio es nula; por lo cual no utilizo todo su quantum evidentemente.
2. Para cambiar la prioridad de un proceso implementamos la funcion `static void scheduler_mlfq_change_priority(struct proc* p);` la cual sera utilizada por la funcion `void scheduler(void);` para aumentar la prioridad de un proceso se bloquea antes de terminar su quantum; o bien disminuirla de haber finalizado su quantum asignado.
3. En la seccion `void procdump(void)` nos aseguramos de que tanto la cantidad de selecciones de un proceso como su prioridad se impriman, incluyendolos en su planificacion. Cuyo resultado es visible en la siguiente imagen:

![Procdump](https://bitbucket.org/sistop-famaf/so24lab3g24/raw/0979b52422bdf805a35d208e8dca7db56e9cdf9d/xv6-riscv/Graficos/procdump.png)

Observamos que los procesos `cpubench` tienen una prioridad mas baja prioridad mientras que los `iobench` tienen prioridad alta prioridad, que es el comportamiento deseado. 

Hasta este punto hemos implementado eficazmente la inicializacion y el cambio de prioridad de un proceso. Las demas funcionalidades propias de un planificador MLFQ estan dispuestas a continuacion.


## Cuarta parte: Implementar MLFQ

Aca nos ocupamos de completar las implementacion pertinentemente para que `xv6-riscv` funcione con la politica de planificacion MLFQ en su totalidad. Al igual que en la parte previa, implementamos en `proc.c` otra funcion de manera modularizada , que se encarga de seleccionar al proceso que va a ejecutar: `struct proc*scheduler_mlfq_best_priority()`.
En primero lugar, esta funcion actua como un algoritmo de seleccion, para lo cual nos fue necesario definir manualmente el puntero NULL, asi como un numero grande para la cantidad de selecciones, y a 0 como la prioridad minima, pues son valores que ibamos a necesitar como punto de partida para la busqueda del "mejor proceso".
Definido esto, se elige entre los procesos listos a aquel que tenga mayor prioridad (cumpliendo con la regla 1: *Si el proceso A tiene mayor prioridad que el proceso B, corre A. (y no B)*); o bien al proceso que menos veces fue elegido de haber una coincidencia en prioridades (obedeciendo la regla 2: *Si dos procesos A y B tienen la misma prioridad, corre el que menos veces fue elegido por el  planificador*).
Esta funcion sera llamada por la funcion scheduler, obtendra al proceso mas optimo y lo pondra en ejecucion.


##### ¿Se puede producir starvation en el nuevo planificador?

Podemos decir que en esta implementacion `SI` puede producirse starvation dado, a que al tener esta implementación en donde a los procesos `iobenchs`tienen una mayor prioridad hacen que en experimentos como `cpubench N &; iobench N &; iobench N &; &;` y `iobench N &; cpubench N &; cpubench N &; cpubench N &;` se nota la sútil puntualidad de que los *cpubenchs* al tener una menor prioridad sufriran, llamemoslo "un olvido" por parte del planificador donde se le da mucha más atención a los *iobenchs*. Para probar esto veremos algunos gráficos de estos experimentos que remarcan está tendencia: 

*`Gráfico del experimento cpubench N &; iobench N &; iobench N &; iobench N &;`*

![Gráfico](https://bitbucket.org/sistop-famaf/so24lab3g24/raw/fdb2599f3aba062fdc25bcd0f7dc9d492f32cc2a/xv6-riscv/Graficos/MLFQ/cpubench_con_varios_io.png)

*`Gráfico del experimento iobenhc N &; cpubench N &; cpubench N &;`*

![Gráfico](https://bitbucket.org/sistop-famaf/so24lab3g24/raw/fdb2599f3aba062fdc25bcd0f7dc9d492f32cc2a/xv6-riscv/Graficos/MLFQ/iobench_con_varios_cpu.png)

Cómo vemos en los gráficos al ir reduciendo los quanto se nota como la performance de los *iobench* se mantiene estable respecto a que los procesos *cpubench* sufren ciertos
cambios en sus métricas respecto a está relegación en la prioridad.

## CONCLUSIONES

Al haber pasado mucho tiempo analizando los distintos planificadores, hemos observado de que si bien, ambos planificadores funcionan de una misma forma tal que si comparamos la cantidad de operaciones que producen cada tipo de proceso y su diferencia es sútil. En cuánto a como cambia en el tiempo que se le da a los procesos, logra verse una diferencia considerable, en donde una buena implementación de la `MLFQ`
logra hacer a que la planificación sea mucho más justa.

Otra cosa a recalcar, que hay ciertas sutilidades a notar de MLFQ respecto a RR que nos parecen importantes recalcarlas:
        
		- El caso donde tenemos procesos de un mismo benchmark (es decir, `cpubench N &; cpubench N &; cpubench N &;`y `iobench N &; iobench N &; iobench N &;`)
          es recalcable de cierta manera que al tener la misma prioridad entre procesos, lo que hará la planificación *MLFQ* es tomar el comportamiento de *Round Robin*
          en el sentido, de que irá seleccionando aquel proceso que menos veces se ejecuto para que se ejecute.
		  
        - El caso donde tenemos la intercalación de procesos *iobench y cpubench* de forma que se llega a los experimentos `cpubench N &; iobench N &;` podemos ver que realmente
          la diferencia entre ambas planificaciones casi no es destacable, dado a que como los procesos `iobench`se la pasan mayormente dormidos dada su naturaleza
          de tener que esperar las operaciones de E/S, y al suceder esto, se termina poniendo en ejecución el proceso `cpubench`. Sin embargo, la diferencia entre las planificaciones
          sucede cuando nos ponemos a ver la prioridad de un *iobench* contra un *cpubench* en la planificación *MLFQ*, donde este con un quanto razonable, se notará que los procesos
          `iobench` son seleccionados una mayor cantidad de veces que los procesos `cpubench`.
		  
        - El caso realmente importante a descatar es el caso `iobench N &; cpubench N &; cpubench N &;` dado a que se cambia a groso modo la visión de la cantidad de veces que se
          seleccionan los procesos. En una planificación *Round Robin* la cantidad de veces que es seleccionado un proceso es apróximadamente la misma cantidad de veces que los demás
          procesos que se ejecutan en paralelo. Ahora el caso resaltnte es cuando se ve la planificación *MLFQ* donde como el iobench tiene más prioridad por sobre los cpubench,
          el scheduler lo elegirá la mitad de las veces, mientras que la otra mitad va alternando entre cada proceso cpubench. Esto es representable con una tabla tal que:

                | iobench    | cpubench0|   cpubench1   |
                | ---------- |:--------:| -------------:|
                |  RUN       | READY    | READY         |
                | BLOCKED    | RUN      | READY         |
                |  RUN       | READY    | READY         |
                | BLOCKED    | READY    | RUN           |
                |  RUN       | READY    | READY         |
                | BLOCKED    | RUN      | READY         |
                |  RUN       | READY    | READY         |
                | BLOCKED    | READY    | RUN           |
                |  RUN       | READY    | READY         |
        
Nos pareció un proyecto, ya dejando de lado las conclusiones puntuales del proyecto, muy extenso pero la verdad como programadores algo muy interesante de hacer ya que se sale del molde
de lo que a uno siempre le presentan y eso nos gusto mucho. Esperamos que este `INFORME`de su agrado y que le haya parecido interesante a modo de lectura.

MUCHISIMAS GRACIAS!!!! 

## Anexo: Puntos estrella

En esta seccion se encuentran las explicaciones de las implementaciones de la mayoria de puntos estrella propuestos por la catedra para este laboratorio.

### (*) *Priority boost*

Para hacer que los procesos vuelvan a tener la maxima prioridad, en primer lugar definimos una consante S = 1, la cual utilizaremos como referencia cuando para el priority boost; luego implementamos la funcion `static void movement_to_max_priority(void)`, que itera entre todos los procesos que esten disponibles, y si su estado es `RUNNABLE` o listo, cambia su prioridad a la maxima. Posteriormente, dentro de la funcion del planificador propiamente dicha, definimos un entrero de 64 bits `time_of_call_scheduler` inicializado en 0, que dentro del bucle general del planificador va a ir incrementandose en 1, y en funcion de si es mayor o igual a S, llamamos a la funcion antes mencionada a que haga efectivamente el priority boost (en cuyo caso asignamos a S nuevamente el valor 0). Caso contrario, simplemente hacemos el cambio de prioridad usual.

### (*) Niveles de prioridad con distintos *quantums*

En este inciso, comenzamos definiendo 3 segmentos de tiempo: q1, q2, y q3, con valores de 30, 50 y 500 respectivamente; asi como un arreglo de quantums cuyos elementos son dichos segmentos.
Luego, en la funcion encargada de cambiar la prioridad de un proceso, agregamos a la guarda que se fija si utilizo todo su quantum el ver si la prioridad de un proceso dado es mayor a 0, en cuyo caso revisamos si su tiempo de ejecucion es mayor al de nuestro arreglo de quantums segun la prioridad de este proceso, y de ser cierto ejecutamos el cambio de prioridad.

### (*) Eliminando *busy waiting*

La solucion que hallamos para eliminar el *busy_waiting* fue mediante una instruccion que permitiera ensamlar condigo en C
`asm`, la cual le dice al compilador que no optimize ni elimine la instruccion, para que se ejecute siempre donde deba. Luego, mediante otra instruccion `wfi` esperamos a que venga una interrupcion.


### (*) *Cputime Syscall*

Encontraran los efectos de este punto aplicados tanto en `iobench.c` como en `cpubench.c`. En primer lugar, debimos agregar debidamente la llamada nueva a todos los archivos de llamadas al sistema del kernel de `xv6-riscv`. En segundo lugar, agregamos un campo dentro de la estructura del proceso que va a guardar los ticks que llevan una instancia de ejecucion del mismo. Este campo va a incrementarse en 1 cada vez que se produzca un timer interrupt. Finalmente, hemos agregado esta llamada al sistema dentro de los archivos mencionados al principio a modo de exposicion, donde se mostrara el tiempo en ticks que le llevo al CPU la implementacion  


