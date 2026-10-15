#!/usr/bin/env bash
set -u

readonly SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
readonly REPO_ROOT="$(git rev-parse --show-toplevel)"
readonly RUN_DIR="${SCRIPT_DIR#"${REPO_ROOT}/"}"
readonly REVIEW_DIR="${RUN_DIR}/integrity_review_20261015T162917Z"
readonly OLD_MANIFEST="${RUN_DIR}/raw_final/011_evidence_sha256.stdout.txt"

record() {
    local directory="$1"
    local id="$2"
    local command="$3"
    local prefix="${directory}/${id}"
    local rc

    printf '%s\n' "${command}" > "${prefix}.command.txt"
    date -u +%Y-%m-%dT%H:%M:%SZ > "${prefix}.started_utc.txt"
    bash -o pipefail -c "${command}" > "${prefix}.stdout.txt" 2> "${prefix}.stderr.txt"
    rc=$?
    printf '%s\n' "${rc}" > "${prefix}.exitcode.txt"
    date -u +%Y-%m-%dT%H:%M:%SZ > "${prefix}.finished_utc.txt"
}

capture_pre_correction() {
    if [[ -e "${REVIEW_DIR}" ]]; then
        printf 'Refusing to overwrite existing integrity-review directory: %s\n' "${REVIEW_DIR}" >&2
        exit 99
    fi
    mkdir "${REVIEW_DIR}"
    record "${REVIEW_DIR}" 001_old_manifest_pre_correction_verify "sha256sum -c \"${OLD_MANIFEST}\""
}

capture_final() {
    local final_dir="${REVIEW_DIR}/final"
    local inventory="${final_dir}/010_complete_inventory.txt"
    local manifest="${final_dir}/011_complete_sha256.txt"
    local checksum_stderr="${final_dir}/011_complete_sha256.stderr.txt"
    local checksum_exit="${final_dir}/011_complete_sha256.exitcode.txt"
    local checksum_finished="${final_dir}/011_complete_sha256.finished_utc.txt"
    local verify_stdout="${final_dir}/012_complete_sha256_verify.stdout.txt"
    local verify_stderr="${final_dir}/012_complete_sha256_verify.stderr.txt"
    local verify_exit="${final_dir}/012_complete_sha256_verify.exitcode.txt"
    local verify_finished="${final_dir}/012_complete_sha256_verify.finished_utc.txt"
    local files=()
    local inventory_files=()
    local checksum_files=()
    local file
    local rc

    if [[ ! -d "${REVIEW_DIR}" ]]; then
        printf 'Missing pre-correction review directory: %s\n' "${REVIEW_DIR}" >&2
        exit 98
    fi
    if [[ -e "${final_dir}" ]]; then
        printf 'Refusing to overwrite existing final integrity directory: %s\n' "${final_dir}" >&2
        exit 99
    fi
    mkdir "${final_dir}"

    record "${final_dir}" 001_old_manifest_post_correction_verify "sha256sum -c \"${OLD_MANIFEST}\""
    record "${final_dir}" 002_initial_source_hash_verify "sha256sum -c \"${RUN_DIR}/raw_metadata/021_source_sha256.stdout.txt\""
    record "${final_dir}" 003_post_analysis_source_hash_verify "sha256sum -c \"${RUN_DIR}/raw_analysis/005_source_sha256_after.stdout.txt\""
    record "${final_dir}" 004_old_final_source_hash_verify "sha256sum -c \"${RUN_DIR}/raw_final/002_final_source_sha256.stdout.txt\""
    record "${final_dir}" 005_current_head 'git rev-parse HEAD'
    record "${final_dir}" 006_current_production_diff 'git diff -- components main Makefile && git diff --cached -- components main Makefile'
    record "${final_dir}" 007_current_worktree 'git status --short --branch --untracked-files=all'

    printf '%s\n' 'Bash globstar inventory of every regular file under the run directory after all final artifact names were precreated.' > "${final_dir}/010_complete_inventory.command.txt"
    date -u +%Y-%m-%dT%H:%M:%SZ > "${final_dir}/010_complete_inventory.started_utc.txt"
    : > "${inventory}"
    : > "${final_dir}/010_complete_inventory.stderr.txt"
    : > "${final_dir}/010_complete_inventory.exitcode.txt"
    : > "${final_dir}/010_complete_inventory.finished_utc.txt"

    printf '%s\n' 'sha256sum over every path in 010_complete_inventory.txt except the eight explicitly named self-referential or post-hash output files in 009_checksum_exclusions.txt.' > "${final_dir}/011_complete_sha256.command.txt"
    date -u +%Y-%m-%dT%H:%M:%SZ > "${final_dir}/011_complete_sha256.started_utc.txt"
    : > "${manifest}"
    : > "${checksum_stderr}"
    : > "${checksum_exit}"
    : > "${checksum_finished}"

    printf '%s\n' "sha256sum -c \"${manifest}\"" > "${final_dir}/012_complete_sha256_verify.command.txt"
    date -u +%Y-%m-%dT%H:%M:%SZ > "${final_dir}/012_complete_sha256_verify.started_utc.txt"
    : > "${verify_stdout}"
    : > "${verify_stderr}"
    : > "${verify_exit}"
    : > "${verify_finished}"
    : > "${final_dir}/013_coverage_counts.txt"

    printf '%s\n' \
        "${manifest}" \
        "${checksum_stderr}" \
        "${checksum_exit}" \
        "${checksum_finished}" \
        "${verify_stdout}" \
        "${verify_stderr}" \
        "${verify_exit}" \
        "${verify_finished}" \
        > "${final_dir}/009_checksum_exclusions.txt"

    printf '%s\n' 'The inventory includes every regular file present under the run directory, including itself and all precreated final output paths. The checksum manifest excludes exactly eight files: its own content plus its stderr/exit/finished outputs, and the subsequent verification stdout/stderr/exit/finished outputs. Those files cannot be included without self-reference or mutation after hashing. Command and start-time records, the exclusion list, this script, the addendum, corrected documents, all historical raw files, and the complete inventory are included in the checksum manifest.' > "${final_dir}/008_scope_and_self_exclusion.txt"

    shopt -s globstar nullglob
    files=("${RUN_DIR}"/**)
    for file in "${files[@]}"; do
        if [[ -f "${file}" ]]; then
            inventory_files+=("${file}")
            printf '%s\n' "${file}" >> "${inventory}"
        fi
    done
    printf '%s\n' '0' > "${final_dir}/010_complete_inventory.exitcode.txt"
    date -u +%Y-%m-%dT%H:%M:%SZ > "${final_dir}/010_complete_inventory.finished_utc.txt"

    for file in "${inventory_files[@]}"; do
        case "${file}" in
            "${manifest}"|"${checksum_stderr}"|"${checksum_exit}"|"${checksum_finished}"|"${verify_stdout}"|"${verify_stderr}"|"${verify_exit}"|"${verify_finished}") ;;
            *) checksum_files+=("${file}") ;;
        esac
    done

    {
        printf 'inventory_entries=%d\n' "${#inventory_files[@]}"
        printf 'checksum_entries=%d\n' "${#checksum_files[@]}"
        printf 'explicit_exclusions=%d\n' '8'
        printf 'coverage_equation=%d-%d=%d\n' "${#inventory_files[@]}" '8' "${#checksum_files[@]}"
    } > "${final_dir}/013_coverage_counts.txt"

    sha256sum "${checksum_files[@]}" > "${manifest}" 2> "${checksum_stderr}"
    rc=$?
    printf '%s\n' "${rc}" > "${checksum_exit}"
    date -u +%Y-%m-%dT%H:%M:%SZ > "${checksum_finished}"

    sha256sum -c "${manifest}" > "${verify_stdout}" 2> "${verify_stderr}"
    rc=$?
    printf '%s\n' "${rc}" > "${verify_exit}"
    date -u +%Y-%m-%dT%H:%M:%SZ > "${verify_finished}"
}

case "${1:-}" in
    pre-correction) capture_pre_correction ;;
    finalize) capture_final ;;
    *)
        printf 'Usage: %s {pre-correction|finalize}\n' "$0" >&2
        exit 2
        ;;
esac
