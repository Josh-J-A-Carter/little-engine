# Find all .cpp and .h files in ./src/ and ./include/
files=$(find ./src ./include -type f \( -name "*.cpp" -o -name "*.h" \))

# Filter out 'preprocessor.cpp'
filtered_files=()
for file in $files; do
    if [[ $(basename "$file") != "preprocessor.cpp"
            && $(basename "$file") != "parse_types.cpp"
            && $(basename "$file") != "parse_types.h" ]]; then
        filtered_files+=("$file")
    fi
done

# Print the files (or use them as arguments)
g++ ./src/preprocessor.cpp -o ./build/preprocessor
./build/preprocessor "${filtered_files[@]}"
# printf "%s\n" "${filtered_files[@]}"