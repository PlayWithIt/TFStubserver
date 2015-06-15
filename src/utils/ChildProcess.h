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

#include <vector>
#include <thread>
#include <atomic>
#include <iostream>
#include <condition_variable>

#include "File.h"


namespace utils {

struct Redirect;

/**
 * This class is used for starting other processes asynchronously.
 * The status of the process can be asked via the class methods.
 */
class ChildProcess {

public:
    /**
     * Prepare to start a child process and return, the cmd-array must have at least the name
     * of the executable.
     */
    explicit ChildProcess(const std::string &cmdline);
    explicit ChildProcess(const char *cmdline);
    ChildProcess(const std::vector<std::string> &programAndArgs);

    /**
     * This kills the child if it is still active.
     */
    virtual ~ChildProcess();

    /**
     * Change the working directory of the child process, before the process is started.
     */
    void setWorkDir(const File &workDir);
    void setWorkDir(const char *workDir);
    void setWorkDir(const std::string &workDir) {
        setWorkDir(workDir.c_str());
    }

    /**
     * Start the child process asynchronously and return. If starting the process fails, an
     * exception is thrown.
     * <P>
     * If a child has finished, the start() method maybe called again. Be aware that existing
     * redirects get invalidated when the child terminates.
     */
    void start();

    /**
     * Get the stdout from the child process: this must be called before {@link #start()}
     * in order to create a pipe before starting the child.
     */
    std::istream& stdout();

    /**
     * Get the stderr from the child process: this must be called before {@link #start()}
     * in order to create a pipe before starting the child.
     */
    std::istream& stderr();

    /**
     * Get the stdin from the child process: this must be called before {@link #start()}
     * in order to create a pipe before starting the child. The parent can write into the
     * stream, but the child process must consume stdin, otherwise writing into the pipe
     * will block.
     */
    std::ostream& stdin();

    /**
     * Set file where to read from in the child process. If the file is relative, it
     * is relative to the working dir of the child process!
     */
    void redirectStdin(const File &in);
    ChildProcess& operator<(const File &in) {
        redirectStdin(in);
        return *this;
    }

    /**
     * Set file where to write to in the child process. If the file is relative, it
     * is relative to the working dir of the child process!
     */
    void redirectStdout(const File &out, bool append = false);

    /**
     * Redirect stdout of the child process into a file (overwrite the file).
     */
    ChildProcess& operator>(const File &out) {
        redirectStdout(out, false);
        return *this;
    }

    /**
     * Redirect stdout of the child process into a file (append the file).
     */
    ChildProcess& operator>>(const File &out) {
        redirectStdout(out, true);
        return *this;
    }

    /**
     * Set file where to write to in the child process. If the file is relative, it
     * is relative to the working dir of the child process!
     */
    void redirectStderr(const File &out, bool append = false);

    /**
     * Close stdin if it was piped to the child process. This might be necessary
     * to terminate the child which reads stdin. If stdin was not piped, this
     * method throws an exception.
     */
    void closeStdin();

    /**
     * Is the process still running?
     */
    bool isActive() const {
        return active;
    }

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

    /**
     * Wait for the child process 'ms' milliseconds to terminate, if 'ms' is 0,
     * wait endlessly (be careful). If the method returns because the child has terminated,
     * TRUE is returned, otherwise FALSE (this means, the method returns '!active').
     */
    bool waitFor(unsigned ms = 0);

private:
    std::vector<std::string> programAndArgs;
    File             workDir;
    std::string      errorMsg;
    Redirect        *redirect;   // implementation detail
    std::thread     *th;
    std::atomic_bool active;

    // for synchronization
    std::mutex              childMutex;
    std::condition_variable childEvent;

    int  rc;
    int  pid;

    // split one line into a list of arguments
    static void splitCmdLine(const char *cmdLine, std::vector<std::string> &out);
    void checkArgs();
    void watchChild();
    void validateRedirect();
    void tryToStart();
};

} /* namespace utils */
#endif /* CHILDPROCESS_H_ */
