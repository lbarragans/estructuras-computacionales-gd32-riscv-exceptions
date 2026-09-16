# FreeRTOS puro

La prueba se ejecuta una sola vez desde `test_task`. La excepcion es sincrona:
el nucleo entra al handler dentro del contexto de esa tarea, se corrige el
`mepc` guardado y la tarea continúa después de `c.unimp`.

Una vez verificada la recuperacion, `test_task` notifica a `indicator_task`,
que genera tres pulsos y una pausa. Una validacion fallida produce parpadeo
rapido.

El handler no llama APIs de FreeRTOS mientras procesa el trap. La notificacion
se envia solamente despues de regresar al contexto normal de la tarea.
