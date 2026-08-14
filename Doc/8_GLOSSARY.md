# 8. Glosario

| Termino | Definicion en este ejercicio |
| --- | --- |
| ABI | Reglas para argumentos, retornos y uso de registros entre funciones |
| CSR | Registro de control y estado del procesador |
| `c.unimp` | Codificacion comprimida reservada para provocar instruccion ilegal |
| Contexto | Registros necesarios para reanudar correctamente la ejecucion |
| ECLIC | Controlador de interrupciones local mejorado del nucleo Nuclei |
| ELF | Ejecutable con codigo, direcciones, secciones y simbolos de debug |
| Excepcion | Trap sincrono causado por la instruccion actual |
| Handler | Funcion que atiende una causa determinada |
| Halfword | Unidad de 16 bits, equivalente a dos bytes |
| Interrupcion | Trap asincrono solicitado por hardware |
| ISR | Rutina de servicio de interrupcion |
| JTAG | Interfaz usada para programar y depurar la placa |
| Linker script | Archivo que ubica secciones en Flash y RAM |
| `mcause` | CSR que codifica tipo y causa del trap |
| `mepc` | CSR con la direccion de retorno del trap |
| `mret` | Instruccion que retorna desde modo maquina |
| `mstatus` | CSR con estado privilegiado e interrupciones |
| `mtval` | CSR con informacion adicional del trap |
| OpenOCD | Servidor que conecta GDB con el depurador fisico |
| PC | Program Counter; direccion de la instruccion actual |
| Pila | Memoria LIFO usada para llamadas y contexto |
| RISC-V C | Extension de instrucciones comprimidas de 16 bits |
| `sp` | Registro puntero de pila; se entrega como `a1` al handler |
| Startup | Codigo que prepara la CPU antes de ejecutar `main` |
| SysTimer | Temporizador del nucleo usado como base de tiempo |
| Trap | Transferencia de control por excepcion o interrupcion |
| Vector | Direccion de entrada asociada al manejo de eventos |
| `volatile` | Calificador que obliga accesos observables a memoria |

## Registros ABI mencionados

| Registro | Nombre ABI | Uso habitual |
| --- | --- | --- |
| `x1` | `ra` | Direccion de retorno |
| `x2` | `sp` | Puntero de pila |
| `x4` | `tp` | Puntero de hilo |
| `x5–x7` | `t0–t2` | Temporales |
| `x10–x17` | `a0–a7` | Argumentos y valores de retorno |
| `x28–x31` | `t3–t6` | Temporales adicionales |

Durante la entrada de excepcion, `a0` transporta `mcause` y `a1` transporta
el puntero al marco guardado.
