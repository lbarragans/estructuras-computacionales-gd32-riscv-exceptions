# Ensamblador RISC-V puro

`main.S` implementa la aplicacion completa sin C propio:

- GPIO, SysTimer y ECLIC;
- registro del handler mediante la infraestructura oficial;
- disparo controlado de `c.unimp`;
- captura de `mcause`, `mepc`, `mtval` e instruccion;
- validacion de causa, longitud y prueba armada;
- correccion del `mepc` guardado en el frame;
- patrones de exito y error sobre PC13.

La llamada `Exception_Register_EXC` y el retorno mediante `entry.S` son
infraestructura del SDK, no codigo C de la aplicacion.
