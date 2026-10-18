#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
test_build=$(mktemp -d /tmp/dmt99-tests.XXXXXX)
trap 'rm -rf "$test_build"' EXIT
# Set SANITIZE=1 where compiler sanitizer runtimes are installed.
extra_flags=()
if [[ ${SANITIZE:-0} == 1 ]]; then
    extra_flags=(-fsanitize=address,undefined -fno-omit-frame-pointer)
fi
"${CC:-cc}" -std=c11 -g -O1 -Wall -Wextra -Werror -pedantic "${extra_flags[@]}" \
   -I main main/rvswd_ro.c main/rvswd_diag.c main/rvswd_memory_lab.c main/rvswd_progbuf_lab.c tests/test_transport.c \
   -o "$test_build/test_transport"
"$test_build/test_transport"
"${CC:-cc}" -std=c11 -g -O1 -Wall -Wextra -Werror -pedantic "${extra_flags[@]}" \
   -I main main/rvswd_ro.c main/rvswd_diag.c main/rvswd_memory_lab.c main/rvswd_progbuf_lab.c tests/test_diagnostic.c \
   -o "$test_build/test_diagnostic"
"$test_build/test_diagnostic"
"${CC:-cc}" -std=c11 -g -O1 -Wall -Wextra -Werror -pedantic "${extra_flags[@]}" \
   -DRV_ABSTRACT_MEMORY_LAB -DRV_PROGBUF_LAB -I main main/rvswd_ro.c main/rvswd_diag.c \
   main/rvswd_memory_lab.c main/rvswd_progbuf_lab.c tests/test_diagnostic.c -o "$test_build/test_memory_lab"
"$test_build/test_memory_lab"
PYTHONDONTWRITEBYTECODE=1 python3 -m unittest discover -s tests -p 'test_*.py' -v
