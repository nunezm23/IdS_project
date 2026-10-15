#!/usr/bin/env bash
set -u

readonly RUN_DIR="docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z"
readonly RAW_DIR="${RUN_DIR}/raw_analysis"

if [[ -e "${RAW_DIR}" ]]; then
    printf 'Refusing to overwrite existing analysis directory: %s\n' "${RAW_DIR}" >&2
    exit 99
fi
mkdir "${RAW_DIR}"

record() {
    local id="$1"
    local command="$2"
    local prefix="${RAW_DIR}/${id}"
    local rc

    printf '%s\n' "${command}" > "${prefix}.command.txt"
    date -u +%Y-%m-%dT%H:%M:%SZ > "${prefix}.started_utc.txt"
    bash -o pipefail -c "${command}" > "${prefix}.stdout.txt" 2> "${prefix}.stderr.txt"
    rc=$?
    printf '%s\n' "${rc}" > "${prefix}.exitcode.txt"
    date -u +%Y-%m-%dT%H:%M:%SZ > "${prefix}.finished_utc.txt"
}

readonly SOURCES='main/main.c components/logger/src/app_log.c components/acquisition/src/acquisition.c components/ads1263/src/ads1263.c components/ads1263/arch/linux/src/ads1263_spi_linux.c components/ads1263/arch/linux/src/ads1263_gpio_linux.c components/logger/include/app_log.h components/acquisition/include/acquisition.h components/ads1263/include/ads1263.h components/ads1263/include/ads1263_defs.h components/ads1263/arch/linux/include/ads1263_spi_linux.h components/ads1263/arch/linux/include/ads1263_gpio_linux.h'
readonly C_SOURCES='main/main.c components/logger/src/app_log.c components/acquisition/src/acquisition.c components/ads1263/src/ads1263.c components/ads1263/arch/linux/src/ads1263_spi_linux.c components/ads1263/arch/linux/src/ads1263_gpio_linux.c'
readonly PROJECT_INCLUDES='-Icomponents/logger/include -Icomponents/acquisition/include -Icomponents/ads1263/include -Icomponents/ads1263/arch/linux/include'
readonly CPPCHECK_PROFILE='--language=c --std=c11 --platform=unix64 --library=posix --enable=all --inconclusive --force --error-exitcode=2'
readonly GCC15_PROFILE='-std=c11 -fsyntax-only -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow -Wformat=2 -Wundef -Wnull-dereference -Wdouble-promotion -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wvla -fanalyzer'

record 001_cppcheck_no_gpiod "cppcheck ${CPPCHECK_PROFILE} -D_POSIX_C_SOURCE=200809L -DADS1263_NO_GPIOD=1 ${PROJECT_INCLUDES} ${SOURCES}"

printf '%s\n' 'BLOCKED' > "${RAW_DIR}/002_cppcheck_libgpiod_v2.status.txt"
printf '%s\n' 'pkg-config is unavailable (exit 127), the libgpiod >= 2.0 package gate cannot pass (exit 127), and the gpiod.h syntax probe fails (exit 1). See raw_metadata/015_*, 017_*, and 019_*.' > "${RAW_DIR}/002_cppcheck_libgpiod_v2.reason.txt"
printf '%s\n' "cppcheck ${CPPCHECK_PROFILE} -D_POSIX_C_SOURCE=200809L -DADS1263_NO_GPIOD=0 \$(pkg-config --cflags libgpiod) ${PROJECT_INCLUDES} ${SOURCES}" > "${RAW_DIR}/002_cppcheck_libgpiod_v2.command_not_run.txt"

if command -v gcc-15 >/dev/null 2>&1; then
    record 003_gcc15_no_gpiod "gcc-15 ${GCC15_PROFILE} -D_POSIX_C_SOURCE=200809L -DADS1263_NO_GPIOD=1 ${PROJECT_INCLUDES} ${C_SOURCES}"
else
    printf '%s\n' 'BLOCKED' > "${RAW_DIR}/003_gcc15_no_gpiod.status.txt"
    printf '%s\n' 'gcc-15 is unavailable; informational diagnostics were not run.' > "${RAW_DIR}/003_gcc15_no_gpiod.reason.txt"
fi

printf '%s\n' 'BLOCKED' > "${RAW_DIR}/004_gcc15_libgpiod_v2.status.txt"
printf '%s\n' 'The shared libgpiod-v2 pkg-config/header gate failed. See raw_metadata/015_*, 017_*, and 019_*.' > "${RAW_DIR}/004_gcc15_libgpiod_v2.reason.txt"
printf '%s\n' "gcc-15 ${GCC15_PROFILE} -D_POSIX_C_SOURCE=200809L -DADS1263_NO_GPIOD=0 \$(pkg-config --cflags libgpiod) ${PROJECT_INCLUDES} ${C_SOURCES}" > "${RAW_DIR}/004_gcc15_libgpiod_v2.command_not_run.txt"

record 005_source_sha256_after 'sha256sum main/main.c components/logger/src/app_log.c components/acquisition/src/acquisition.c components/ads1263/src/ads1263.c components/ads1263/arch/linux/src/ads1263_spi_linux.c components/ads1263/arch/linux/src/ads1263_gpio_linux.c components/logger/include/app_log.h components/acquisition/include/acquisition.h components/ads1263/include/ads1263.h components/ads1263/include/ads1263_defs.h components/ads1263/arch/linux/include/ads1263_spi_linux.h components/ads1263/arch/linux/include/ads1263_gpio_linux.h'
record 006_production_diff_after 'git diff -- components main Makefile && git diff --cached -- components main Makefile'

exit 0
