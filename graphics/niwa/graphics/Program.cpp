/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "Program.h"

#include <gl/glew.h>

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>

#include "niwa/logging/Logger.h"

namespace niwa {
    namespace graphics {
        static logging::Logger log(typeid(Program));

        Program* Program::create(
            std::string const& vertexShaderFilename, 
            std::string const& fragmentShaderFilename) {
            boost::shared_ptr<Shader<SHADER_VERTEX>> vertexShader(
                new Shader<SHADER_VERTEX>(vertexShaderFilename));

            boost::shared_ptr<Shader<SHADER_FRAGMENT>> fragmentShader(
                new Shader<SHADER_FRAGMENT>(fragmentShaderFilename));

            return new Program(vertexShader, fragmentShader);
        }

        Program::Program(
            boost::shared_ptr<Shader<SHADER_VERTEX>> vertexShader,
            boost::shared_ptr<Shader<SHADER_FRAGMENT>> fragmentShader) 
        : vertexShader_(vertexShader), fragmentShader_(fragmentShader), handle_(0) {
            handle_ = glCreateProgram();

            if(handle_) {
                glAttachShader(handle_, vertexShader->getHandle());
                glAttachShader(handle_, fragmentShader->getHandle());

                glLinkProgram(handle_);

                GLint result;

                glGetProgramiv(handle_, GL_LINK_STATUS, &result);

                if(result == GL_FALSE) {
                    char* msg = new char[512 + 1];
                    int length;

                    glGetProgramInfoLog(handle_, 512, &length, msg);
                    msg[length] = 0;

                    log.warn() << "Linking failed. Reason: " << msg;
                }
            } else {
                log.warn() << "Could not create program.";
            }
        }

        Program::~Program() {
            glDeleteProgram(handle_);
        }

        unsigned int Program::getHandle() const {
            return handle_;
        }

        std::set<std::string>& Program::getWarnedNames() const {
            return warnedNames_;
        }

        Program::ScopedUse::ScopedUse(Program const& program) 
            : currentProgram_(program), previousProgram_(0) {
            glGetIntegerv(
                GL_CURRENT_PROGRAM, 
                reinterpret_cast<GLint*>(&previousProgram_));

            glUseProgram(program.getHandle());
        }

        Program::ScopedUse::~ScopedUse() {
            glUseProgram(previousProgram_);
        }

        static GLint getUniformLocation(
                GLint program, std::string const& name, std::set<std::string>& warnedNames) {
            GLint location = glGetUniformLocation(program, name.c_str());

            if(location == -1) {
                if(warnedNames.find(name) == warnedNames.end()) {
                    log.warn() << "Cannot find variable " << name;

                    warnedNames.insert(name);
                }
            }

            return location;
        }

        void Program::ScopedUse::setIntValue(std::string const& name, int value) {
            GLint location = getUniformLocation(
                currentProgram_.getHandle(), name, currentProgram_.getWarnedNames());

            if(location != -1) {
                glUniform1i(location, value);
            }
        }

        void Program::ScopedUse::setFloatValue(std::string const& name, float value) {
            GLint location = getUniformLocation(
                currentProgram_.getHandle(), name, currentProgram_.getWarnedNames());

            if(location != -1) {
                glUniform1f(location, value);
            }
        }

        void Program::ScopedUse::setIntValues(std::string const& name, std::vector<int> const& values) {
            GLint location = getUniformLocation(
                currentProgram_.getHandle(), name, currentProgram_.getWarnedNames());

            if(location != -1) {
                switch(values.size()) {
                case 1:
                    glUniform1iv(location, 1, &values[0]);
                    break;
                case 2:
                    glUniform2iv(location, 1, &values[0]);
                    break;
                case 3:
                    glUniform3iv(location, 1, &values[0]);
                    break;
                case 4:
                    glUniform4iv(location, 1, &values[0]);
                    break;
                default:
                    std::set<std::string>& warnedNames = 
                        currentProgram_.getWarnedNames();

                    if(warnedNames.find(name) != warnedNames.end()) {
                        log.warn() << "Unsupported vector dimension ( " 
                                   << values.size() 
                                   << ") for variable " << name;

                        warnedNames.insert(name);
                    }
                    break;
                }
            }
        }

        void Program::ScopedUse::setFloatValues(
            std::string const& name, std::vector<float> const& values) {
            GLint location = getUniformLocation(
                currentProgram_.getHandle(), name, currentProgram_.getWarnedNames());

            if(location != -1) {
                switch(values.size()) {
                case 1:
                    glUniform1fv(location, 1, &values[0]);
                    break;
                case 2:
                    glUniform2fv(location, 1, &values[0]);
                    break;
                case 3:
                    glUniform3fv(location, 1, &values[0]);
                    break;
                case 4:
                    glUniform4fv(location, 1, &values[0]);
                    break;
                default:
                    std::set<std::string>& warnedNames = 
                        currentProgram_.getWarnedNames();

                    if(warnedNames.find(name) != warnedNames.end()) {
                        log.warn() << "Unsupported vector dimension ( " 
                            << values.size() 
                            << ") for variable " << name;

                        warnedNames.insert(name);
                    }
                    break;
                }
            }
        }

        void Program::ScopedUse::setMatrix(
            std::string const& name, std::pair<int,int> dim, bool isRowMajor,
            std::vector<float> const& values) {
            GLint location = getUniformLocation(
                currentProgram_.getHandle(), name, currentProgram_.getWarnedNames());

            if(location == -1) {
                return;
            }

            if(dim.first != dim.second) {
                std::set<std::string>& warnedNames = 
                    currentProgram_.getWarnedNames();

                if(warnedNames.find(name) != warnedNames.end()) {
                    log.warn() << "Unsupported non-rectangular dimension "
                        << dim.first << "x" << dim.second
                        << ") for variable " << name;

                    warnedNames.insert(name);
                }
            } else {
                int n = dim.first;

                if(n==2) {
                    glUniformMatrix2fv(location, 1, isRowMajor, &values[0]);
                } else if(n==3) {
                    glUniformMatrix3fv(location, 1, isRowMajor, &values[0]);
                } else if(n==4) {
                    glUniformMatrix4fv(location, 1, isRowMajor, &values[0]);
                } else {
                    std::set<std::string>& warnedNames = 
                        currentProgram_.getWarnedNames();

                    if(warnedNames.find(name) != warnedNames.end()) {
                        log.warn() << "Unsupported matrix dimension ( " 
                            << n << "x" << n
                            << ") for variable " << name;

                        warnedNames.insert(name);
                    }
                }
            }
        }
    }
}
