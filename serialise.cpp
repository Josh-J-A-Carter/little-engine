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

class node {
    public:
        virtual void print() {}
};

class primitive_node : public node {
    public:
        std::string entry {};

        void print() {
            std::cout << entry << std::endl;
        }
};

class array_node : public node {
    public:
        std::vector<node*> entries;

        void print() {
            std::cout << "[" << std::endl;
            for (node* n : entries) n->print();
            std::cout << "]" << std::endl;
        }
};

class object_node : public node {
    public:
        std::vector<std::pair<std::string, node*>> attributes;

        void print() {
            std::cout << "{" << std::endl;
            for (std::pair<std::string_view, node*> attr : attributes) {
                std::cout << attr.first << ":::" << std::endl;
                attr.second->print();
            }
            std::cout << "}" << std::endl;
        }
};


option<node*, error> parse(arena& arena, std::string_view str, int start, int end);

option<node*, error> parse_primitive(arena& arena, std::string_view str, int start, int end) {
    if (start > end) return { error { "Empty primitive." } };
    // Basically the base-case; just need to first check syntax
    error err { "Invalid character in value '" + std::string(str.substr(start, start + 1 - end)) + "', in key-value pair." };
    for (int i = start ; i <= end ; i += 1) {
        if (str[i] == ':') return { err };
        if (str[i] == '{') return { err };
        if (str[i] == '}') return { err };
        if (str[i] == '[') return { err };
        if (str[i] == ']') return { err };
        if (str[i] == ',') return { err };
    }

    primitive_node* n { arena.allocate<primitive_node>() };
    n->entry = str.substr(start, end + 1 - start);

    return { n };
}

option<node*, error> parse_array(arena& arena, std::string_view str, int start, int end) {
    if (start >= end) return { error { "Empty array." } };

    // Find all pairs of indices in the string so that each pair encapsulates exactly one object
    std::stack<char> brace_stack {};
    array_node* n { arena.allocate<array_node>() };

    int entry_begin = start;

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
                if (entry_begin >= i) return { error { "Empty array entry." } };

                option<node*, error> result = parse(arena, str, entry_begin, i - 1);
                if (std::holds_alternative<error>(result)) return result;

                n->entries.push_back(std::get<node*>(result));
                entry_begin = i + 1;
            }

            else if (i == end && entry_begin <= i) {
                option<node*, error> result = parse(arena, str, entry_begin, i);
                if (std::holds_alternative<error>(result)) return result;

                n->entries.push_back(std::get<node*>(result));
            }
        }
    }

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
        n->attributes.push_back({ std::string(str.substr(pair.first, pos_first - pair.first)), value });
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

    if (str[start] == '{' && str[final_index] == '}') {
        return parse_object(arena, str, start + 1, final_index - 1);
    }

    else if (str[start] == '[' && str[final_index] == ']') {
        return parse_array(arena, str, start + 1, final_index - 1);
    }

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

void read_scene_from_file(arena& arena, std::string filename) {
    
    option<node*, error> result = parse_file_to_node_tree(arena, filename);
    std::cout << "Completed parsing" << std::endl;
    
    if (std::holds_alternative<error>(result)) {
        std::cout << "Error: " << std::get<error>(result).message << std::endl;
        return;
        // return result;
    }
    
    std::cout << "Got node structure:" << std::endl;
    std::get<node*>(result)->print();

    // return std::get<node*>(result);
}


