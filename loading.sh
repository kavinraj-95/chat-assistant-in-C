#!/bin/bash

# Set background to white
tput setb 7
# Set text color to black
tput setaf 0

# Define spinner characters
spinner=( '⠋' '⠙' '⠹' '⠸' '⠼' '⠴' '⠦' '⠧' '⠇' '⠏' )

# Clean up on script exit
cleanup() {
    tput reset
    tput cnorm
    exit 0
}

trap cleanup EXIT
tput civis

start_time=$(date +%s)
duration=2

i=0
while true; do
    current_time=$(date +%s)
    elapsed=$((current_time - start_time))
    
    # Calculate progress percentage
    progress=$((elapsed * 100 / duration))
    if [ $progress -gt 100 ]; then
        progress=100
    fi
    
    if [ $elapsed -ge $duration ]; then
        printf "\rLoading complete! [100%%]   \n"
        break
    fi
    
    # Display spinner with progress percentage
    printf "\r[%s] Loading... %d%%" "${spinner[i]}" "$progress"
    i=$(( (i+1) % ${#spinner[@]} ))
    sleep 0.1
done

cleanup