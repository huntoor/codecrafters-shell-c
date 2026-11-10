!/bin/bash

# Create a temporary directory for testing
TEST_DIR=$(mktemp -d)
cd "$TEST_DIR"

# Define the expected outputs
EXPECTED_OUTPUT_1="Hello, World!"
EXPECTED_OUTPUT_2="Hello, World!\\nAgain!"
EXPECTED_OUTPUT_3="New content"
EXPECTED_OUTPUT_4="Error"
EXPECTED_OUTPUT_5=$(cat /etc/passwd)
EXPECTED_OUTPUT_6="Test"

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

# Clean up Functino
clean_up() {
	cd ..
	rm -rf "$TEST_DIR"
}

# Run the test cases
run_test_case "echo 'Hello, World!' > test.txt" "$EXPECTED_OUTPUT_1"
run_test_case "echo 'Hello, World!' > test.txt && echo 'Again!' >> test.txt" "$EXPECTED_OUTPUT_2"
run_test_case "echo 'Initial content' > test.txt && echo 'New content' > test.txt" "$EXPECTED_OUTPUT_3"
run_test_case "echo 'Error' > /nonexistent/test.txt" "$EXPECTED_OUTPUT_4"
run_test_case "cat /etc/passwd > test.txt && cat test.txt" "$EXPECTED_OUTPUT_5"
run_test_case "echo 'Test' > /tmp/test.txt" "$EXPECTED_OUTPUT_6"

clean_up
