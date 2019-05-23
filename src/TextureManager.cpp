#include "TextureManager.hpp"

#include <fstream>

#include <GLFW/glfw3.h>
#include <cstring>
#include <iostream>


#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII


unsigned grx_txtr::TextureManager::loadDDS(const std::string_view& path) {
    // Todo: assert in can't open
    std::ifstream img(path.data(), std::ios::in | std::ios::ate);
    auto imgEng = img.tellg();
    img.seekg(0, std::ios::beg);
    auto imgSize = imgEng - img.tellg();

    auto imgBytes = new unsigned char[imgSize + 1];
    imgBytes[imgSize] = '\0';
    img.read(reinterpret_cast<char*>(imgBytes), imgSize);
    img.close();

    // Todo: assert if 4 bytes not equal with "DDS "

    unsigned char header[124];
    std::memcpy(header, imgBytes + 4, 124);


    // Todo: endian insensitive reading
    unsigned int height      = *(unsigned int*)&(header[8 ]);
    unsigned int width       = *(unsigned int*)&(header[12]);
    unsigned int linearSize  = *(unsigned int*)&(header[16]);
    unsigned int mipMapCount = *(unsigned int*)&(header[24]);
    unsigned int fourCC      = *(unsigned int*)&(header[80]);

    unsigned int bufsize;
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    auto buffer = new unsigned char[bufsize];
    std::memcpy(buffer, imgBytes + 128, bufsize);

    unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch(fourCC)
    {
        case FOURCC_DXT1:
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break;
        case FOURCC_DXT3:
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;
        case FOURCC_DXT5:
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;
        default:
            // Todo: assert or error with dummy texture
            delete [] imgBytes;
            delete [] buffer;
            return 0;
    }


    GLuint id;
    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);

    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    for (unsigned l = 0; l < mipMapCount && (width || height); ++l) {
        unsigned size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, l, format, width, height,
                               0, size, buffer + offset);

        offset += size;
        width  /= 2;
        height /= 2;

        if(width < 1) width = 1;
        if(height < 1) height = 1;
    }

    delete [] imgBytes;
    delete [] buffer;

    return id;
}

unsigned grx_txtr::TextureManager::loadTexture(const std::string_view& path) {
    // Todo: add dummy texture if path is invalid

    auto format = path.substr(path.length() - 3, 3);

    // Todo: case insensitive
    if (format == "dds") {
        return loadDDS(path);
    }

    return 0;
}


grx_txtr::TextureManager:: TextureManager() {}
grx_txtr::TextureManager::~TextureManager() {}

unsigned grx_txtr::TextureManager::load(const char* path) {
    auto lock = std::lock_guard{load_mutex};

    auto find = textures.find(path);

    if (find != textures.end())
        return find->second;
    else {
        return textures[path] = loadTexture(path);
    }
}


unsigned grx_txtr::TextureManager::pushTexture(ska::flat_hash_map<std::string, unsigned>* map, const char* path) {
    return (*map)[path] = loadTexture(path);
}

auto grx_txtr::TextureManager::asyncLoad(const char* path) -> std::future<unsigned> {
    auto lock = std::lock_guard{load_mutex};

    auto promise = std::promise<unsigned>();

    auto find = textures.find(path);

    if (find != textures.end()) {
        promise.set_value(find->second);
        return promise.get_future();
    } else {
        promise.set_value(pushTexture(&textures, path));
        return promise.get_future();
    }
}
