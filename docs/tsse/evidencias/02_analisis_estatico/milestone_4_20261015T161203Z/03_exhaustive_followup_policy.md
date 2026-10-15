# Cppcheck exhaustive follow-up policy

The mandatory command in `00_policy.md` completed, but Cppcheck 2.19.0 emitted `normalCheckLevelMaxBranches` for `main/main.c` and `components/ads1263/src/ads1263.c`. That original output is preserved and is not called clean or exhaustive.

Before interpreting source findings, this append-only follow-up adds `--check-level=exhaustive` to remove that identified analysis-depth limitation. It retains every mandatory option, all 12 declared files, the same defines and include paths, no suppressions, and error exit code 2. A stable pipe-delimited diagnostic template is added only to support complete, auditable disposition.

Exact command:

```sh
cppcheck --language=c --std=c11 --platform=unix64 --library=posix --enable=all --inconclusive --force --check-level=exhaustive --error-exitcode=2 --template='{file}|{line}|{column}|{severity}|{id}|{message}' -D_POSIX_C_SOURCE=200809L -DADS1263_NO_GPIOD=1 -Icomponents/logger/include -Icomponents/acquisition/include -Icomponents/ads1263/include -Icomponents/ads1263/arch/linux/include main/main.c components/logger/src/app_log.c components/acquisition/src/acquisition.c components/ads1263/src/ads1263.c components/ads1263/arch/linux/src/ads1263_spi_linux.c components/ads1263/arch/linux/src/ads1263_gpio_linux.c components/logger/include/app_log.h components/acquisition/include/acquisition.h components/ads1263/include/ads1263.h components/ads1263/include/ads1263_defs.h components/ads1263/arch/linux/include/ads1263_spi_linux.h components/ads1263/arch/linux/include/ads1263_gpio_linux.h
```

The findings summary uses this exhaustive follow-up as the primary Cppcheck result. The first run remains a reproducible diagnostic showing why the follow-up was necessary.
