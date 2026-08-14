# 4. Laboratorio de depuracion en VS Code

## Objetivo

Observar la excepcion en cuatro momentos: antes del disparo, dentro del
manejador, al corregir el retorno y despues de `mret`.

## Preparacion

1. Ejecute `3. Compilar GD32 (Debug)`.
2. Ejecute una vez `6. Preparar depuracion`.
3. Abra **Run and Debug**.
4. Seleccione `Debug GD32VW553 - Cortex Debug`.

La tarea 6 crea `.vscode/launch.json` con las rutas locales de GDB y OpenOCD.
No se publica porque esas rutas cambian entre equipos.

## Breakpoints recomendados

No dependa del numero de linea; busque la instruccion indicada.

| Orden | Archivo | Breakpoint | Que demuestra |
| ---: | --- | --- | --- |
| 1 | `main.c` | `g_test_armed = 1U;` | La prueba esta preparada |
| 2 | `trap_demo.c` | inicio de `illegal_instruction_handler` | La CPU transfirio el control |
| 3 | `trap_demo.c` | `frame->mepc += instruction_length;` | Se acepto la recuperacion |
| 4 | `main.c` | `g_test_completed = 1U;` | `mret` regreso al flujo normal |

No coloque breakpoints en `eclic_mtip_handler`: se ejecuta cada milisegundo.

## Expresiones Watch

Agregue una por una:

```text
g_exception_count
g_last_mcause
g_last_mcause & 0xFFF
g_last_mepc
g_last_mtval
g_last_instruction
g_instruction_length
g_recovery_count
g_test_armed
g_test_completed
g_unexpected_exception
g_systimer_ticks
g_background_iterations
```

Use formato hexadecimal para `mcause`, `mepc`, `mtval` e instruccion.

## Recorrido guiado

### Estacion 1 — antes de la excepcion

Detengase en `g_test_armed = 1U` y avance una linea.

| Observacion | Esperado |
| --- | ---: |
| `g_test_armed` | 1 |
| `g_exception_count` | 0 |
| `g_test_completed` | 0 |

La siguiente llamada entra en `trigger_illegal_instruction()`.

### Estacion 2 — entrada al manejador

Continue hasta `illegal_instruction_handler`. En **Call Stack** debe aparecer la
ruta asociada a la excepcion. Los parametros locales son:

- `mcause`: causa entregada en `a0`;
- `sp`: direccion del marco entregada en `a1`;
- `frame`: interpretacion estructurada de esa memoria.

Avance con Step Over hasta completar las asignaciones. Entonces verifique:

```text
g_last_mcause & 0xFFF = 2
g_last_instruction = 0x0000
g_instruction_length = 2
```

### Estacion 3 — correccion del retorno

En `frame->mepc += instruction_length`, despliegue `frame` y anote el valor de
`mepc`. Ejecute Step Over y confirme que aumenta exactamente `0x2`.

Tambien deben cambiar:

```text
g_recovery_count: 0 -> 1
g_test_armed:      1 -> 0
```

### Estacion 4 — regreso a `main`

Pulse Continue hasta `g_test_completed = 1U`. Haber alcanzado esta linea es la
prueba mas directa de que `entry.S` restauro el contexto y `mret` reanudo la
ejecucion despues de `c.unimp`.

Avance y compruebe:

```text
g_test_completed = 1
g_unexpected_exception = 0
```

## Ver el ensamblador

Abra `build/debug/GD32VW55x.lst` y busque
`trigger_illegal_instruction`. Debe reconocer:

```text
0000    instruccion ilegal comprimida
ret     retorno de la funcion
```

Compare la direccion de `0000` con `g_last_mepc`. Esta comparacion une el
codigo fuente, el ELF, la direccion de Flash y el diagnostico de la CPU.

## Registros y memoria

En el panel **Registers**, localice `mepc` y `mcause` si el adaptador los
expone. En **Memory**, examine 16 bytes desde `g_last_mepc`; los primeros dos
deben corresponder a `00 00` en little-endian.

## Evidencia para el informe

Capture:

1. Watch dentro del manejador con causa 2 y longitud 2.
2. `frame->mepc` antes y despues de sumar 2.
3. Watch en `main` con `g_test_completed=1`.
4. Fragmento del `.lst` donde aparece `0000`.
5. Descripcion del patron observado en PC13.

Cada captura debe incluir una frase que explique **que demuestra**, no solo la
imagen.
