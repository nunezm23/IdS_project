# Tests host con Ceedling

La configuracion compila directamente `components/ads1263/src/ads1263.c`. CMock
reemplaza los adapters SPI/GPIO Linux y el linker redirige `nanosleep` al wrapper
sin espera de `test/ceedling/support/wait_wrapper.c`. No se enlazan objetos de los
adapters Linux ni se accede a dispositivos fisicos.

## Comandos no interactivos

Ejecutar desde la raiz del repositorio:

```sh
ceedling clobber
ceedling test:all
ceedling clobber --mixin=test/ceedling/mixins/coverage.yml
ceedling gcov:all --mixin=test/ceedling/mixins/coverage.yml
ceedling clobber --mixin=test/ceedling/mixins/sanitizer.yml
ceedling test:all --mixin=test/ceedling/mixins/sanitizer.yml
```

Los perfiles escriben respectivamente en `build/ceedling/normal`,
`build/ceedling/coverage` y `build/ceedling/sanitizer`. Cada comando retorna cero
si completa correctamente y un valor distinto de cero ante error de build o test.
El perfil de cobertura filtra el denominador a
`components/ads1263/src/ads1263.c`; tests, runners, mocks, framework y soporte no
forman parte de las metricas de produccion. No se aplica un umbral de cobertura.
