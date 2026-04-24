# N-Queens GA Project Context & Workflow

Este archivo define las reglas, el contexto y los comandos interactivos (skills) que utilizaremos para guiar el desarrollo de este proyecto.

## Comandos del Workflow

- **/load-project**
  *Uso:* Al iniciar cada sesión de trabajo.
  *Acción:* Lee `CLAUDE.md`, `SPEC.md` y `WORKLOG.md` en ese orden. Luego, devuelve: 1) Resumen del estado actual. 2) Qué estaba en progreso. 3) Próximo paso recomendado. (Sin generar código aún).

- **/review-code**
  *Uso:* Después de generar nuevo código, antes de aceptarlo.
  *Acción:* Revisa el código como senior developer estricto. Evalúa: bugs/edge cases, convenciones, simplificación, manejo de errores y fallos de producción.

- **/fix-bug [descripción del bug]**
  *Uso:* Cuando algo se rompe.
  *Acción:* Diagnostica la causa probable, lista los archivos involucrados y propone el fix mínimo necesario (sin tocar nada más). Muestra el diff exacto.

- **/update-worklog**
  *Uso:* Al terminar cada sesión.
  *Acción:* Actualiza `WORKLOG.md` con la fecha de hoy, lo implementado, lo en progreso, los bloqueos y los próximos pasos prioritarios.

- **/add-reference [tema]**
  *Uso:* Al introducir un nuevo patrón o librería.
  *Acción:* Agrega a `REFERENCES.md` un snippet funcional que incluye cuándo usarlo, el código mínimo, y los gotchas importantes.

- **/update-spec**
  *Uso:* Cuando una feature cambia o se añade al alcance.
  *Acción:* Actualiza `SPEC.md`, marcando features implementadas, agregando nuevas tareas, y actualizando modelos si es necesario.

## Sub-agentes

- **/agent security**: Revisa el código enfocado en vulnerabilidades (inyecciones, auth, secrets, inputs). Da reporte (CRÍTICO/ALTO/MEDIO/BAJO).
- **/agent dba**: Analiza bases de datos o en este caso, la estructura y almacenamiento de datos de las corridas genéticas buscando optimización y eficiencia.
- **/agent qa**: Como QA Engineer. Genera casos de prueba (Vitest/Jest/Pytest), edge cases y un test de integración para el happy path.
- **/agent frontend**: Revisa componentes visuales/UI. Analiza estado, re-renders, UX (estados de loading/error) y modularidad.
