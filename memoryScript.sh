#!/bin/bash

# Enhanced memory test script with better error handling
echo "Starting enhanced memory leak test..."

# Start server
./webserv ./config/file.conf &
SERVER_PID=$!

# Give server time to start
sleep 2

# Check if server started successfully
if ! kill -0 $SERVER_PID 2>/dev/null; then
    echo "ERROR: Server failed to start or crashed immediately"
    exit 1
fi

echo "Server started with PID: $SERVER_PID"

# Function to check if process is still running
is_process_running() {
    kill -0 $1 2>/dev/null
    return $?
}

# Function to get detailed memory info
get_memory_info() {
    local pid=$1
    if is_process_running $pid; then
        local rss=$(ps -o rss= -p $pid 2>/dev/null | tr -d ' ')
        local vsz=$(ps -o vsz= -p $pid 2>/dev/null | tr -d ' ')
        local pmem=$(ps -o pmem= -p $pid 2>/dev/null | tr -d ' ')
        echo "RSS: ${rss}KB, VSZ: ${vsz}KB, %MEM: ${pmem}%"
    else
        echo "Process not running"
        return 1
    fi
}

# Initial memory check
echo "Initial memory:"
get_memory_info $SERVER_PID

# Stress test with better error handling
FAILED_REQUESTS=0
SUCCESSFUL_REQUESTS=0

for i in {1..200}; do
    # Check if server is still running before making request
    if ! is_process_running $SERVER_PID; then
        echo "ERROR: Server crashed at request $i"
        echo "Check server logs for details"
        exit 1
    fi
    
    # Make request with timeout
    if curl -s --connect-timeout 5 --max-time 10 http://localhost:8080/ > /dev/null 2>&1; then
        ((SUCCESSFUL_REQUESTS++))
    else
        ((FAILED_REQUESTS++))
        echo "Request $i failed"
    fi
    
    # Memory check every 25 requests
    if [ $((i % 25)) -eq 0 ]; then
        echo "Request $i: $(get_memory_info $SERVER_PID)"
        echo "  Success: $SUCCESSFUL_REQUESTS, Failed: $FAILED_REQUESTS"
    fi
    
    # Small delay to prevent overwhelming the server
    sleep 0.1
done

echo "Test completed!"
echo "Total requests: 200"
echo "Successful: $SUCCESSFUL_REQUESTS"
echo "Failed: $FAILED_REQUESTS"

if is_process_running $SERVER_PID; then
    echo "Final memory check:"
    get_memory_info $SERVER_PID
    
    echo "Detailed process info:"
    ps -o pid,ppid,rss,vsz,%mem,%cpu,time,comm -p $SERVER_PID
    
    # Clean shutdown
    kill $SERVER_PID
    sleep 2
    
    # Force kill if still running
    if is_process_running $SERVER_PID; then
        echo "Force killing server..."
        kill -9 $SERVER_PID
    fi
else
    echo "Server process already terminated"
fi

echo "Memory test completed."