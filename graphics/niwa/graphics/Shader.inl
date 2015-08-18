/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef NIWA_GRAPHICS_SHADER_INL
#define NIWA_GRAPHICS_SHADER_INL

namespace niwa {
    namespace graphics {
        template <ShaderType T>
        Shader<T>::Shader(std::string filename) : impl_(T, filename) {
            // ignored
        }

        template <ShaderType T>
        unsigned int Shader<T>::getHandle() const {
            return impl_.getHandle();
        }
    }
}

#endif