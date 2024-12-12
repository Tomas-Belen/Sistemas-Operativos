# README LAB_1 #


Bienvenid@s, en este directorio se encuentra el trabajo práctico referido a la consigna presentada en la cátedra de Sistemas Operativos, en el marco del 1º proyecto de la materia.
El proyecto, elaborado por Belén Tomás, Ferreyra Joaquín Ignacio, Figueroa Jeremías, y Ramírez Alexis Diego, presenta en los distintos archivos el código necesario para ejecutar un shell al estilo de Bash (Bourne Again Shell).

Además de los archivos entregados, como los headers (command.h, parsing.h, execute.h, builtin.h y strextra.h), el parser (parser.h, lexer.o, parser.o), y los tests, se encuentran los realizados y/o completados por el grupo, tales como builtin.c, command.c, execute.c, mybash.c, parsing.c, strextra.c y este archivo readme.txt.

Dentro de estos se encuentran comentadas distintas soluciones o criterios de diseño que fueron tomados a lo largo del desarrollo del bash. Algunos criterios a destacar son los siguientes:

(<com1> | ): En el Bash de Linux, este caso hace que el sistema espere un segundo comando. En nuestra bash, lo tratamos como un error de sintaxis, ya que consideramos más práctico manejarlo de esta forma.

<com1> | &: Este y su variación (com1 & |) se interpretan como un error de sintaxis, tal como sucede en el Bash de Linux.

(<com1> | builtin): En la consola de Linux, al ejecutar (cd | ls), solo se ejecuta ls. Para denotar el funcionamiento de los comandos internos en nuestra bash, optamos por que solo se ejecuten los built-in.

(<com1> < ): En este caso, si se ejecuta, se lanzará un mensaje de error solicitando al usuario el uso correcto de las redirecciones (tanto de entrada como de salida).

(<com1> | <com2> >): Este caso se maneja de la misma forma que en el Bash de Linux, con el agregado de que se interpreta como un error sintáctico del comando.

