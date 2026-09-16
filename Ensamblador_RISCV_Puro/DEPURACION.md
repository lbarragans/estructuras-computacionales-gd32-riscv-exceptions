# Depuracion Assembly

Observe `g_exception_count`, `g_last_mcause`, `g_last_mepc`, `g_last_mtval`,
`g_last_instruction`, `g_instruction_length`, `g_recovery_count`,
`g_test_armed`, `g_test_completed` y `g_unexpected_exception`.

El offset `FRAME_MEPC_OFFSET = 48` reproduce el frame de
`Firmware/RISCV/env_Eclipse/entry.S` y `handlers.c` del SDK V1.6.0 usado por la
referencia. Debe volver a comprobarse si cambia el SDK.

Integre `main.S` como unica aplicacion, mantenga startup, handlers y linker del
SDK y no compile simultaneamente los archivos de `Src/`.
