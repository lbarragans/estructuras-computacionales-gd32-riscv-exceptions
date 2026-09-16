# Integracion FreeRTOS

1. Añadir kernel, port RISC-V, `heap_x.c` y `FreeRTOSConfig.h`.
2. Conservar `entry.S`, `handlers.c`, startup y linker script del SDK exacto.
3. Sustituir las fuentes de aplicacion de `Src/` por este `main.c`.
4. Confirmar que el port no reemplaza de forma incompatible el vector de traps.
5. Verificar el orden y offsets del frame antes de probar la recuperacion.
6. Ejecutar `Doc/7_PLAN_DE_VALIDACION.md` con breakpoints y placa.

No combinar esta aplicacion con `Src/main.c` ni `Src/trap_demo.c`.
