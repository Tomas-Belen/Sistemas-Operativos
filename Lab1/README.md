# README #

En el siguiente repositorio encontraran los laboratorios correspondientes a la catedra SISTEMAS OPERATIVOS 2024, hechos en conjunto por los alumnos:
-Belen, Tomás.
-Ferreyra, Joaquín Ignacio.
-Figueroa, Jeremías.
-Ramírez, Alexis Diego.

La consigna fue implementar un shell al estilo bash *mybash*, que funcione de manera similar al conocido bash de linux. El trabajo se realizo sobre varios modulos, cuyas explicaciones estan a continuacion.

### Sobre el modulo `command.c`:
En esta seccion se implementaron 2 TADs: *scommand* y *pipeline*, siendo el primero el que captura el funcionamiento de un comando que se ingresa al shell, y el segundo una conexion entre varios comandos.

De las implementaciones que teniamos disponibles de GNOME, nosotros utilizamos GSList, pues al tratarse de una lista enlazada simple nos parecio lo mas adecuado para manejar el comportamiento de los comandos que se ingresan en la consola, agregando elementos al inicio e iterando sobre ellos.

Veran que varias de las funciones que nos proporciona este TAD estan utilizadas en estos modulos: por ejemplo en pop_front estan gslistnth_data cuyo retorno es el argumento de gslist_remove para el scommand; o gslits_append para agregar el comando al ultimo del pipeline.
Utilicé g_slist_nth_data porque necesitabamos acceder a los datos almacenados en nodos específicos, por ejemplo para obtener el nodo actual durante el procesamiento de argumentos de un comando. 


### Sobre el modulo: `parsing.c` 
Esta seccion era la encargada de leer la secuencia de comandos e ir armando una instancia *pipeline*.
En este caso fue un agregado interesante utilizar las funciones de un tad implementado de manera opaca en este modulo. La implementacion la hicimos: guardando el argumento del pipe en un string con `parser_next_argument` para definir el tipo de argumento que recibiamos, y en funcion de lo que nos devolviera lo tratamos como una redireccion o un comando con las funciones que teniamos en el modulo command. Luego en el parsing del pipeline utilizamos esta funcion para procesar el pipeline entero mientras tengamos otra instancia que analizar con `parser_op_pipe`.

### Sobre el modulo `builtin.c`
Aqui nos encargamos de implementar funcionalidades de los TADs antes implementados,  como `builtin_is_internal` que chequea de si se ingreso un comando interno; `builtin_alone` para revisar si se trata de un solo comando; y `builtin_run`, que se encarga de ejecutar cada uno de los comandos internos definidos. Estos utlimos estan modularizados en 3 funciones aparte que ejecutaban "cd"/"help"/"exit".
En particular, nos ocupamos de emular el comportamiendo de cd lo mejor posible, cambiando al directorio raiz si no recibe ningun argumento adicional. Asi como los distintos casos de error como no tener permiso al directorio, que no exista, etc.
 

### Sobre el modulo `execute.c`:
En este punto utilizamos varias llamadas al sistema, asi como funciones de los modulos antes mencionados, para ejecutar los comandos en un entorno aislado del intérprete de línea de comandos.
Respecto a las syscalls implementadas, estas fueron: *dup2* para las redirecciones de entrada/salida,  *execvp* para la ejecucion de un comando, y *fork* para armar la instancia del pipeline utilizando a los procesos padre e hijo.. 

Para mas informacion, podran examinar el codigo escrito, asi como los comentarios que hemos dejado para mayor entendimiento

Esperamos sea de su agrado.