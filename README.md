# Ejercicio 08 - Excepciones RISC-V y recuperacion controlada

**Curso:** Estructuras Computacionales

**Autora:** Laura Daniela Barragan Silva

**Plataforma:** GD32VW553HMQ6/HMQ7, RISC-V RV32

## Objetivo

Ejecutar deliberadamente `c.unimp`, capturar la excepcion de instruccion
ilegal, conservar `mcause`, `mepc` y `mtval`, avanzar dos bytes el punto de
retorno y continuar de manera controlada.

## Tres caminos

| Camino | Archivos | Concepto |
|---|---|---|
| Referencia | `Src/main.c`, `Src/trap_demo.c`, `Src/trigger_exception.S` | manejador C integrado al frame del SDK |
| RISC-V puro | `Ensamblador_RISCV_Puro/main.S` | disparo, diagnostico y correccion del frame en Assembly |
| FreeRTOS puro | `FreeRTOS_Puro/main.c` | tarea de prueba y notificacion tras recuperar el trap |

La referencia original continúa seleccionada en el `CMakeLists.txt` de la
raiz. Las rutas alternativas son aplicaciones independientes y no se compilan
simultaneamente con `Src/`.

## Resultado observable

| Patron PC13 | Interpretacion |
|---|---|
| Tres destellos y pausa | excepcion esperada y recuperacion correcta |
| Parpadeo rapido | causa, longitud o recuperacion inesperada |

## Evidencia esperada

| Variable | Valor esperado |
|---|---:|
| `g_exception_count` | `1` |
| `g_last_mcause & 0xFFF` | `2` |
| `g_last_instruction` | `0x0000` |
| `g_instruction_length` | `2` |
| `g_recovery_count` | `1` |
| `g_test_armed` | `0` |
| `g_test_completed` | `1` |
| `g_unexpected_exception` | `0` |

## Estructura

```text
08_RISCV_Exception_Recovery/
├── Src/                         # referencia original C + Assembly
├── Inc/
├── Ensamblador_RISCV_Puro/      # aplicacion completa en Assembly
├── FreeRTOS_Puro/               # tarea y notificacion FreeRTOS
├── Doc/
├── CMakeLists.txt
└── README.md
```

## Documentacion

Los documentos `Doc/1_SETUP.md` a `Doc/8_GLOSSARY.md` conservan la ruta
didactica original. `Doc/6_VARIANTES_DEL_EJERCICIO.md` compara los tres caminos
y `Doc/7_PLAN_DE_VALIDACION.md` define las pruebas comunes.

## Seguridad

Saltar una instruccion solo es valido en esta practica porque la causa, la
codificacion, la longitud y el punto de retorno son conocidos. Una excepcion
desconocida debe llevar el sistema a una politica segura; nunca se debe omitir
automaticamente.

## Estado

| Implementacion | Estado |
|---|---|
| Referencia | validada físicamente: tres destellos y pausa |
| Assembly puro | validada físicamente: tres destellos y pausa |
| FreeRTOS puro | validada físicamente con MSDK V1.0.3g: tres destellos y pausa |

## Ejecutar las variantes

Referencia original por JTAG/OpenOCD:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\\tools\\build_variant.ps1 -Variant original -Flash
```

Assembly puro por JTAG/OpenOCD:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\\tools\\build_variant.ps1 -Variant assembly -Flash
```

FreeRTOS por medio del MSDK oficial:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\\tools\\build_freertos.ps1 -Clean -Flash
```

Desde VS Code use **Terminal > Run Task** y elija `Build + Flash Original`,
`Build + Flash Assembly` o `Build + Flash FreeRTOS`. FreeRTOS se integra
automaticamente con el MSDK V1.0.3g.

## Guía central de ejecución

La [guía central GD32VW553](https://github.com/lbarragans/gd32vw553-vscode-cmake-guide)
explica VS Code, JTAG, depuración de Assembly e integración FreeRTOS.
