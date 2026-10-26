#!/usr/bin/env bash
# Renders the demo animation offscreen and turns it into docs/demo.gif.
# Needs ffmpeg; gifsicle is used to shrink the result when it is installed.
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
binary="${PATHFINDER:-$root/build/pathfinder}"
output="${1:-$root/docs/demo.gif}"
frames="$(mktemp -d)"
trap 'rm -rf "$frames"' EXIT

"$binary" --seed 42 --size 41 --delay-ms 8 --record "$frames"

# Hold the final frame for three seconds, then build a palette from the
# frames so the few flat colours stay exact.
ffmpeg -loglevel error -y -framerate 30 -i "$frames/frame-%05d.bmp" \
    -vf "tpad=stop_mode=clone:stop_duration=3,split[a][b];[a]palettegen=max_colors=32:stats_mode=full[p];[b][p]paletteuse=dither=none" \
    "$output"

if command -v gifsicle >/dev/null; then
    gifsicle -O3 --batch "$output"
fi
echo "Wrote $output ($(du -h "$output" | cut -f1))"
