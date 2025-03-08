#include <stack>

#include "serialise.h"
#include "utilities.h"

#include <iostream>

// 
// 
// Deserialisation
// 
// 

option<node*, error> parse_attribute(std::string_view str, int start, int end) {
    // Check the correctness of the formatting
    // Curly & square braces
    std::stack<char> brace_stack = {};

    int final_index = -1;
    
    for (int i = start ; i <= end ; i += 1) {
        char c = str[i];

        if (c == '{' || c == '[') brace_stack.push(c);

        else if (c == '}') {
            if (brace_stack.top() != '{' || brace_stack.size() <= 0) {
                return { error { "Malformed JSON. Closed curly brace with no matching opening brace." } };
            }
            
            brace_stack.pop();
        }

        else if (c == ']') {
            if (brace_stack.top() != '[' || brace_stack.size() <= 0) {
                return { error { "Malformed JSON. Closed square bracket with no matching opening bracket." } };
            }
            
            brace_stack.pop();
        }

        if (brace_stack.size() == 0) {
            final_index = i;
            break;
        }
    }

    if (str[0] == '{' && str[final_index] == '}') {
        std::cout << "obj" << std::endl;
    }

    else if (str[0] == '[' && str[final_index] == ']') {
        std::cout << "array" << std::endl;
    }

    else {
        std::cout << "primitive" << std::endl;
    }

    return { error { "No errors found" } };
}

option<node*, error> parse_file_to_node_tree(std::string filename) {
    // Read entire file in
    std::ifstream file { filename };
    std::string contents {};
    std::string line {};

    if (!file.is_open()) {
        return { error { "Failed to open file." } };
    }

    while (std::getline(file, line)) {
        contents += line + "\n";
    }

    // Pre-processing for correctness
    rtrim(contents);
    ltrim(contents);

    if (contents[0] != '{' || contents[contents.size() - 1] != '}') {
        return { error { "Malformed JSON. File should be enclosed by an object using curly braces." } };
    }

    return parse_attribute(contents, 0, contents.length() - 1);
}

node* read_scene_from_file(std::string filename) {
    option<node*, error> out = parse_file_to_node_tree(filename);

    if (std::holds_alternative<error>(out)) {
        std::cout << "Error: " << std::get<error>(out).message << std::endl;
        return nullptr;
    }

    return std::get<node*>(out);
}




// 
// 
// Serialisation
// 
// 

