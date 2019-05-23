#pragma once

#include <flat_hash_map.hpp>

#include <glm/glm.hpp>

#include "SingletonMacro.hpp"

namespace grx {
    enum class ShaderType {
        Vertex,
        Fragment
    };
}

namespace grx_sl {
    struct Program {
        ska::flat_hash_map<std::string, int> uniforms;
    };

    class ShaderLoader {
    public:
        unsigned int loadShader(const char *shader_path, grx::ShaderType shaderType);
        unsigned int load      (const char *vertex_shader_path, const char *fragment_shader_path);

        int getUniformId (unsigned programID, const char* name);
        int uniformId    (unsigned programID, const char* name);

        template <typename... T>
        void uniform(unsigned programID, int ID, const T&... values);

        template <typename... T>
        void uniform(unsigned programID, const char* name, const T&... values) {
            uniform(programID, uniformId(programID, name), values...);
        }

    protected:
        ska::flat_hash_map<unsigned, Program> programs;

        MARK_AS_SINGLETON(ShaderLoader);
    };
}




// Todo: specialize uniform template for all types
// Uniform template method

#define GENERATE_UNIFORM(TYPE) \
template <> \
void grx_sl::ShaderLoader::uniform<TYPE>(unsigned, int, const TYPE&); \
template <> \
void grx_sl::ShaderLoader::uniform<TYPE, TYPE>(unsigned, int, const TYPE&, const TYPE&); \
template <> \
void grx_sl::ShaderLoader::uniform<TYPE, TYPE, TYPE>(unsigned, int, const TYPE&, const TYPE&, const TYPE&); \
template <> \
void grx_sl::ShaderLoader::uniform<TYPE, TYPE, TYPE, TYPE> \
        (unsigned, int, const TYPE&, const TYPE&, const TYPE&, const TYPE&) \


GENERATE_UNIFORM(float);
GENERATE_UNIFORM(double);
GENERATE_UNIFORM(int);
GENERATE_UNIFORM(unsigned int);

#undef GENERATE_UNIFORM

template <>
void grx_sl::ShaderLoader::uniform<glm::mat4>(unsigned, int, const glm::mat4&);


namespace grx {
    /**
     * @return Shader Loader input context
     */
    inline auto &shaders() {
        return grx_sl::ShaderLoader::instance();
    }
}