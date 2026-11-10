#!/bin/bash


# Function to run a test case
run_test_case() {
    local input="$1"
    local expected_output="$2"
    local actual_output

    # Execute the input command
    actual_output=$(bash -c "$input" 2>&1)

    # Check if the actual output matches the expected output
    if [[ "$actual_output" == "$expected_output" ]]; then
        echo "PASS: $input"
    else
        echo "FAIL: $input"
        echo "Expected: $expected_output"
        echo "Actual: $actual_output"
    fi
}

# Read test cases from the file
test_cases_file="./test_cases.txt"
if [[ ! -e "$test_cases_file" ]]; then
    echo "Test cases file not found!"
    exit 1
fi


# Initialize variables
input=""
expected_output=""

# Read the test cases file line by line
while IFS= read -r line; do
    if [[ "$line" == \#* ]]; then
        continue  # Skip comments
    fi

    # Skip empty line
    if [[ -z "$line" ]]; then
        continue
    fi

    if [[ -z "$input" ]]; then
        input="$line"
    elif [[ -z "$expected_output" ]]; then
        expected_output="$line"
        run_test_case "$input" "$expected_output"
        input=""
        expected_output=""
    fi
done < "$test_cases_file"

# Create a temporary directory for testing
TEST_DIR=$(mktemp -d)
cd "$TEST_DIR"

# Clean up
cd ..
rm -rf "$TEST_DIR"

