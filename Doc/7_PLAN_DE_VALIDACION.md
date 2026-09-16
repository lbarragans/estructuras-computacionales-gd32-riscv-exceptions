# 7. Plan de validacion

## Comportamiento comun

- disparar exactamente una instruccion `c.unimp` (`0x0000`);
- capturar causa 2, `mepc`, `mtval` e instruccion;
- reconocer longitud de dos bytes;
- avanzar solamente el `mepc` de la prueba armada;
- continuar despues del trap y mostrar tres pulsos con pausa;
- tratar cualquier otra condicion como error.

## Referencia

Compilar con `build-debug`. Usar breakpoints antes del disparo, dentro del
manejador y despues del retorno. Comparar Watch y desensamblado con la tabla del
README.

## Assembly

Integrar `main.S` como unica aplicacion. Verificar el offset 48 del `mepc` en el
frame de la version exacta del SDK, la lectura de CSRs, el cambio de dos bytes y
el `mret` ejecutado por la infraestructura oficial.

## FreeRTOS

Integrar kernel, port, heap y configuracion. Confirmar que la tarea de prueba
se reanuda despues del trap, completa una sola prueba y notifica al indicador.

## Criterio de cierre

Conservar ELF/LST, valores de los CSR, frame antes/despues y video o registro
del LED. No declarar las alternativas validadas sin compilacion y placa.
