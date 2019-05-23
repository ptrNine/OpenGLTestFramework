#include "ShaderLoader.hpp"

#include <fstream>

#include <GL/glew.h>


grx_sl::ShaderLoader::ShaderLoader() {}

grx_sl::ShaderLoader::~ShaderLoader() {
    for (auto& p : programs)
        glDeleteProgram(p.first);
}

unsigned int grx_sl::ShaderLoader::loadShader(const char* sp, grx::ShaderType st) {
    unsigned shType = GL_VERTEX_SHADER;
    switch (st) {
        case grx::ShaderType::Fragment: shType = GL_FRAGMENT_SHADER; break;
        case grx::ShaderType::Vertex:   shType = GL_VERTEX_SHADER;   break;
        default: break; // todo: assert
    }

    GLuint ID = glCreateShader(shType);

    // Todo: assert if can't open!
    std::ifstream shIfs(sp, std::ios::in | std::ios::ate);
    auto shIfsEnd = shIfs.tellg();
    shIfs.seekg(0, std::ios::beg);
    auto shCodeSize = shIfsEnd - shIfs.tellg();

    char* shCode = new char[shCodeSize + 1];
    shCode[shCodeSize] = '\0';
    shIfs.read(shCode, shCodeSize);
    shIfs.close();

    // Todo: compilation log
    glShaderSource(ID, 1, &shCode, nullptr);
    glCompileShader(ID);


    GLint rc = GL_FALSE;
    int infoLogLength;

    // Todo: check log
    glGetShaderiv(ID, GL_COMPILE_STATUS, &rc);
    glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        char* errorMsg = new char[infoLogLength + 1];
        errorMsg[infoLogLength] = '\0';
        glGetShaderInfoLog(ID, infoLogLength, nullptr, errorMsg);
        fprintf(stderr, "%sn", errorMsg);
        delete [] errorMsg;
    }

    delete [] shCode;

    return ID;
}

unsigned int grx_sl::ShaderLoader::load(const char* vsp, const char* fsp) {
    auto vsID = loadShader(vsp, grx::ShaderType::Vertex);
    auto fsID = loadShader(fsp, grx::ShaderType::Fragment);

    // Todo: log: create shader program
    GLuint ID = glCreateProgram();
    glAttachShader(ID, vsID);
    glAttachShader(ID, fsID);
    glLinkProgram(ID);

    GLint rc = GL_FALSE;
    int infoLogLength;

    // Todo: check log
    glGetProgramiv(ID, GL_LINK_STATUS, &rc);
    glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        char* errorMsg = new char[infoLogLength + 1];
        errorMsg[infoLogLength] = '\0';
        glGetProgramInfoLog(ID, infoLogLength, nullptr, errorMsg);
        fprintf(stderr, "%sn", errorMsg);
        delete [] errorMsg;
    }

    glDeleteShader(vsID);
    glDeleteShader(fsID);

    programs.emplace(ID, Program());

    return ID;
}

int grx_sl::ShaderLoader::getUniformId(unsigned prgID, const char* name) {
    auto foundProgram = programs.find(prgID);

    // Todo: assert if can't found program

    return foundProgram->second.uniforms[name];
}

int grx_sl::ShaderLoader::uniformId(unsigned prgID, const char* name) {
    auto foundProgram = programs.find(prgID);

    // Todo: assert if can't found program

    auto& program = foundProgram->second;

    auto find = program.uniforms.find(name);
    if (find != program.uniforms.end())
        return find->second;
    else {
        int id = glGetUniformLocation(prgID, name);
        return program.uniforms[name] = id;
    }
}


// Todo: specialize uniform template for all types
// Uniform template method specializations

#define GENERATE_UNIFORM_1(TYPE) \
template <> \
void grx_sl::ShaderLoader::uniform(unsigned prgID, int ID, const TYPE& v1)

#define GENERATE_UNIFORM_2(TYPE) \
template <> \
void grx_sl::ShaderLoader::uniform(unsigned prgID, int ID, const TYPE& v1, const TYPE& v2)

#define GENERATE_UNIFORM_3(TYPE) \
template <> \
void grx_sl::ShaderLoader::uniform(unsigned prgID, int ID, const TYPE& v1, const TYPE& v2, const TYPE& v3)

#define GENERATE_UNIFORM_4(TYPE) \
template <> \
void grx_sl::ShaderLoader::uniform(unsigned prgID, int ID, const TYPE& v1, const TYPE& v2, const TYPE& v3, const TYPE& v4)


GENERATE_UNIFORM_1(float) { glProgramUniform1f(prgID, ID, v1); }
GENERATE_UNIFORM_2(float) { glProgramUniform2f(prgID, ID, v1, v2); }
GENERATE_UNIFORM_3(float) { glProgramUniform3f(prgID, ID, v1, v2, v3); }
GENERATE_UNIFORM_4(float) { glProgramUniform4f(prgID, ID, v1, v2, v3, v4); }

GENERATE_UNIFORM_1(int) { glProgramUniform1i(prgID, ID, v1); }
GENERATE_UNIFORM_2(int) { glProgramUniform2i(prgID, ID, v1, v2); }
GENERATE_UNIFORM_3(int) { glProgramUniform3i(prgID, ID, v1, v2, v3); }
GENERATE_UNIFORM_4(int) { glProgramUniform4i(prgID, ID, v1, v2, v3, v4); }

GENERATE_UNIFORM_1(unsigned) { glProgramUniform1ui(prgID, ID, v1); }
GENERATE_UNIFORM_2(unsigned) { glProgramUniform2ui(prgID, ID, v1, v2); }
GENERATE_UNIFORM_3(unsigned) { glProgramUniform3ui(prgID, ID, v1, v2, v3); }
GENERATE_UNIFORM_4(unsigned) { glProgramUniform4ui(prgID, ID, v1, v2, v3, v4); }

GENERATE_UNIFORM_1(double) { glProgramUniform1d(prgID, ID, v1); }
GENERATE_UNIFORM_2(double) { glProgramUniform2d(prgID, ID, v1, v2); }
GENERATE_UNIFORM_3(double) { glProgramUniform3d(prgID, ID, v1, v2, v3); }
GENERATE_UNIFORM_4(double) { glProgramUniform4d(prgID, ID, v1, v2, v3, v4); }


template <>
void grx_sl::ShaderLoader::uniform(unsigned prgID, int ID, const glm::mat4& val) {
    glProgramUniformMatrix4fv(prgID, ID, 1, GL_FALSE, &val[0][0]);
}