#!/usr/bin/env bash
set -u

readonly RUN_DIR="docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z"
readonly RAW_DIR="${RUN_DIR}/raw_analysis_exhaustive"

if [[ -e "${RAW_DIR}" ]]; then
    printf 'Refusing to overwrite existing exhaustive-analysis directory: %s\n' "${RAW_DIR}" >&2
    exit 99
fi
mkdir "${RAW_DIR}"

readonly COMMAND="cppcheck --language=c --std=c11 --platform=unix64 --library=posix --enable=all --inconclusive --force --check-level=exhaustive --error-exitcode=2 --template='{file}|{line}|{column}|{severity}|{id}|{message}' -D_POSIX_C_SOURCE=200809L -DADS1263_NO_GPIOD=1 -Icomponents/logger/include -Icomponents/acquisition/include -Icomponents/ads1263/include -Icomponents/ads1263/arch/linux/include main/main.c components/logger/src/app_log.c components/acquisition/src/acquisition.c components/ads1263/src/ads1263.c components/ads1263/arch/linux/src/ads1263_spi_linux.c components/ads1263/arch/linux/src/ads1263_gpio_linux.c components/logger/include/app_log.h components/acquisition/include/acquisition.h components/ads1263/include/ads1263.h components/ads1263/include/ads1263_defs.h components/ads1263/arch/linux/include/ads1263_spi_linux.h components/ads1263/arch/linux/include/ads1263_gpio_linux.h"

printf '%s\n' "${COMMAND}" > "${RAW_DIR}/001_cppcheck_exhaustive_no_gpiod.command.txt"
date -u +%Y-%m-%dT%H:%M:%SZ > "${RAW_DIR}/001_cppcheck_exhaustive_no_gpiod.started_utc.txt"
bash -o pipefail -c "${COMMAND}" > "${RAW_DIR}/001_cppcheck_exhaustive_no_gpiod.stdout.txt" 2> "${RAW_DIR}/001_cppcheck_exhaustive_no_gpiod.stderr.txt"
rc=$?
printf '%s\n' "${rc}" > "${RAW_DIR}/001_cppcheck_exhaustive_no_gpiod.exitcode.txt"
date -u +%Y-%m-%dT%H:%M:%SZ > "${RAW_DIR}/001_cppcheck_exhaustive_no_gpiod.finished_utc.txt"

exit 0
