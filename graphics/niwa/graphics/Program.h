/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_GRAPHICS_PROGRAM_H
#define NIWA_GRAPHICS_PROGRAM_H

#include "Shader.h"

#include <boost/shared_ptr.hpp>

#include <set>
#include <string>
#include <vector>

namespace niwa {
    namespace graphics {
        /**
         * A shading program.
         */
        class Program {
        public:
            class ScopedUse;

        public:
            /**
             * A file-loading constructor for convenience.
             */
            static Program* create(
                std::string const& vertexShaderFilename,
                std::string const& fragmentShaderFilename);

            Program(
                boost::shared_ptr<Shader<SHADER_VERTEX>> vertexShader,
                boost::shared_ptr<Shader<SHADER_FRAGMENT>> fragmentShader);

            ~Program();

        private:
            unsigned int getHandle() const;

            /**
             * @return The set of variable names for which
             *         a warning has already been issued.
             */
            std::set<std::string>& getWarnedNames() const;

        private: // prevent copying
            Program(Program const&);
            Program& operator = (Program const&);

        private:
            boost::shared_ptr<Shader<SHADER_VERTEX>> vertexShader_;
            boost::shared_ptr<Shader<SHADER_FRAGMENT>> fragmentShader_;

            unsigned int handle_;

            mutable std::set<std::string> warnedNames_;

            friend class ScopedUse;
        };

        /**
         * A RAII session for shading program use.
         */
        class Program::ScopedUse {
        public:
            ScopedUse(Program const& program);
            ~ScopedUse();

            /**
             * Assigns a uniform GLSL variable with an integer value.
             */
            void setIntValue(std::string const& name, int value);

            /**
             * Assigns a uniform GLSL variable with a floating point value.
             */
            void setFloatValue(std::string const& name, float value);

            /**
             * Assigns a uniform GLSL variable with integer values.
             */
            void setIntValues(std::string const& name, std::vector<int> const& values);

            /**
             * Assigns a uniform GLSL variable with floating point values.
             */
            void setFloatValues(std::string const& name, std::vector<float> const& values);

            /**
             * Assigns a uniform GLSL variable with a matrix.
             *
             * @param dim Dimensions between 2x2 and 4x4 are supported.
             *            Non-rectangular dimensions are not supported.
             *
             * @param isRowMajor Whether the values are provided in row-major format.
             */
            void setMatrix(std::string const& name, std::pair<int,int> dim,
                bool isRowMajor, std::vector<float> const& values);

        private: // prevent copying
            ScopedUse(ScopedUse const&);
            ScopedUse& operator = (ScopedUse const&);

        private:
            Program const& currentProgram_;

            unsigned int previousProgram_;
        };
    }
}

#endif