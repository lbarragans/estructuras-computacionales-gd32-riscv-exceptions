# 6. Implementaciones definitivas

| Implementacion | Contexto del trap | Recuperacion |
|---|---|---|
| Referencia | frame del `entry.S` del SDK | manejador C modifica `frame->mepc` |
| Assembly puro | mismo frame oficial | manejador Assembly modifica offset de `mepc` |
| FreeRTOS puro | trap sincrono mientras ejecuta una tarea | manejador C recupera y notifica la tarea |

## Referencia

`Src/main.c`, `Src/trap_demo.c`, `Src/trigger_exception.S` y `Src/systimer.c`
se conservan como control y siguen siendo las fuentes del CMake principal.

## Assembly puro

`Ensamblador_RISCV_Puro/main.S` registra un manejador Assembly mediante la API
del SDK. El handler comprueba causa 2, prueba armada e instruccion comprimida;
registra CSRs y cambia el `mepc` guardado en el frame antes del retorno.

## FreeRTOS puro

`FreeRTOS_Puro/main.c` ejecuta la prueba desde una tarea. La excepcion sigue
siendo un evento sincrono de CPU, no una interrupcion de FreeRTOS. Tras volver
del trap, la tarea valida las metricas y notifica la tarea del LED.
