#include <stack>

#include "serialise.h"
#include "utilities.h"
#include "arena.h"

#include <iostream>

#define PARSE_ARENA_SIZE 1024 * 1024 // = 1 MiB

// 
// 
// Deserialisation
// 
// 

option<node*, error> parse(arena& arena, std::string_view str, int start, int end);

option<node*, error> parse_primitive(arena& arena, std::string_view str, int start, int end) {
    if (start > end) return { error { "Empty primitive." } };
    // Basically the base-case; just need to first check syntax
    error err = { "Invalid character in value, in key-value pair." };
    for (int i = start ; i <= end ; i += 1) {
        if (str[i] == ':') return { err };
        if (str[i] == '{') return { err };
        if (str[i] == '}') return { err };
        if (str[i] == '[') return { err };
        if (str[i] == ']') return { err };
        if (str[i] == ',') return { err };
    }

    std::string_view s { str.substr(start, end) };
    primitive_node* n { arena.allocate<primitive_node>() };
    n->entry = s;

    return { n };
}

option<node*, error> parse_object(arena& arena, std::string_view str, int start, int end) {
    if (start >= end) return { error { "Empty object." } };
    // Find all pairs of indices in the string such that str.substr(attributes[i][0], attributes[i][1]) is an attribute
    std::vector<std::pair<int, int>> attributes {};
    // Curly & square braces - only split attributes if they are within the base level object
    // e.g. 
    // {
    //  a : 5,
    //  b : { x : 1, y : 2 }
    // }
    // should only find two attributes, and must parse the object stored in 'b' only in a recursive function call
    std::stack<char> brace_stack {};

    int attr_begin = start;

    for (int i = start ; i <= end ; i += 1) {
        // Calculate the scope / how nested the object is at this current character
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

        // Are we in the main object scope?
        if (brace_stack.empty()) {
            // Just ended an attribute
            if (c == ',') {
                attributes.push_back({ attr_begin, i - 1 });
                attr_begin = i + 1;
            }

            else if (i == end && attr_begin < i) {
                attributes.push_back({ attr_begin, i });
            }
        }
    }
    
    // Create an object node and begin parsing the attributes
    object_node* n { arena.allocate<object_node>() };
    
    for (std::pair<int, int> pair : attributes) {
        if (pair.first >= pair.second) return { error { "Empty object attribute." } };

        // For this to be a valid attribute, there must exist exactly one colon between pair.first and pair.second.
        int pos_first = str.find(":", pair.first);

        if (pos_first == std::string::npos || pos_first > end) {
            return { error { "Did not find colon in attribute. JSON must be structured as key-value pairs." } };
        }
        
        int pos_second = str.find(":", pos_first + 1);
 
        if (pos_second != std::string::npos && pos_second <= pair.second) {
            return { error { "Too many colons. JSON must be structured as key-value pairs." } };
        }

        // The data is a valid key-value pair, so now we need to parse the value part further into a node.
        option<node*, error> result = parse(arena, str, pos_first + 1, pair.second);
        if (std::holds_alternative<error>(result)) {
            return result;
        }

        node* value = std::get<node*>(result);
        n->attributes.push_back({ str.substr(pair.first, pair.first + 1 - pos_first), value });
    }

    return { n };
}

option<node*, error> parse(arena& arena, std::string_view str, int start, int end) {
    // Check the correctness of the formatting
    // Curly & square braces
    std::stack<char> brace_stack {};

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

        if (brace_stack.empty()) {
            final_index = i;
        }
    }

    if (str[0] == '{' && str[final_index] == '}') {
        return parse_object(arena, str, start + 1, final_index - 1);
    }

    // else if (str[0] == '[' && str[final_index] == ']') {
    //     return parse_array(arena, str, start + 1, final_index - 1);
    // }

    else {
        return parse_primitive(arena, str, start, final_index);
    }
}

option<node*, error> parse_file_to_node_tree(arena& arena, std::string filename) {
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
    clearspace(contents);

    if (contents[0] != '{' || contents[contents.size() - 1] != '}') {
        return { error { "Malformed JSON. File should be enclosed by an object using curly braces." } };
    }

    return parse(arena, contents, 0, contents.length() - 1);
}

option<node*, error> read_scene_from_file(arena& arena, std::string filename) {
    
    option<node*, error> result = parse_file_to_node_tree(arena, filename);

    if (std::holds_alternative<error>(result)) {
        std::cout << "Error: " << std::get<error>(result).message << std::endl;
        return result;
    }

    return std::get<node*>(result);
}




// 
// 
// Serialisation
// 
// 

