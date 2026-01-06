/*
 * ProcessInformation.cpp
 *
 * Copyright (C) 2023-2024 Holger Grosenick
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

#include "ProcessInformation.h"
#include "StringUtil.h"
#include "Exceptions.h"
#include "utils.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>

#ifdef _WIN32
#else
#include <unistd.h>
#endif

static const char *INT_FORMAT = "%-12s: %d\n";
static const char *KB_FORMAT  = "%-12s: %lu Kb\n";
static const char *US_FORMAT  = "%-12s: %.2f secs\n";


namespace utils {

/**
 * Init all members from data file.
 */
ProcessInformation::ProcessInformation(unsigned _pid)
  : pid(_pid ? _pid : getProcessId())
  , parentPid(0)
  , threadCount(0)
  , startTime(0)
  , ticksPerSec(0)
  , userCpu(0)
  , kernelCpu(0)
  , childUserCpu(0)
  , childKernelCpu(0)
  , vmSize(0)
  , vmDataSize(0)
  , vmPeakSize(0)
  , swapSize(0)
  , residentSize(0)
{
    char filename[256];
    sprintf(filename, "/proc/%d/stat", pid);

    readStat(filename);
    strcat(filename, "us");
    readStatus(filename);
}

/**
 * Read a long int out of the current string, stop if no digit appears.
 */
uint64_t ProcessInformation::getValue(const std::string &line, size_t start)
{
    size_t i = start;
    uint64_t result = 0;

    char digit;
    while ((digit = line[i]) >= '0' && digit <= '9') {
        result *= 10;
        result += (digit - '0');
        ++i;
    }

    return result;
}

/**
 * Read /proc/<pid>/stat file - see man 5 proc
 */
void ProcessInformation::readStat(const char *filename)
{
    // check open file
    std::ifstream input(filename);
    if (!input.is_open()) {
        throw FileOpenException(true, filename);
    }

#ifdef _WIN32
#else
    ticksPerSec = sysconf(_SC_CLK_TCK);
#endif

    int c;
    unsigned colNo = 0;
    unsigned i = 0;
    char data[256];
    char *end;

    while ((c = input.get()) != EOF) {
        if (c == ' ') {
            data[i] = 0;
            i = 0;
            ++colNo;

            // skip column 3: process state
            if (colNo == 3)
                continue;

            uint64_t value = strtoul(data, &end, 10);
            if (*end != 0) {
                // invalid
                throw std::logic_error("ProcessInformation number parse error");
            }

            switch (colNo) {
            case  1:  this->pid = value;
                      // now skip all chars up to next ')'
                      while ((c = input.get()) != input.eof() && c != ')') {
                          // skip
                      }
                      // skip blank after ' '
                      input.get();
                      // this was column 2 with the executable / thread name
                      ++colNo;
                      break;
            case  4:  parentPid = value;    break;
            case 14:  userCpu = value;      break;
            case 15:  kernelCpu = value;    break;
            case 16:  childUserCpu = value; break;
            case 17:  childUserCpu = value; break;
            case 22:  startTime = value;    return;  // stop here
            }
        }
        else {
            data[i++] = c;
            if (i + 10 >= sizeof(data)) {
                throw std::logic_error("ProcessInformation data area overflow");
            }
        }
    }

    if (ticksPerSec == 0) {
        // this would cause a division by zero later on
        throw std::logic_error("ProcessInformation::ticksPerSec unset");
    }
}

/**
 * Dump current information into a file or stdout if file is null.
 */
void ProcessInformation::dumpTo(FILE *f) const
{
    if (!f)
        f = stdout;

    fprintf(f, INT_FORMAT, "pid", getPid());
    fprintf(f, INT_FORMAT, "parent pid", getParentPid());
    fprintf(f, KB_FORMAT, "vm peak", getVmPeakSize());
    fprintf(f, KB_FORMAT, "vm size", getVmSize());
    fprintf(f, KB_FORMAT, "vm data", getVmDataSize());
    fprintf(f, KB_FORMAT, "rs size", getResidentSize());
    if (getSwapSize() > 0)
        fprintf(f, KB_FORMAT, "swap size", getSwapSize());
    fprintf(f, US_FORMAT, "user time", getUserCpuSecs());
    fprintf(f, US_FORMAT, "kernel time", getKernelCpuSecs());
    fprintf(f, US_FORMAT, "CPU total", getCpuSecs());
    fprintf(f, US_FORMAT, "Elapsed time", getElapsedSecs());
    if (getThreadCount() > 0)
        fprintf(f, INT_FORMAT, "threads", getThreadCount());
}

/**
 * Returns the elapsed seconds since process start.
 */
double ProcessInformation::getElapsedSecs() const
{
    if (!startTime)
        return 0;

    // open /proc/uptime
    std::ifstream input("/proc/uptime");
    if (!input.is_open()) {
        throw FileOpenException(true, "/proc/uptime");
    }

    // read the first value: this is the number of seconds since boot.
    char line[100];
    line[0] = 0;
    input.getline(line, 90, ' ');

    // empty ?? strange, but handle this
    if (!line[0])
        return 0;

    double start = strings::parseDouble(line);
    return(start - static_cast<double>(startTime) / static_cast<double>(ticksPerSec));
}


/**
 * Read /proc/<pid>/status file - see man 5 proc
 */
void ProcessInformation::readStatus(const char *filename)
{
    // check open file
    std::ifstream input(filename);
    if (!input.is_open()) {
        throw FileOpenException(true, filename);
    }

    // now parse text
    std::string line;
    while (std::getline(input, line))
    {
        size_t pos = line.find_first_of(':');
        if (pos == std::string::npos)
            continue;

        // value name
        std::string key = line.substr(0,  pos);

        // find first non blank char after ':'
        ++pos;
        while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t'))
            ++pos;

        if (key == "VmSize") {
            vmSize = getValue(line, pos);
            continue;
        }
        if (key == "VmData") {
            vmDataSize = getValue(line, pos);
            continue;
        }
        if (key == "VmSwap") {
            swapSize = getValue(line, pos);
            continue;
        }
        if (key == "VmPeak") {
            vmPeakSize = getValue(line, pos);
            continue;
        }
        if (key == "VmRSS") {
            residentSize = getValue(line, pos);
            continue;
        }
        if (key == "Threads") {
            threadCount = getValue(line, pos);
            continue;
        }
    }
}


/**
 * Check if a DateTimeFormat is active in the stream and then format the date.
 */
std::ostream& operator<< (std::ostream& os, const ProcessInformation& p)
{
#define LEFT_13 std::setw(13) << std::left

    os << LEFT_13 << "pid" << p.getPid() << std::endl;
    os << LEFT_13 << "parent pid" << p.getParentPid() << std::endl;
    os << LEFT_13 << "vm peak" << p.getVmPeakSize() << " Kb" << std::endl;
    os << LEFT_13 << "vm size" << p.getVmSize() << " Kb"  << std::endl;
    os << LEFT_13 << "vm data" << p.getVmDataSize() << " Kb"  << std::endl;
    os << LEFT_13 << "rs size" << p.getResidentSize() << " Kb" << std::endl;
    if (p.getSwapSize() > 0)
        os << LEFT_13 << "swap size" << p.getSwapSize() << " Kb" << std::endl;
    os << LEFT_13 << "user time" << p.getUserCpuSecs() << " secs" << std::endl;
    os << LEFT_13 << "kernel time" << p.getKernelCpuSecs() << " secs" << std::endl;
    os << LEFT_13 << "CPU total" << p.getCpuSecs() << " secs" << std::endl;
    os << LEFT_13 << "Elapsed time" << p.getElapsedSecs() << " secs" << std::endl;
    if (p.getThreadCount() > 0)
        os << LEFT_13 << "threads" << p.getThreadCount() << std::endl;
    return os;
}


} /* namespace utils */
