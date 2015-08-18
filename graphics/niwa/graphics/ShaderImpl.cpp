/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "ShaderImpl.h"

#include "niwa/logging/Logger.h"

#include <gl/glew.h>

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>

#include <fstream>
#include <string>

namespace niwa {
    namespace graphics {
        static logging::Logger log(typeid(ShaderImpl));

        static void loadShader(GLuint shader, std::string filename) {
            std::ifstream ifs(filename.c_str());

            if(!ifs.is_open()) {
                log.warn() << "Cannot load " << filename;
                return;
            }

            std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

            char const* chars = str.c_str();

            glShaderSource(shader, 1, &chars, NULL);

            glCompileShader(shader);

            GLint result;

            glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

            if(result == GL_FALSE) {
                char* msg = new char[512 + 1];
                int length;

                glGetShaderInfoLog(shader, 512, &length, msg);
                msg[length] = 0;

                log.warn() << "Compiling " << filename << " failed. Reason: " << msg;
            }

            ifs.close();
        }

        ShaderImpl::ShaderImpl(ShaderType shaderType, std::string filename) : handle_(0) {
            handle_ = glCreateShader(
                shaderType == SHADER_VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);

            if(handle_) {
                loadShader(handle_, filename);
            } else {
                log.warn() << "Could not create shader for " << filename;
            }
        }

        ShaderImpl::~ShaderImpl() {
            glDeleteShader(handle_);
        }

        unsigned int ShaderImpl::getHandle() const {
            return handle_;
        }
    }
}