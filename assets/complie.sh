#!/bin/bash

# Get the directory path where the shell script is located
script_directory=$(dirname "$(readlink -f "$0")")

# Check if file paths are provided as parameters
if [ "$#" -lt 1 ]; then
    echo "Please provide the paths to the C and assembly files as parameters."
    exit 1
fi

# Change to the script directory
cd "$script_directory" || exit

# Create an array to store the object files
object_files=()

# Iterate over the provided file paths
for file_path in "$@"; do
    # Construct the full path of the file
    full_path="$script_directory/$file_path"

    # Check if the provided file exists
    if [ ! -f "$full_path" ]; then
        echo "File $file_path does not exist."
        continue
    fi

    # Get the file extension
    extension="${file_path##*.}"

    # Check if the file has a .c or .s extension
    if [ "$extension" != "c" ] && [ "$extension" != "s" ]; then
        echo "Invalid file type. Only C and assembly (.s) files are supported."
        continue
    fi

    # Extract the filename without extension
    filename=$(basename "$file_path")
    filename="${filename%.*}"

    # Compile the file to an object file
    if [ "$extension" == "c" ]; then
        gcc -c -o "$filename.o" "$full_path"
    else
        gcc -c -o "$filename.o" "$full_path"
    fi

    # Check if the compilation was successful
    if [ $? -eq 0 ]; then
        echo "Compilation successful for $file_path."
        object_files+=("$filename.o")
    else
        echo "Compilation failed for $file_path."
    fi
done

# Link the object files into a single executable
if [ "${#object_files[@]}" -gt 0 ]; then
    gcc -o output "${object_files[@]}"
    if [ $? -eq 0 ]; then
        echo "Linking successful. The executable 'output' is created."
    else
        echo "Linking failed."
    fi
fi

