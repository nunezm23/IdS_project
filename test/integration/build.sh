#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
    printf 'Usage: %s EMPTY_OUTPUT_DIR\n' "$0" >&2
    exit 2
fi

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)
repo_root=$(cd -- "$script_dir/../.." && pwd -P)
requested_output=$1

if [[ ! -d "$requested_output" ]]; then
    printf 'OUTPUT_DIR must already exist and be an empty directory: %s\n' "$requested_output" >&2
    exit 2
fi
if [[ -L "$requested_output" ]]; then
    printf 'OUTPUT_DIR must not be a symbolic link: %s\n' "$requested_output" >&2
    exit 2
fi

lexical_output=$(readlink -m -- "$requested_output")
physical_output=$(realpath -e -- "$requested_output")
if [[ "$lexical_output" != "$physical_output" ]]; then
    printf 'OUTPUT_DIR must not contain symbolic-link path components: %s\n' "$requested_output" >&2
    exit 2
fi
output_dir=$physical_output

case "$output_dir/" in
    "$repo_root/"*)
        printf 'OUTPUT_DIR must be outside the repository: %s\n' "$output_dir" >&2
        exit 2
        ;;
esac

shopt -s nullglob dotglob
output_entries=("$output_dir"/*)
if (( ${#output_entries[@]} != 0 )); then
    printf 'OUTPUT_DIR must be empty to prevent clobbering: %s\n' "$output_dir" >&2
    exit 2
fi
shopt -u nullglob dotglob

cc_name=${CC:-gcc}
if ! cc=$(command -v -- "$cc_name"); then
    printf 'mandatory compiler is unavailable: %s\n' "$cc_name" >&2
    exit 3
fi
cc=$(realpath -e -- "$cc")

common_flags=(
    -std=c11
    -O2
    -g
    -Wall
    -Wextra
    -pedantic
    -D_POSIX_C_SOURCE=200809L
    -DADS1263_NO_GPIOD=1
    -I"$repo_root/components/logger/include"
    -I"$repo_root/components/ads1263/include"
    -I"$repo_root/components/ads1263/arch/linux/include"
    -I"$repo_root/components/acquisition/include"
)
production_sources=(
    "$repo_root/main/main.c"
    "$repo_root/components/acquisition/src/acquisition.c"
    "$repo_root/components/ads1263/src/ads1263.c"
    "$repo_root/components/logger/src/app_log.c"
)
linux_adapters=(
    "$repo_root/components/ads1263/arch/linux/src/ads1263_spi_linux.c"
    "$repo_root/components/ads1263/arch/linux/src/ads1263_gpio_linux.c"
)

real_binary="$output_dir/ads1263_app"
fake_binary="$output_dir/ads1263_app_fake_hal"
real_argv=(
    "$cc"
    "${common_flags[@]}"
    "${production_sources[@]}"
    "${linux_adapters[@]}"
    -o "$real_binary"
)
fake_argv=(
    "$cc"
    "${common_flags[@]}"
    "${production_sources[@]}"
    "$script_dir/fake_hal/fake_hal.c"
    -o "$fake_binary"
)

"${real_argv[@]}"
"${fake_argv[@]}"

real_argv_file="$output_dir/.real_argv"
fake_argv_file="$output_dir/.fake_argv"
compiler_version_file="$output_dir/.compiler_version"
printf '%s\0' "${real_argv[@]}" > "$real_argv_file"
printf '%s\0' "${fake_argv[@]}" > "$fake_argv_file"
"$cc" --version > "$compiler_version_file"

python3 "$script_dir/build_manifest.py" \
    --repo-root "$repo_root" \
    --output-dir "$output_dir" \
    --compiler "$cc" \
    --compiler-version-file "$compiler_version_file" \
    --real-argv-file "$real_argv_file" \
    --fake-argv-file "$fake_argv_file"

rm -f -- "$real_argv_file" "$fake_argv_file" "$compiler_version_file"

printf 'Built %s\n' "$real_binary"
printf 'Built %s\n' "$fake_binary"
printf 'Manifest %s\n' "$output_dir/build_manifest.json"
