#include "Mesh.h"


#include "stb_image.h"

Mesh::Mesh(const char* path) {
    //TextureID  = glGetUniformLocation(programID, "texture_sampler");

    std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals;
    std::vector<unsigned short> indices;
	AssimpLoadFile(path, vertices, normals, indices);


	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	//indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
    //AssimpLoadFile(path, indexed_vertices, indexed_uvs, indexed_normals, indices);

    indices_count = vertices.size();

    //Texture
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, numComponents;
    unsigned char* imageData;

    imageData = stbi_load("capsule0.jpeg", &width, &height, &numComponents, 3);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

    stbi_image_free(imageData);

    //VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
#if 0
	glGenBuffers(1, &uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
#endif
	glGenBuffers(1, &normal_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
#if 0
	glGenBuffers(1, &element_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);
#endif
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &vao);
}

void Mesh::Draw(GLuint shader) {
    glBindVertexArray(vao);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader, "texture_sampler"), 0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    //glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_SHORT, (void*)0 );
	glDrawArrays(GL_TRIANGLES, 0, indices_count);

    glBindVertexArray(0);
}
