#!/bin/sh
set -eu

APP="${1:-./ads1263_app}"

"$APP" --help
"$APP" --check
"$APP" --samples 5 --channel 0 --rate-hz 2 --csv /tmp/ads1263_smoke.csv
cat /tmp/ads1263_smoke.csv
