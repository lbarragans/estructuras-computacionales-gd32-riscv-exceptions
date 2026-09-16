# Integracion funcional con el MSDK oficial

Esta variante usa el FreeRTOS, port Nuclei/ECLIC, heap, tick, startup y linker
ya integrados y probados por GigaDevice en `GD32VW55x_RELEASE_V1.0.3g`.

1. Instale o extraiga el SDK en `C:\\GD32\\GD32VW55x_RELEASE_V1.0.3g`.
2. Configure `GD32_MSDK_ROOT`, `NUCLEI_TOOLCHAIN_DIR` y `OPENOCD_ROOT` en
   `tools/local_config.ps1`.
3. Ejecute la tarea de VS Code **Build + Flash FreeRTOS**. El script copia
   `main.c` y `app_cfg.h` a `MSDK/app`, conserva una copia de seguridad y fuerza
   la recompilación.
4. El MSDK compila MBL y la aplicación, y genera `scripts/images/image-all.bin`.
5. OpenOCD programa la imagen completa en `0x08000000` mediante WCH-Link
   CMSIS-DAP (`usb_bulk`, VID:PID `1a86:8012`, JTAG a 50 kHz).
6. Reinicie y observe PC13 y los símbolos `g_*` indicados por el ejercicio.

`main.c` llama `platform_init()`, crea las tareas/objetos FreeRTOS y entrega
el control a `sys_os_start()`. No descargue otro kernel ni mezcle un port
RISC-V generico con el ECLIC de este dispositivo.

La compilacion en el MSDK elimina la antigua dependencia pendiente de un port
externo. Esta ruta fue validada físicamente en la placa real.
