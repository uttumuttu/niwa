/**
 * @file
 * @author Mikko Kauppila
 *
 * Capability code is mostly copied from Microsoft's example.
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/system/ProcessorInfo.h"

#define NOMINMAX
#include <windows.h>

#include <algorithm>

#define _CPU_FEATURE_MMX    0x0001
#define _CPU_FEATURE_SSE    0x0002
#define _CPU_FEATURE_SSE2   0x0004

// This namespace is mostly copied from Microsoft's example.
namespace {
    // These are the bit flags that get set on calling cpuid
    // with register eax set to 1
    #define _MMX_FEATURE_BIT        0x00800000
    #define _SSE_FEATURE_BIT        0x02000000
    #define _SSE2_FEATURE_BIT       0x04000000

    static bool isCpuidSupported() {
        __try {
            _asm {
                xor eax, eax
                cpuid
            }
        }
        #pragma warning (suppress: 6320)
        __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
        return true;
    }

    /**
     * @param The feature we want to check if OS supports it.
     *
     * @returns Whether OS supports the capability.
     */
    static bool _os_support(int feature)
    {
        __try {
            switch (feature) {
            case _CPU_FEATURE_SSE:
                __asm {
                    xorps xmm0, xmm0        // executing SSE instruction
                }
                break;
            case _CPU_FEATURE_SSE2:
                __asm {
                    xorpd xmm0, xmm0        // executing SSE2 instruction
                }
                break;
            case _CPU_FEATURE_MMX:
                __asm {
                    pxor mm0, mm0           // executing MMX instruction
                    emms
                }
                break;
            }
        }
        #pragma warning (suppress: 6320)
        __except (EXCEPTION_EXECUTE_HANDLER) {
            if (_exception_code() == STATUS_ILLEGAL_INSTRUCTION) {
                return false;
            }
            return false;
        }
        return true;
    }
}

namespace {
    static int getLogicalProcessorCount() {
        HANDLE handle = GetCurrentProcess();

        if(handle) {
            DWORD processAffinity;
            DWORD systemAffinity;

            if(GetProcessAffinityMask(handle, 
                &processAffinity, 
                &systemAffinity)) {
                int nBits = 0;

                while(processAffinity > 0) {
                    ++nBits;
                    processAffinity /= 2;
                }

                // The number of bits may be zero in
                // some very hypothetical technical
                // circumstances, so let's check it.

                return std::max(1, nBits);
            }
        }

        // If getting the processor count failed,
        // let's use the default.
        return 1;
    }
}

namespace niwa {
    namespace system {
        ProcessorInfo::ProcessorInfo(
                bool supportsMmx,
                bool supportsSse,
                bool supportsSse2,
                int logicalProcessorCount) 
                : supportsMmx_(supportsMmx),
                  supportsSse_(supportsSse),
                  supportsSse2_(supportsSse2),
                  logicalProcessorCount_(logicalProcessorCount) {
            // ignored
        }

        ProcessorInfo ProcessorInfo::create() {
            DWORD dwFeature = 0;

            if (!isCpuidSupported()) {
                return ProcessorInfo(false, false, false, 
                    ::getLogicalProcessorCount());
            }

            DWORD maxStandardLevel;

            _asm {
                mov eax, 0
                cpuid
                mov maxStandardLevel, eax
            }

            if(maxStandardLevel < 1) {
                return ProcessorInfo(false, false, false, 
                    ::getLogicalProcessorCount());
            }

            _asm {
                // get the Standard bits
                mov eax, 1
                cpuid
                mov dwFeature, edx
            }

            return ProcessorInfo(
                (dwFeature&_MMX_FEATURE_BIT) && _os_support(_CPU_FEATURE_MMX),
                (dwFeature&_SSE_FEATURE_BIT) && _os_support(_CPU_FEATURE_SSE),
                (dwFeature&_SSE2_FEATURE_BIT) && _os_support(_CPU_FEATURE_SSE2),
                ::getLogicalProcessorCount());
        }

        bool ProcessorInfo::supportsMmx() const {
            return supportsMmx_;
        }

        bool ProcessorInfo::supportsSse() const {
            return supportsSse_;
        }

        bool ProcessorInfo::supportsSse2() const {
            return supportsSse2_;
        }

        int ProcessorInfo::getLogicalProcessorCount() const {
            return logicalProcessorCount_;
        }
    }
}