
#!/usr/bin/env bash
#
# Usage: ./run_tests.sh <path-to-your-shell> [tests-file]
#

set -u

SHELL_BIN="${1:-./myshell}"
TESTS_FILE="${2:-tests_cases.txt}"
TIMEOUT=5

# --- Validate dependencies ---

if [[ ! -x "$SHELL_BIN" ]]; then
    echo "Error: '$SHELL_BIN' is not executable" >&2
    exit 1
fi

if [[ ! -f "$TESTS_FILE" ]]; then
    echo "Error: tests file '$TESTS_FILE' not found" >&2
    exit 1
fi

if ! command -v timeout >/dev/null 2>&1; then
    echo "Error: 'timeout' command is required" >&2
    exit 1
fi

# --- Temporary directory ---

TMP_DIR=$(mktemp -d "${TMPDIR:-/tmp}/shelltest.XXXXXX") || exit 1
trap 'rm -rf -- "$TMP_DIR"' EXIT

STDOUT_FILE="$TMP_DIR/stdout"
STDERR_FILE="$TMP_DIR/stderr"

# --- Colors ---

GREEN=$'\033[0;32m'
RED=$'\033[0;31m'
YELLOW=$'\033[0;33m'
NC=$'\033[0m'

pass=0
fail=0
total=0

# --- Test data ---

cur_name=""
cur_cmds=()
cur_out=()
cur_err=()
cur_exit=""
in_test=0
test_errors=()

# --- Helpers ---

clean() {
    local s="$1"

    s="${s%$'\r'}"
    s="${s#"${s%%[![:space:]]*}"}"
    s="${s%"${s##*[![:space:]]}"}"

    printf '%s' "$s"
}

print_indented() {
    sed 's/^/      /' "$1"
}

# --- Execute and evaluate one test ---

flush_test() {
    [[ $in_test -eq 1 ]] || return

    total=$((total + 1))

    local input=""
    local c
    local actual_exit
    local ok=1

    local -a reasons=()

    # Validate test definition.
    if [[ -z "$cur_name" ]]; then
        reasons+=("Missing NAME field")
        ok=0
    fi

    # Build shell input.
    for c in "${cur_cmds[@]}"; do
        input+="$c"$'\n'
    done

    # Execute shell with timeout.
    printf '%s' "$input" |
        timeout "$TIMEOUT" "$SHELL_BIN" \
        >"$STDOUT_FILE" 2>"$STDERR_FILE"

    actual_exit=$?

    # Check timeout.
    if (( actual_exit == 124 )); then
        ok=0
        reasons+=("Shell timed out after ${TIMEOUT}s")
    fi

    # Check expected exit code.
    if [[ -n "$cur_exit" && "$cur_exit" != "$actual_exit" ]]; then
        ok=0
        reasons+=("exit: expected '$cur_exit', got '$actual_exit'")
    fi

    # Check expected stdout lines.
    local want

    for want in "${cur_out[@]}"; do
        if ! grep -Fqx -- "$want" "$STDOUT_FILE"; then
            ok=0
            reasons+=("stdout missing line: '$want'")
        fi
    done

    # Check expected stderr lines.
    for want in "${cur_err[@]}"; do
        if ! grep -Fqx -- "$want" "$STDERR_FILE"; then
            ok=0
            reasons+=("stderr missing line: '$want'")
        fi
    done

    # --- Report result ---

    if (( ok )); then
        printf '%sPASS%s  %s\n' "$GREEN" "$NC" "$cur_name"
        pass=$((pass + 1))
    else
        printf '%sFAIL%s  %s\n' "$RED" "$NC" "${cur_name:-Unnamed test}"

        printf '      input: %q\n' "$input"

        for reason in "${reasons[@]}"; do
            printf '      - %s\n' "$reason"
        done

        printf '      --- stdout ---\n'
        print_indented "$STDOUT_FILE"

        printf '      --- stderr ---\n'
        print_indented "$STDERR_FILE"

        printf '      --- exit code ---\n'
        printf '      %s\n' "$actual_exit"

        fail=$((fail + 1))
    fi

    # Reset test data.
    cur_name=""
    cur_cmds=()
    cur_out=()
    cur_err=()
    cur_exit=""
    in_test=0
}

# --- Parse test file ---

while IFS= read -r line || [[ -n "$line" ]]; do

    if [[ "$line" == "---TEST---" ]]; then
        flush_test
        in_test=1
        continue
    fi

    [[ $in_test -eq 0 ]] && continue

    case "$line" in
        NAME:*)
            cur_name="${line#NAME:}"
            cur_name="${cur_name# }"
            ;;

        CMD:*)
            cur_cmds+=("$(clean "${line#CMD:}")")
            ;;

        EXPECT_STDOUT:*)
            cur_out+=("$(clean "${line#EXPECT_STDOUT:}")")
            ;;

        EXPECT_STDERR:*)
            cur_err+=("$(clean "${line#EXPECT_STDERR:}")")
            ;;

        EXPECT_EXIT:*)
            cur_exit="$(clean "${line#EXPECT_EXIT:}")"
            ;;
    esac

done < "$TESTS_FILE"

# Run final test.
flush_test

# --- Summary ---

echo
echo "================================"
printf "  total: %d  " "$total"
printf "%spass: %d%s  " "$GREEN" "$pass" "$NC"
printf "%sfail: %d%s\n" "$RED" "$fail" "$NC"
echo "================================"

(( fail == 0 ))

