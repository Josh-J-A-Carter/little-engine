#ifndef MESH_H
#define MESH_H

#include <map>
#include <vector>

#include <glad/glad.h>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "pipeline.h"
#include "texture.h"
#include "material.h"
#include "transform.h"

#define INVALID_MATERIAL 0xFFFFFFFF

struct mesh {
    public:
        mesh() {};

        void load(const std::string& file_name);

        void render();

        material& get_ambient_material();

        transform& get_transform() { return m_transform; }


    private:

        struct mesh_entry {
            unsigned int num_indices { 0 };
            unsigned int base_vertex { 0 };
            unsigned int base_index { 0 };
            unsigned int material_index { INVALID_MATERIAL };
        };

        enum BUFFER_TYPE {
            INDEX_BUFFER = 0,
            V_POS_BUFFER = 1,
            V_TEX_BUFFER = 2,
            V_NORM_BUFFER = 3,
            MVP_MAT_BUFFER = 4,  // required only for instancing
            MODEL_MAT_BUFFER = 5,  // required only for instancing
            NUM_BUFFERS = 6
        };

        void init_from_scene(const aiScene* p_scene, const std::string& file_name);

        void count_vertices_and_indices(const aiScene* p_scene, unsigned int& num_vertices, unsigned int& num_indices);

        void reserve_space(unsigned int mum_vertices, unsigned int num_indices);

        void init_all_meshes(const aiScene* p_scene);
        
        void init_single_mesh(unsigned int mesh_index, const aiMesh* p_ai_mesh);

        void init_materials(const aiScene* p_scene, const std::string& file_name);
        
        void populate_buffers();
        

        std::vector<mesh_entry> m_meshes {};
        std::vector<unsigned int> m_indices {};
        GLuint m_VAO { 0 };
        GLuint m_buffers[NUM_BUFFERS] = { 0 };

        std::vector<texture*> m_textures {};
        std::vector<material> m_materials {};

        std::vector<glm::vec3> m_vert_positions {};
        std::vector<glm::vec2> m_vert_texcoords {};
        std::vector<glm::vec3> m_vert_normals {};

        transform m_transform;
};


#endif
