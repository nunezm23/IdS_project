#!/usr/bin/env bash
set -u

readonly SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
readonly REPO_ROOT="$(git rev-parse --show-toplevel)"
readonly RUN_DIR="${SCRIPT_DIR#"${REPO_ROOT}/"}"
readonly RAW_DIR="${RUN_DIR}/raw_final"

if [[ -e "${RAW_DIR}" ]]; then
    printf 'Refusing to overwrite existing final-audit directory: %s\n' "${RAW_DIR}" >&2
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

record 001_final_head 'git rev-parse HEAD'
record 002_final_source_sha256 'sha256sum main/main.c components/logger/src/app_log.c components/acquisition/src/acquisition.c components/ads1263/src/ads1263.c components/ads1263/arch/linux/src/ads1263_spi_linux.c components/ads1263/arch/linux/src/ads1263_gpio_linux.c components/logger/include/app_log.h components/acquisition/include/acquisition.h components/ads1263/include/ads1263.h components/ads1263/include/ads1263_defs.h components/ads1263/arch/linux/include/ads1263_spi_linux.h components/ads1263/arch/linux/include/ads1263_gpio_linux.h'
record 003_initial_vs_post_source_hashes "cmp \"${RUN_DIR}/raw_metadata/021_source_sha256.stdout.txt\" \"${RUN_DIR}/raw_analysis/005_source_sha256_after.stdout.txt\""
record 004_initial_vs_final_source_hashes "cmp \"${RUN_DIR}/raw_metadata/021_source_sha256.stdout.txt\" \"${RAW_DIR}/002_final_source_sha256.stdout.txt\""
record 005_final_production_diff 'git diff -- components main Makefile && git diff --cached -- components main Makefile'
record 006_final_changed_tracked_paths 'git diff --name-only && git diff --cached --name-only'
record 007_final_worktree 'git status --short --branch --untracked-files=all'

printf '%s\n' 'Built-in field-count loop in 05_finalize_evidence.sh over the exhaustive pipe-delimited stderr.' > "${RAW_DIR}/008_exhaustive_diagnostic_counts.command.txt"
date -u +%Y-%m-%dT%H:%M:%SZ > "${RAW_DIR}/008_exhaustive_diagnostic_counts.started_utc.txt"
style=0
information=0
missing_include=0
checker_metadata=0
const_pointer=0
unused_member=0
static_function=0
while IFS='|' read -r _ _ _ severity diagnostic_id _; do
    case "${severity}" in
        style) style=$((style + 1)) ;;
        information) information=$((information + 1)) ;;
    esac
    case "${diagnostic_id}" in
        missingIncludeSystem) missing_include=$((missing_include + 1)) ;;
        checkersReport) checker_metadata=$((checker_metadata + 1)) ;;
        constParameterPointer) const_pointer=$((const_pointer + 1)) ;;
        unusedStructMember) unused_member=$((unused_member + 1)) ;;
        staticFunction) static_function=$((static_function + 1)) ;;
    esac
done < "${RUN_DIR}/raw_analysis_exhaustive/001_cppcheck_exhaustive_no_gpiod.stderr.txt"
{
    printf 'style=%d\n' "${style}"
    printf 'information=%d\n' "${information}"
    printf 'missingIncludeSystem=%d\n' "${missing_include}"
    printf 'checkersReport_metadata=%d\n' "${checker_metadata}"
    printf 'constParameterPointer=%d\n' "${const_pointer}"
    printf 'unusedStructMember=%d\n' "${unused_member}"
    printf 'staticFunction=%d\n' "${static_function}"
} > "${RAW_DIR}/008_exhaustive_diagnostic_counts.stdout.txt"
: > "${RAW_DIR}/008_exhaustive_diagnostic_counts.stderr.txt"
printf '%s\n' '0' > "${RAW_DIR}/008_exhaustive_diagnostic_counts.exitcode.txt"
date -u +%Y-%m-%dT%H:%M:%SZ > "${RAW_DIR}/008_exhaustive_diagnostic_counts.finished_utc.txt"

printf '%s\n' 'Built-in prefix-count loop in 05_finalize_evidence.sh over findings.md.' > "${RAW_DIR}/009_finding_record_counts.command.txt"
date -u +%Y-%m-%dT%H:%M:%SZ > "${RAW_DIR}/009_finding_record_counts.started_utc.txt"
source_records=0
environment_records=0
while IFS= read -r line; do
    case "${line}" in
        '| `SA-CPPCHECK-'*) source_records=$((source_records + 1)) ;;
        '| `SA-ENV-'*) environment_records=$((environment_records + 1)) ;;
    esac
done < "${RUN_DIR}/findings.md"
{
    printf 'SA-CPPCHECK records=%d\n' "${source_records}"
    printf 'SA-ENV records=%d\n' "${environment_records}"
    printf 'total stable records=%d\n' "$((source_records + environment_records))"
} > "${RAW_DIR}/009_finding_record_counts.stdout.txt"
: > "${RAW_DIR}/009_finding_record_counts.stderr.txt"
printf '%s\n' '0' > "${RAW_DIR}/009_finding_record_counts.exitcode.txt"
date -u +%Y-%m-%dT%H:%M:%SZ > "${RAW_DIR}/009_finding_record_counts.finished_utc.txt"

readonly INVENTORY="${RAW_DIR}/010_evidence_inventory.stdout.txt"
readonly MANIFEST="${RAW_DIR}/011_evidence_sha256.stdout.txt"
readonly CHECKSUM_STDERR="${RAW_DIR}/011_evidence_sha256.stderr.txt"
readonly CHECKSUM_EXIT="${RAW_DIR}/011_evidence_sha256.exitcode.txt"
readonly CHECKSUM_FINISHED="${RAW_DIR}/011_evidence_sha256.finished_utc.txt"
readonly VERIFY_STDOUT="${RAW_DIR}/012_evidence_sha256_verify.stdout.txt"
readonly VERIFY_STDERR="${RAW_DIR}/012_evidence_sha256_verify.stderr.txt"
readonly VERIFY_EXIT="${RAW_DIR}/012_evidence_sha256_verify.exitcode.txt"
readonly VERIFY_FINISHED="${RAW_DIR}/012_evidence_sha256_verify.finished_utc.txt"

printf '%s\n' 'Bash globstar inventory of every regular file under the run directory after all final output paths were precreated.' > "${RAW_DIR}/010_evidence_inventory.command.txt"
date -u +%Y-%m-%dT%H:%M:%SZ > "${RAW_DIR}/010_evidence_inventory.started_utc.txt"
: > "${INVENTORY}"
: > "${RAW_DIR}/010_evidence_inventory.stderr.txt"
: > "${RAW_DIR}/010_evidence_inventory.exitcode.txt"
: > "${RAW_DIR}/010_evidence_inventory.finished_utc.txt"

printf '%s\n' 'sha256sum over every inventory entry except the eight exact paths recorded in 013_checksum_exclusions.txt.' > "${RAW_DIR}/011_evidence_sha256.command.txt"
date -u +%Y-%m-%dT%H:%M:%SZ > "${RAW_DIR}/011_evidence_sha256.started_utc.txt"
: > "${MANIFEST}"
: > "${CHECKSUM_STDERR}"
: > "${CHECKSUM_EXIT}"
: > "${CHECKSUM_FINISHED}"

printf '%s\n' "sha256sum -c \"${MANIFEST}\"" > "${RAW_DIR}/012_evidence_sha256_verify.command.txt"
date -u +%Y-%m-%dT%H:%M:%SZ > "${RAW_DIR}/012_evidence_sha256_verify.started_utc.txt"
: > "${VERIFY_STDOUT}"
: > "${VERIFY_STDERR}"
: > "${VERIFY_EXIT}"
: > "${VERIFY_FINISHED}"
: > "${RAW_DIR}/014_checksum_coverage.txt"

printf '%s\n' \
    "${MANIFEST}" \
    "${CHECKSUM_STDERR}" \
    "${CHECKSUM_EXIT}" \
    "${CHECKSUM_FINISHED}" \
    "${VERIFY_STDOUT}" \
    "${VERIFY_STDERR}" \
    "${VERIFY_EXIT}" \
    "${VERIFY_FINISHED}" \
    > "${RAW_DIR}/013_checksum_exclusions.txt"

shopt -s globstar nullglob
paths=("${RUN_DIR}"/**)
inventory_files=()
checksum_files=()
for file in "${paths[@]}"; do
    if [[ -f "${file}" ]]; then
        inventory_files+=("${file}")
        printf '%s\n' "${file}" >> "${INVENTORY}"
    fi
done
printf '%s\n' '0' > "${RAW_DIR}/010_evidence_inventory.exitcode.txt"
date -u +%Y-%m-%dT%H:%M:%SZ > "${RAW_DIR}/010_evidence_inventory.finished_utc.txt"

for file in "${inventory_files[@]}"; do
    case "${file}" in
        "${MANIFEST}"|"${CHECKSUM_STDERR}"|"${CHECKSUM_EXIT}"|"${CHECKSUM_FINISHED}"|"${VERIFY_STDOUT}"|"${VERIFY_STDERR}"|"${VERIFY_EXIT}"|"${VERIFY_FINISHED}") ;;
        *) checksum_files+=("${file}") ;;
    esac
done
{
    printf 'inventory_entries=%d\n' "${#inventory_files[@]}"
    printf 'checksum_entries=%d\n' "${#checksum_files[@]}"
    printf 'explicit_exclusions=8\n'
    printf 'coverage_equation=%d-8=%d\n' "${#inventory_files[@]}" "${#checksum_files[@]}"
} > "${RAW_DIR}/014_checksum_coverage.txt"

sha256sum "${checksum_files[@]}" > "${MANIFEST}" 2> "${CHECKSUM_STDERR}"
rc=$?
printf '%s\n' "${rc}" > "${CHECKSUM_EXIT}"
date -u +%Y-%m-%dT%H:%M:%SZ > "${CHECKSUM_FINISHED}"

sha256sum -c "${MANIFEST}" > "${VERIFY_STDOUT}" 2> "${VERIFY_STDERR}"
rc=$?
printf '%s\n' "${rc}" > "${VERIFY_EXIT}"
date -u +%Y-%m-%dT%H:%M:%SZ > "${VERIFY_FINISHED}"

exit 0
