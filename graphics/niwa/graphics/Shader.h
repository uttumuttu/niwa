/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef NIWA_GRAPHICS_SHADER_H
#define NIWA_GRAPHICS_SHADER_H

#include "ShaderType.h"
#include "ShaderImpl.h"

#include <string>

namespace niwa {
    namespace graphics {
        /**
         * A vertex or fragment shader.
         */
        template <ShaderType T>
        class Shader {
        public:
            Shader(std::string filename);

        private:
            unsigned int getHandle() const;

        private: // prevent copying
            Shader(Shader const&);
            Shader& operator = (Shader const&);

        private:
            ShaderImpl impl_;

            friend class Program;
        };
    }
}

#include "Shader.inl"

#endif