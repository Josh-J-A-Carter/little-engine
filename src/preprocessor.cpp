// 
// Reset template file for 'parse_types.cpp'
// Reset template file for 'parse_types.h'
// 
// For each source file (*.cpp, *.h) inside /src and /include
//   1)
//      substr find 'REGISTER_PARSE_REF'
//      if not found, close file and go to next.
//   2)   
//      found, so rewind file until:
//          - start of file -> (3)
//          - found "#define" statement -> (1)
//          - string before does not contain 'define' -> (3)
//   3)
//      successful.
//          - Create 'else if' clause in deserialise_type (parse_types.cpp)
//          - Create '#include "T.h"' (parse_types.cpp)
//          - Create 'T' in scene_node_type (parse_types.h)
//
// Confirm changes to files.
// 

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <regex>


void error(std::string message) {
    std::cout << message << std::endl;
    exit(EXIT_FAILURE);
}

std::string replace_all(const std::string& templ, const std::string& remove, const std::string& insert) {

    std::string str { templ };
    std::string::size_type pos = 0;
    while ((pos = str.find(remove, pos)) != std::string::npos) {
        str.replace(pos, remove.size(), insert);
        pos += 1;
    }

    return str;
}


int main(int argv, char** args) {
    std::cout << "Generating code for scene node reference types..." << std::endl;

    std::vector<std::string> types {};

    // Read files to find the types that need code generated; this is signaled with the macro 'REGISTER_PARSE_REF'
    for (int i = 1 ; i < argv ; i += 1) {
        std::string filename { args[i] };

        // Read entire file in
        std::ifstream file { filename };
        std::string contents {};
        std::string line {};

        if (!file.is_open()) error("Failed to open file " + filename + ", halting code generation...");

        while (std::getline(file, line)) contents += line + "\n";

        // Look for macro expansion matches
        std::smatch match;
        std::regex e { "((#define)(\\s+))?(REGISTER_PARSE_REF[\\(])([^\\(\\)]*)([\\)])" };
    
        while (std::regex_search(contents, match, e)) {
    
            bool allowable = true;
            std::vector<std::string> matches {};
    
            for (auto submatch : match) {
                if (submatch == "#define") {
                    allowable = false;
                    break;
                }
    
                matches.push_back(submatch);
            }
    
            if (allowable && matches.size() >= 3) types.push_back(matches.at(matches.size() - 2));
    
            contents = match.suffix().str();
        }
    }

    // Generate the code, with statements for each type

    std::string parse_types_cpp__template =
        "#include <string>\n"
        "#include \"serialise.h\"\n"
        "#include \"scene_node.h\"\n"
        "#include \"utilities.h\"\n"
        "#include \"parse_types.h\"\n\n"

        "// Dynamically generated includes\n"


        "{{dynamic-includes}}\n"


        "namespace serial {\n"
        "   option<scene_node*, error> deserialise_type(arena& arena, node* n, std::string type) {\n"
        "       if (type == \"empty\");\n\n"

        "       // Dynamic else-ifs\n"


        "       {{dynamic-else-ifs}}\n"


        "       return error { \"Type '\" + type + \"' not recognised when deserialising JSON to scene node.\" };\n"
        "   }\n"
        "}\n";

    std::string dynamic_includes {};
    std::string dynamic_includes_template = "#include \"{{type}}.h\"\n";

    std::string dynamic_else_ifs {};
    std::string dynamic_else_ifs_template =
        "else if (type == \"{{type}}\") {\n"
        "           option<{{type}}*, error> res = deserialise_ref<{{type}}>(arena, n);\n"
        "           if (std::holds_alternative<error>(res)) return std::get<error>(res);\n"
        "           scene_node* sc = arena.allocate<scene_node>();\n"
        "           sc->component_type = scene_node_type::{{type}};\n"
        "           sc->component = std::get<{{type}}*>(res);\n"
        "           return sc;\n"
        "       }\n";

    for (std::string type : types) {
        dynamic_includes += replace_all(dynamic_includes_template, "{{type}}", type) + "\n";
        dynamic_else_ifs += replace_all(dynamic_else_ifs_template, "{{type}}", type) + "\n";
    }

    std::string parse_types_cpp = replace_all(
        replace_all(
            parse_types_cpp__template, "{{dynamic-includes}}", dynamic_includes
        ),
        "{{dynamic-else-ifs}}", dynamic_else_ifs
    );

    std::cout << parse_types_cpp << std::endl;


    std::cout << "Code generation complete." << std::endl;
}
