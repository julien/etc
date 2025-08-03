#!/usr/bin/env bash
set -eou pipefail

colorize() {
  local code=$1
  local text=$2
  printf "\\x1b[%sm -> \x1b[%sm%s\x1b[0m\n" "$code" "$code" "$text"
}

echo "--- Text Styles ---"
colorize "1" "Bold"
colorize "4" "Underline"
colorize "5" "Blink"
colorize "7" "Reverse"
echo

echo "--- 8 Foreground Colors ---"
for i in {30..37}; do colorize "$i" "Color $i"; done
echo

echo "--- 8 Bright Foreground Colors ---"
for i in {90..97}; do colorize "$i" "Color $i"; done
echo

echo "--- 8 Background Colors ---"
for i in {40..47}; do colorize "$i" "Background $i"; done
echo

echo "--- 8 Bright Background Colors ---"
for i in {100..107}; do colorize "$i" "Background $i"; done
echo

echo "--- Combined ---"
colorize "1;31" "Bold Red"
colorize "4;92" "Underlined Bright Green"
colorize "1;103" "Bold with Bright Yellow Background"
echo

echo "--- Other common codes (should be stripped) ---"
printf "Saving screen: \x1b[?47h"
printf "Loading screen: \x1b[?47l"
printf "Line with a bell: \x07" # Bell character, not an escape code
printf "Cursor up: \x1b[A"

