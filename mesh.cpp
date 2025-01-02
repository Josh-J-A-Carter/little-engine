#include <vector>
#include <string>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "pipeline.h"
#include "texture.h"
#include "material.h"
#include "utilities.h"

#define POSITION_LOCATION  0
#define TEX_COORD_LOCATION 1
#define NORMAL_LOCATION    2

#define COLOR_TEXTURE_UNIT GL_TEXTURE0

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)


std::string get_full_path(const std::string& dir, const aiString& path) {
    std::string p { path.data };

    if (p == "C:\\\\") p = "";

    else if (p.substr(0, 2) == ".\\") p = p.substr(2, p.size() - 2);

    std::string full_path = dir + "/" + p;

    return full_path;
}



void mesh::load(const std::string& file_name) {
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(ARRAY_SIZE(m_buffers), m_buffers);

    Assimp::Importer importer {};
    const aiScene* p_scene = importer.ReadFile(file_name, ASSIMP_LOAD_FLAGS);

    if (!p_scene) {
        std::cout << "Error - unable to read file \"" << file_name << "\"" << std::endl;
        std::cout << "Message: " << importer.GetErrorString() << std::endl;
        exit(EXIT_FAILURE);
    }

    init_from_scene(p_scene, file_name);

    glBindVertexArray(0);
}

void mesh::init_from_scene(const aiScene* p_scene, const std::string& file_name) {
    m_meshes.resize(p_scene->mNumMeshes);
    m_textures.resize(p_scene->mNumMaterials);
    m_materials.resize(p_scene->mNumMaterials);

    unsigned int num_vertices = 0;
    unsigned int num_indices = 0;

    count_vertices_and_indices(p_scene, num_vertices, num_indices);

    reserve_space(num_vertices, num_indices);

    init_all_meshes(p_scene);

    init_materials(p_scene, file_name);

    populate_buffers();

    gl_error_check_barrier
}


void mesh::count_vertices_and_indices(const aiScene* pScene, unsigned int& num_vertices, unsigned int& num_indices) {
    for (unsigned int i { 0 }; i < m_meshes.size() ; i += 1) {
        m_meshes[i].material_index = pScene->mMeshes[i]->mMaterialIndex;
        m_meshes[i].num_indices = pScene->mMeshes[i]->mNumFaces * 3;
        m_meshes[i].base_vertex = num_vertices;
        m_meshes[i].base_index = num_indices;

        num_vertices += pScene->mMeshes[i]->mNumVertices;
        num_indices  += m_meshes[i].num_indices;
    }
}


void mesh::reserve_space(unsigned int num_vertices, unsigned int num_indices) {
    m_vert_positions.reserve(num_vertices);
    m_vert_texcoords.reserve(num_vertices);
    m_vert_normals.reserve(num_vertices);
    m_indices.reserve(num_indices);
}


void mesh::init_all_meshes(const aiScene* p_scene) {
    for (unsigned int i { 0 } ; i < m_meshes.size() ; i += 1) {
        const aiMesh* p_ai_mesh = p_scene->mMeshes[i];
        init_single_mesh(i, p_ai_mesh);
    }
}


void mesh::init_single_mesh(unsigned int mesh_index, const aiMesh* p_ai_mesh) {
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int i = 0; i < p_ai_mesh->mNumVertices ; i += 1) {
        const aiVector3D& p_pos = p_ai_mesh->mVertices[i];
        const aiVector3D& p_normal = p_ai_mesh->mNormals[i];
        const aiVector3D& p_texcoord = p_ai_mesh->HasTextureCoords(0) 
                                            ? p_ai_mesh->mTextureCoords[0][i] : Zero3D;

        m_vert_positions.push_back(glm::vec3(p_pos.x, p_pos.y, p_pos.z));
        m_vert_texcoords.push_back(glm::vec2(p_texcoord.x, p_texcoord.y));
        m_vert_normals.push_back(glm::vec3(p_normal.x, p_normal.y, p_normal.z));
    }

    // Populate the index buffer
    for (unsigned int i { 0 } ; i < p_ai_mesh->mNumFaces ; i += 1) {
        const aiFace& Face = p_ai_mesh->mFaces[i];
        m_indices.push_back(Face.mIndices[0]);
        m_indices.push_back(Face.mIndices[1]);
        m_indices.push_back(Face.mIndices[2]);
    }
}

void mesh::init_materials(const aiScene* p_scene, const std::string& file_name) {
    std::string::size_type slash_index { file_name.find_last_of("/") };
    std::string dir {};

    if (slash_index == std::string::npos) dir = ".";
    else if (slash_index == 0) dir = "/";
    else dir = file_name.substr(0, slash_index);

    for (unsigned int i { 0 } ; i < p_scene->mNumMaterials ; i += 1) {
        const aiMaterial* p_material = p_scene->mMaterials[i];

        m_textures[i] = nullptr;
        
        if (p_material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString path;

            if (p_material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
                std::string p { path.data };

                if (p.substr(0, 2) == ".\\") p = p.substr(2, p.size() - 2);
                std::string full_path { dir + "/" + p };

                m_textures[i] = new texture(GL_TEXTURE_2D, full_path.c_str());
                
                m_textures[i]->load();
            }
        }

        aiColor3D ambient_color { 0, 0, 0 };

        if (p_material->Get(AI_MATKEY_COLOR_AMBIENT, ambient_color) == AI_SUCCESS) {
            m_materials[i].ambient_color.r = ambient_color.r;
            m_materials[i].ambient_color.g = ambient_color.g;
            m_materials[i].ambient_color.b = ambient_color.b;
        }

        aiColor3D diffuse_color { 0, 0, 0 };

        if (p_material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color) == AI_SUCCESS) {
            m_materials[i].diffuse_color.r = diffuse_color.r;
            m_materials[i].diffuse_color.g = diffuse_color.g;
            m_materials[i].diffuse_color.b = diffuse_color.b;
        }
    }
}


void mesh::populate_buffers() {
    GLsizeiptr vert_pos_bytes = sizeof(m_vert_positions[0]) * m_vert_positions.size();
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[V_POS_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, vert_pos_bytes, m_vert_positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLsizeiptr vert_tex_bytes = sizeof(m_vert_texcoords[0]) * m_vert_texcoords.size();
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[V_TEX_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, vert_tex_bytes, m_vert_texcoords.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

    GLsizeiptr vert_norm_bytes = sizeof(m_vert_normals[0]) * m_vert_normals.size();
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[V_NORM_BUFFER]);
    glBufferData(GL_ARRAY_BUFFER, vert_norm_bytes, m_vert_normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLsizeiptr index_bytes = sizeof(m_indices[0]) * m_indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_bytes, m_indices.data(), GL_STATIC_DRAW);
}


void mesh::render() {
    glBindVertexArray(m_VAO);

    for (unsigned int i { 0 } ; i < m_meshes.size() ; i += 1) {
        const unsigned int material_index = m_meshes[i].material_index;

        assert(material_index < m_textures.size());

        if (m_textures[material_index]) {
            m_textures[material_index]->bind(COLOR_TEXTURE_UNIT);
        }

        glDrawElementsBaseVertex(GL_TRIANGLES, m_meshes[i].num_indices, GL_UNSIGNED_INT,
                            (void*) (sizeof(m_indices[0]) * m_meshes[i].base_index), m_meshes[i].base_vertex);
    }

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);
}

material& mesh::get_material() {
    for (material& material : m_materials) {
        if (material.ambient_color != glm::vec3 { 0, 0, 0 }) return material;
    }

    return m_materials[0];
}