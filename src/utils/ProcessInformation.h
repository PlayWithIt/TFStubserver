/*
 * ProcessInformation.h
 *
 * Copyright (C) 2023 Holger Grosenick
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTILS_PROCESSINFORMATION_H_
#define UTILS_PROCESSINFORMATION_H_

#include <string>
#include <stdint.h>

namespace utils {

/**
 * Holds some information about a process with a given process id.
 * If the pid 0 is passed to the constructor: get information from the
 * current process.
 *
 * The object is read-only after creation.
 */
class ProcessInformation final
{
    unsigned pid;              // current process
    unsigned parentPid;        // parent process
    unsigned threadCount;      // number of active threads
    uint64_t startTime;        // process' start time (OS specific)
    uint64_t ticksPerSec;      // clock ticks per second
    uint64_t userCpu;          // in clock ticks
    uint64_t kernelCpu;        // in clock ticks
    uint64_t childUserCpu;     // in clock ticks
    uint64_t childKernelCpu;   // in clock ticks
    uint64_t vmSize;           // overall virtual memory size in KBytes (1024 bytes)
    uint64_t vmDataSize;       // in KB
    uint64_t vmPeakSize;       // in KB
    uint64_t swapSize;         // swapped memory in KB
    uint64_t residentSize;     // resident memory in KB (actually in RAM)

    uint64_t getValue(const std::string &, size_t start);
    void readStat(const char *filename);
    void readStatus(const char *filename);

public:
    /**
     * Get process information for the given process (must be current user context).
     * If pid == 0, then the current process is queried.
     */
    explicit ProcessInformation(unsigned pid = 0);

    //---------------------------------------
    // simple getters
    //---------------------------------------
    unsigned getPid() const {
        return pid;
    }

    unsigned getParentPid() const {
        return parentPid;
    }

    uint64_t getTicksPerSec() const {
        return ticksPerSec;
    }

    uint64_t getUserCpu() const {
        return userCpu;
    }

    uint64_t getKernelCpu() const {
        return kernelCpu;
    }

    uint64_t getChildUserCpu() const {
        return childUserCpu;
    }

    uint64_t getChildKernelCpu() const {
        return childKernelCpu;
    }

    /**
     * Get the overall CPU time in seconds (with digits) for this process.
     */
    double getCpuSecs() const {
        return static_cast<double>(kernelCpu + userCpu) / static_cast<double>(ticksPerSec);
    }

    /**
     * Get the user CPU time in seconds (with digits) for this process.
     */
    double getUserCpuSecs() const {
        return static_cast<double>(userCpu) / static_cast<double>(ticksPerSec);
    }

    /**
     * Get the kernel CPU time in seconds (with digits) for this process.
     */
    double getKernelCpuSecs() const {
        return static_cast<double>(kernelCpu) / static_cast<double>(ticksPerSec);
    }

    uint64_t getVmSize() const {
        return vmSize;
    }

    uint64_t getVmDataSize() const {
        return vmDataSize;
    }

    uint64_t getVmPeakSize() const {
        return vmPeakSize;
    }

    uint64_t getResidentSize() const {
        return residentSize;
    }

    /**
     * Returns the process' start time: on Linux this is the number of clocks
     * ticks since kernel boot.
     */
    uint64_t getStartTime() const {
        return startTime;
    }

    /**
     * Returns the elapsed seconds since process start.
     */
    double getElapsedSecs() const;

    uint64_t getSwapSize() const {
        return swapSize;
    }

    unsigned getThreadCount() const {
        return threadCount;
    }

    /**
     * Dump current information into a file or stdout if file is null.
     */
    void dumpTo(FILE *f = nullptr) const;
};

/**
 * Print process information in multiple lines into the output stream.
 */
std::ostream& operator<< (std::ostream& os, const ProcessInformation& p);


} /* namespace utils */

#endif /* UTILS_PROCESSINFORMATION_H_ */
