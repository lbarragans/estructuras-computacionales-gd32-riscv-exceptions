# 1. Preparacion del entorno

## Objetivo de esta etapa

Dejar una cadena reproducible en Windows:

```mermaid
flowchart LR
    VS["VS Code"] --> CM["CMake"]
    CM --> NJ["Ninja"]
    NJ --> GCC["Nuclei GCC"]
    GCC --> ELF["ELF RISC-V"]
    ELF --> OCD["OpenOCD"]
    OCD --> MCU["GD32VW553"]
```

Cada elemento cumple una funcion distinta. VS Code organiza el trabajo; CMake
describe el proyecto; Ninja ejecuta la compilacion; GCC produce codigo RISC-V;
OpenOCD programa y depura la placa.

## Hardware

- Placa GD32VW553HMQ6 o GD32VW553HMQ7.
- Depurador compatible con CMSIS-DAP/WCH-Link.
- Conexion JTAG entre depurador y placa.
- Cable USB de datos.

El ejercicio solo utiliza el LED activo en bajo conectado a PC13. No requiere
protoboard ni componentes externos.

## Software

| Componente | Funcion | Comprobacion |
| --- | --- | --- |
| Visual Studio Code | Edicion, tareas y depuracion | Abre la carpeta del proyecto |
| CMake 3.20+ | Genera el sistema de build | Tarea 1 |
| Ninja | Ejecuta las reglas de compilacion | Tarea 1 |
| Nuclei RISC-V GCC | Compila C y ensamblador RV32 | Tarea 1 |
| Nuclei RISC-V GDB | Controla la CPU durante debug | Tarea 1 |
| OpenOCD | Comunica GDB con WCH-Link | Tarea 1 |
| SDK GD32VW55x V1.6.0 | Startup, drivers y linker script | Tarea 1 |

## Extensiones de VS Code

Al abrir el repositorio, instale las recomendaciones de
`.vscode/extensions.json`:

- **C/C++**: navegacion, simbolos, Watch y soporte de depuracion.
- **CMake Tools**: reconocimiento de proyectos CMake.
- **Cortex-Debug**: integracion de GDB y OpenOCD. Aunque su nombre diga
  Cortex, permite usar el GDB RISC-V configurado por el proyecto.

No seleccione un kit de compilador de escritorio. El toolchain correcto se
define en `cmake/toolchain-riscv.cmake`.

## Crear la configuracion privada

1. En el explorador de VS Code abra `tools/`.
2. Duplique `local_config.example.ps1`.
3. Cambie el nombre de la copia a `local_config.ps1`.
4. Sustituya las tres rutas de ejemplo.

```powershell
$GD32_SDK_ROOT = "C:/ruta/GD32VW55x_Firmware_Library_V1.6.0"
$NUCLEI_TOOLCHAIN_DIR = "C:/ruta/NucleiRISCVGCC/bin"
$OPENOCD_ROOT = "C:/ruta/OpenOCD/xpack-openocd-0.11.0-3"
```

Use `/` o `\` de forma consistente. `OPENOCD_ROOT` debe contener las carpetas
`bin` y `scripts`; `NUCLEI_TOOLCHAIN_DIR` debe contener
`riscv-nuclei-elf-gcc.exe`.

## Por que este archivo no se publica

Las rutas cambian entre computadores y pueden revelar nombres de usuario. Por
eso `.gitignore` excluye:

```text
tools/local_config.ps1
.vscode/launch.json
build/
```

Las plantillas publicas permiten reconstruirlos sin almacenar informacion
personal.

## Verificacion desde VS Code

Abra `Terminal > Run Task` y ejecute `1. Verificar entorno GD32`. Debe aparecer
`[OK]` para CMake, Ninja, GCC, GDB, OpenOCD, scripts, archivo de sistema,
linker script y soporte de placa.

No continue si algun elemento muestra error: la tarea indica exactamente que
ruta debe corregirse.

## Lista de control

- [ ] La carpeta 08 esta abierta como raiz del workspace.
- [ ] VS Code confia en la carpeta.
- [ ] Las extensiones recomendadas estan instaladas.
- [ ] Existe `tools/local_config.ps1`.
- [ ] La tarea 1 termina con “Entorno listo”.
- [ ] La placa y el depurador estan conectados.
