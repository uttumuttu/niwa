/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_GRAPHICS_SHADERIMPL_H
#define NIWA_GRAPHICS_SHADERIMPL_H

#include "ShaderType.h"

#include <string>

namespace niwa {
    namespace graphics {
        template <ShaderType T>
        class Shader;

        /**
         * A non-typed shader.
         */
        class ShaderImpl {
        public:
            ShaderImpl(ShaderType shaderType, std::string filename);
            ~ShaderImpl();

        private:
            unsigned int getHandle() const;

        private: // prevent copying
            ShaderImpl(ShaderImpl const&);
            ShaderImpl& operator = (ShaderImpl const&);

        private:
            unsigned int handle_;

            friend class Shader<SHADER_VERTEX>;
            friend class Shader<SHADER_FRAGMENT>;
        };
    }
}

#endif