#!/usr/bin/env bash
set -u

readonly RUN_DIR="docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z"
readonly RAW_DIR="${RUN_DIR}/raw_metadata"

if [[ -e "${RAW_DIR}" ]]; then
    printf 'Refusing to overwrite existing metadata directory: %s\n' "${RAW_DIR}" >&2
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

record 001_capture_clock 'date -u +%Y-%m-%dT%H:%M:%SZ'
record 002_git_branch 'git branch --show-current'
record 003_git_head 'git rev-parse HEAD'
record 004_git_status 'git status --short --branch --untracked-files=all'
record 005_git_worktree_diff 'git diff -- components main Makefile'
record 006_git_index_diff 'git diff --cached -- components main Makefile'
record 007_uname 'uname -a'
record 008_os_release 'if test -r /etc/os-release; then printf "%s\n" "$(< /etc/os-release)"; else printf "%s\n" "/etc/os-release unavailable"; exit 1; fi'
record 009_architecture 'uname -m && getconf LONG_BIT'
record 010_cppcheck_path 'command -v cppcheck'
record 011_cppcheck_version 'cppcheck --version'
record 012_gcc15_path 'command -v gcc-15'
record 013_gcc15_version 'gcc-15 --version'
record 014_gcc_default_version 'gcc --version'
record 015_pkg_config_version 'pkg-config --version'
record 016_libgpiod_version 'pkg-config --modversion libgpiod'
record 017_libgpiod_v2_pkg_gate 'pkg-config --exists "libgpiod >= 2.0"'
record 018_libgpiod_cflags 'pkg-config --cflags libgpiod'
record 019_libgpiod_header_gate 'printf "#include <gpiod.h>\n" | gcc -std=c11 -D_POSIX_C_SOURCE=200809L $(pkg-config --cflags libgpiod 2>/dev/null) -fsyntax-only -x c -'
record 020_source_inventory 'printf "%s\n" main/main.c components/logger/src/app_log.c components/acquisition/src/acquisition.c components/ads1263/src/ads1263.c components/ads1263/arch/linux/src/ads1263_spi_linux.c components/ads1263/arch/linux/src/ads1263_gpio_linux.c components/logger/include/app_log.h components/acquisition/include/acquisition.h components/ads1263/include/ads1263.h components/ads1263/include/ads1263_defs.h components/ads1263/arch/linux/include/ads1263_spi_linux.h components/ads1263/arch/linux/include/ads1263_gpio_linux.h'
record 021_source_sha256 'sha256sum main/main.c components/logger/src/app_log.c components/acquisition/src/acquisition.c components/ads1263/src/ads1263.c components/ads1263/arch/linux/src/ads1263_spi_linux.c components/ads1263/arch/linux/src/ads1263_gpio_linux.c components/logger/include/app_log.h components/acquisition/include/acquisition.h components/ads1263/include/ads1263.h components/ads1263/include/ads1263_defs.h components/ads1263/arch/linux/include/ads1263_spi_linux.h components/ads1263/arch/linux/include/ads1263_gpio_linux.h'
record 022_source_metadata 'stat --format="%n|size=%s|mtime=%y|mode=%a" main/main.c components/logger/src/app_log.c components/acquisition/src/acquisition.c components/ads1263/src/ads1263.c components/ads1263/arch/linux/src/ads1263_spi_linux.c components/ads1263/arch/linux/src/ads1263_gpio_linux.c components/logger/include/app_log.h components/acquisition/include/acquisition.h components/ads1263/include/ads1263.h components/ads1263/include/ads1263_defs.h components/ads1263/arch/linux/include/ads1263_spi_linux.h components/ads1263/arch/linux/include/ads1263_gpio_linux.h'
record 023_openspec_status 'openspec status --change "tsse-embedded-software-testing" --json'

exit 0
