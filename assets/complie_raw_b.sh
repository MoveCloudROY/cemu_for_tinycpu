#!/bin/bash

# Get the directory path where the shell script is located
script_directory=$(dirname "$(readlink -f "$0")")

# Check if the file path is provided as the first parameter
if [ "$#" -lt 1 ]; then
    echo "Please provide the paths to the C and assembly files as parameters."
    exit 1
fi

# Construct the full path of the C file
c_file="$script_directory/$1"

# Check if the provided file has a .c extension
if [[ "${c_file##*.}" != "c" && "${c_file##*.}" != "s" ]]; then
    echo "Invalid file type. Please provide a C or S file."
    exit 1
fi

# Change to the script directory
cd "$script_directory" || exit

# Compile the C file
loongarch32r-linux-gnusf-gcc -nostdinc -nostdlib -ffreestanding -nostartfiles --sysroot=${LOONGTARGETSYSROOT} -o "${1%.*}.bin" "$c_file" -T ldscript.ld

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. The Binary ELF is created."
else
    echo "Compilation failed."
fi

# Strip the Binary file
loongarch32r-linux-gnusf-strip --strip-all "${1%.*}.bin"
# Check if the strip was successful
if [ $? -eq 0 ]; then
    echo "Strip successful."
else
    echo "Strip failed."
fi

# Preview the RAW Binary file
loongarch32r-linux-gnusf-objdump -fd "${1%.*}.bin"

# Dump the code into a RAW Binary file
loongarch32r-linux-gnusf-objcopy -O binary -j .text "${1%.*}.bin" "${1%.*}.raw"
# Check if the strip was successful
if [ $? -eq 0 ]; then
    echo "Dump successful. The RAW Binary is created."
else
    echo "Dump failed."
fi
