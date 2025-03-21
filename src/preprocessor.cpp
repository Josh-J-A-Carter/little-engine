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

    // 
    // Read files to find the types that need code generated; this is signaled with the macro 'REGISTER_PARSE_REF'
    // 

    // Iterate through args = first arg is this file's name, the rest are
    // relative paths to all the files we need to process.
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
            
            // If the match includes a #define, then it doesn't count
            for (auto submatch : match) {
                if (submatch == "#define") {
                    allowable = false;
                    break;
                }
    
                matches.push_back(submatch);
            }
            
            // We expect the second-to-last match to be the name of the type to use
            if (allowable && matches.size() >= 3) types.push_back(matches.at(matches.size() - 2));
    
            contents = match.suffix().str();
        }
    }

    // 
    // Generate the code, with statements for each type
    // 

    // .cpp file
    std::string parse_types_cpp_template =
        "#include <string>\n"
        "#include <optional>"
        "\n"
        "#include \"serialise.h\"\n"
        "#include \"scene_node.h\"\n"
        "#include \"utilities.h\"\n"
        "#include \"parse_types.h\"\n\n"

        "// Dynamically generated includes\n"


        "{{dynamic-includes}}\n"

        "struct pipeline;\n"
        "\n"
        "namespace serial {\n"
        "    std::optional<error> deserialise_type(arena& arena, scene_node* sc, scene_node* root, node* n, std::string type) {\n"
        "        if (type == \"empty\") {\n"
        "            sc->component_type = scene_node_type::empty;\n"
        "            return std::nullopt;\n"
        "        }\n\n"

        "        // Dynamic else-ifs\n"


                "{{dynamic-else-ifs}}\n"


        "        return error { \"Type '\" + type + \"' not recognised when deserialising JSON to scene node.\" };\n"
        "    }\n"
        "\n"
        "    void serialise_node(std::ostream& os, const scene_node* sc, int indt) {\n"
        "        if (sc->component_type == scene_node_type::empty) os << \"{ type: empty }\";\n\n"
        
        "        // Dynamic serialisation stuff; scene nodes don't know how to serialise their component\n"
        "        // as they don't know its type. Why didn't I just use polymorphism to be honest??? Too late!\n"


                "{{dynamic-serialisation}}"


        "    }\n"
        "}\n";

    std::string dynamic_includes {};
    std::string dynamic_includes_template = "#include \"{{type}}.h\"\n";

    std::string dynamic_else_ifs {};
    std::string dynamic_else_ifs_template =
        "        else if (type == \"{{type}}\") {\n"
        "            option<{{type}}*, error> res = deserialise_ref<{{type}}>(arena, root, n);\n"
        "            if (std::holds_alternative<error>(res)) return std::get<error>(res);\n"
        "            {{type}}* obj = std::get<{{type}}*>(res);\n"
        "            sc->component_type = scene_node_type::{{type}};\n"
        "            sc->component = obj;\n"
        "            sc->cmp_load = [](scene_node* sc) { load(static_cast<{{type}}*>(sc->component)); };\n"
        "            sc->cmp_run = [](scene_node* sc) { run(static_cast<{{type}}*>(sc->component)); };\n"
        "            sc->cmp_render = [](scene_node* sc, pipeline& p) { render(static_cast<{{type}}*>(sc->component), p); };\n"
        "            return std::nullopt;\n"
        "        }\n\n";

    std::string dynamic_serialisation {};
    std::string dynamic_serialisation_template =
        "        else if (sc->component_type == scene_node_type::{{type}}) {\n"
        "            serialise(os, *static_cast<{{type}}*>(sc->component), sc, indt);\n"
        "        }\n\n";

    for (std::string type : types) {
        dynamic_includes += replace_all(dynamic_includes_template, "{{type}}", type);
        dynamic_else_ifs += replace_all(dynamic_else_ifs_template, "{{type}}", type);
        dynamic_serialisation += replace_all(dynamic_serialisation_template, "{{type}}", type);
    }

    std::string parse_types_cpp = replace_all(
        replace_all(
            replace_all(parse_types_cpp_template, "{{dynamic-includes}}", dynamic_includes),
            "{{dynamic-serialisation}}", dynamic_serialisation
        ),
        "{{dynamic-else-ifs}}", dynamic_else_ifs
    );

    // Overwrite the existing parse_types.cpp file
    std::ofstream out_cpp { "./src/parse_types.cpp" };
    out_cpp << parse_types_cpp << std::endl;
    out_cpp.close();

    // .h file
    std::string parse_types_h_template =
        "#ifndef PARSE_TYPES_H\n"
        "#define PARSE_TYPES_H\n"
        "\n"
        "enum class scene_node_type {\n"
        "    empty,\n"
        "\n"
        "    // Dynamic generation\n"


            "{{dynamic-enums}}"


        "};\n"
        "\n"
        "#endif\n";

    std::string dynamic_enums {};
    std::string dynamic_enums_template = "    {{type}},\n";

    for (std::string type : types) {
        dynamic_enums += replace_all(dynamic_enums_template, "{{type}}", type);
    }

    std::string parse_types_h = replace_all(parse_types_h_template, "{{dynamic-enums}}", dynamic_enums);

    // Overwrite the existing parse_types.h file
    std::ofstream out_h { "./include/parse_types.h" };
    out_h << parse_types_h << std::endl;
    out_h.close();

    // declarations file
    std::string parse_declarations_template =
        "#ifndef PARSE_DECLARATIONS_H\n"
        "#define PARSE_DECLARATIONS_H\n"
        "\n"
        "#include <ostream>\n"
        "#include \"scene_node.h\"\n"
        "\n"

            "{{type-declarations}}\n"

        "\n"
        "namespace serial {\n"

            "{{function-declarations}}\n"

        "}\n"
        "\n"
        "#endif\n";

    std::string type_declarations {};
    std::string type_declarations_template = "struct {{type}};\n";

    std::string function_declarations {};
    std::string function_declarations_template =
        "    void serialise(std::ostream& os, const {{type}}& obj, const scene_node* sc, int indt);\n";

    for (std::string type : types) {
        type_declarations += replace_all(type_declarations_template, "{{type}}", type);
        function_declarations += replace_all(function_declarations_template, "{{type}}", type);
    }

    std::string parse_declarations = replace_all(
        replace_all(parse_declarations_template, "{{type-declarations}}", type_declarations),
        "{{function-declarations}}", function_declarations
    );

    // Overwrite the existing parse_declarations.h file
    std::ofstream out_d { "./include/parse_declarations.h" };
    out_d << parse_declarations << std::endl;
    out_d.close();

    std::cout << "Code generation complete." << std::endl;
}
