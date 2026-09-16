# FreeRTOS puro

La prueba se ejecuta una sola vez desde `test_task`. La excepcion es sincrona:
el nucleo entra a `exc_entry` dentro del contexto de esa tarea, se comprueba
`mcause`, se avanza dos bytes el CSR `mepc` y la tarea continúa después de
`c.unimp` mediante `mret`.

Una vez verificada la recuperacion, `test_task` notifica a `indicator_task`,
que genera tres pulsos y una pausa. Una validacion fallida produce parpadeo
rapido.

La entrada conserva los registros temporales que modifica y no llama APIs de
FreeRTOS mientras procesa el trap. La notificacion se envia solamente despues
de regresar al contexto normal de la tarea.

Esta implementación fue validada físicamente en GD32VW553: tres destellos
cortos y una pausa repetitiva.
