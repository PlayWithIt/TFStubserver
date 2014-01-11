/*
 * ChildProcess.h
 *
 * Copyright (C) 2013 Holger Grosenick
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

#ifndef CHILDPROCESS_H_
#define CHILDPROCESS_H_

#include <string>
#include <vector>
#include <thread>

#include "Mutexes.h"
#include "Exceptions.h"


namespace utils {


/**
 * This class is used for starting other processes asynchronously.
 * The status of the process can be asked via the class methods.
 */
class ChildProcess {

    int  rc;
    int  pid;
    bool active;
    std::thread *th;
    std::string errorMsg;

    mutable std::mutex myMutex;

    // split one line into a list of arguments
    static void splitCmdLine(const char *cmdLine, std::vector<std::string> &out);

    // start the process
    void startChild(const std::vector<std::string> &programAndArgs, const char *workDir) throw(Exception);

    void watchChild();
    void setActive(bool a);

public:
    /**
     * Immediately starts a child process and returns. If starting the process
     * fails due to any reason an exception is thrown!
     */
    ChildProcess(const std::string &cmdline)
        throw(Exception);
    ChildProcess(const std::string &cmdline, const std::string &workDir)
        throw(Exception);
    ChildProcess(const std::vector<std::string> &programAndArgs)
        throw(Exception);
    ChildProcess(const std::vector<std::string> &programAndArgs, const std::string &workDir)
        throw(Exception);

    virtual ~ChildProcess() noexcept;

    /**
     * Is the process still running?
     */
    bool isActive() const;

    /**
     * Send a specific signal to the child.
     * @param signum if the value is negative, SIGTERM is used as default signal
     * @return true if the signal number is valid and could be send, false
     *         if the child has already died
     */
    bool kill(int signum = -1);

    /**
     * Returns the return code if the process is not running any more or
     * -1 if the process is still active. Other negative values indicate that
     * the process has died abnormally, e.g. -11 is signal 11 (SIGSEGV).
     */
    int getRc() const {
        return rc;
    }

    /**
     * Returns the process id of the child process if the process still running
     * or -1 if the process has already finished.
     */
    int getPid() const {
        return pid;
    }

    /**
     * Error message after the process has finished, this is normally only set
     * if the exit code is negative (internal error or process exited abnormally).
     */
    const std::string& getErrorMsg() const {
        return errorMsg;
    }
};

} /* namespace utils */
#endif /* CHILDPROCESS_H_ */
