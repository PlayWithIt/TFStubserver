/*
 * ChildProcess.cpp
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

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

#include <stdexcept>
#include <sstream>
#include <vector>

#include "ChildProcess.h"
#include "Exceptions.h"
#include "File.h"
#include "Log.h"
#include "HandleStreambuf.h"
#include "StringUtil.h"

#define MAX_ARGS 200


namespace utils {

/**
 * Helper class with redirect options: it handles creation of pipes and/or files.
 */
struct Redirect
{
    enum Type {
        FROM_PARENT,
        PIPE,
        TO_FILE
    };

    Redirect() : file(), append(false), type(FROM_PARENT), pipeHandles{-1, -1}, in(NULL), out(NULL) { };

    /**
     * Close all
     */
    ~Redirect() {
        closePipe();
    }

    /**
     * Close the pipes and steams if they are open.
     */
    void closePipe();

    /**
     * Open output for stdout or stderr (pipe or file).
     */
    void openInOut(int targetFileno);

    /**
     * Prepare the pipes to the child process (not reversible).
     */
    void openPipe()
    {
        if (pipeHandles[0] >= 0 || pipeHandles[1] >= 0)
            throw std::logic_error("Child stream already open, cannot change any more!");
        if (pipe(pipeHandles) < 0)
            throw utils::RuntimeError("pipe() failed");
        type = Redirect::PIPE;
    }

    /**
     * Set the file where to read from or write to, do not yet open the file.
     */
    void setFile(const File &f, bool _append);

    /**
     * Close a pipe opened as stdin to the child.
     */
    void closeIn()
    {
        if (type != Redirect::PIPE)
            throw std::logic_error("Cannot close child's stdin: not opened as PIPE");
        delete out;
        out = NULL;
    }

private:
    utils::File file;
    bool        append;
public:
    Type        type;
    int         pipeHandles[2];
    InputStream  *in;
    OutputStream *out;
};


/**
 * Close the pipes and steams if they are open.
 */
void Redirect::closePipe()
{
    if (type != Redirect::PIPE)
        return;

    if (pipeHandles[0] >= 0)
        close(pipeHandles[0]);
    if (pipeHandles[1] >= 0)
        close(pipeHandles[1]);

    delete in;
    in = NULL;
    delete out;
    out = NULL;

    type = Redirect::FROM_PARENT;
}

/**
 * Open output for stdout or stderr (pipe or file).
 */
void Redirect::openInOut(int targetFileno)
{
    int idxDup = (targetFileno == STDIN_FILENO ? 0 : 1);

    if ( type == Redirect::PIPE )
    {
        int idxClose = (targetFileno == STDIN_FILENO ? 1 : 0);

        // close read or write side of the pipe and alter the other side
        close(pipeHandles[idxClose]);
        pipeHandles[idxClose] = -1;
        dup2(pipeHandles[idxDup], targetFileno);
    }
    else if ( type == Redirect::TO_FILE )
    {
        bool read = targetFileno == STDIN_FILENO;
        pipeHandles[idxDup] = open(file.getFullname().c_str(),
                                   read ? O_RDONLY : O_WRONLY | O_CREAT | (append ? O_APPEND : 0),
                                   S_IRUSR | S_IWUSR);
        if (pipeHandles[idxDup] < 0)
        {
            // this is called within the child process: terminate after error
            fprintf(::stderr, "open(%s) failed: %s\n", file.getFullname().c_str(), strerror(errno));
            _exit(125);
        }
        dup2(pipeHandles[idxDup], targetFileno);
    }
}

/**
 * Set the file where to read from or write to, do not yet open the file.
 */
void Redirect::setFile(const File &f, bool _append)
{
    if (pipeHandles[0] >= 0 || pipeHandles[1] >= 0)
        throw std::logic_error("Child stream already open, cannot change any more!");
    type = Redirect::TO_FILE;
    file = f;
    append = _append;
}


// -----------------------------------------------------------------------------------


/**
 * Prepare to start a child process and return, the cmd-array must have at least the name
 * of the executable.
 */
ChildProcess::ChildProcess(const std::string &_cmdline)
  : workDir(".")
  , redirect(new Redirect[3])
  , th(NULL)
  , active(false)
  , rc(-1)
  , pid(-1)
{
    splitCmdLine(_cmdline.c_str(), programAndArgs);
    checkArgs();
}

ChildProcess::ChildProcess(const char *_cmdline)
  : workDir(".")
  , redirect(new Redirect[3])
  , th(NULL)
  , active(false)
  , rc(-1)
  , pid(-1)
{
    splitCmdLine(_cmdline, programAndArgs);
    checkArgs();
}

ChildProcess::ChildProcess(const std::vector<std::string> &_programAndArgs)
  : programAndArgs(_programAndArgs)
  , workDir(".")
  , redirect(new Redirect[3])
  , th(NULL)
  , active(false)
  , rc(-1)
  , pid(-1)
{
    checkArgs();
}


/**
 * Kill the child process if it is still active and cleanup the thread.
 */
ChildProcess::~ChildProcess()
{
    if (th) {
        kill();
        th->join();
        delete th;
    }
    delete[] redirect;
}

void ChildProcess::checkArgs()
{
    if (programAndArgs.size() == 0)
        throw std::invalid_argument("command array must have a length > 0 and at least the name of the executable");
}

void ChildProcess::closeStdin()
{
    redirect[0].closeIn();
}

/**
 * Send a specific signal to the child.
 */
bool ChildProcess::kill(int signum)
{
    if (!isActive() || signum > 32)
        return false;

    if (pid <= 0) {
        Log::error("ChildProcess::kill called when pid is < 0");
        return false;
    }

    int signal = signum <= 0 ? SIGTERM : signum;

    utils::Log() << "ChildProcess: send signal " << signal <<  " to pid " << pid;
    int rc = ::kill(pid, signal);

    return rc == 0;
}

/**
 * Set file where to read from in the child process.
 */
void ChildProcess::redirectStdin(const File &in)
{
    validateRedirect();
    redirect[0].setFile(in, false);
}

/**
 * Set file where to write to in the child process.
 */
void ChildProcess::redirectStdout(const File &out, bool append)
{
    validateRedirect();
    redirect[1].setFile(out, append);
}

/**
 * Set file where to write to in the child process.
 */
void ChildProcess::redirectStderr(const File &out, bool append)
{
    validateRedirect();
    redirect[2].setFile(out, append);
}

/**
 * Change the working directory of the child process, before the process is started.
 */
void ChildProcess::setWorkDir(const File &_workDir)
{
    this->workDir = _workDir;
}

void ChildProcess::setWorkDir(const char *_workDir)
{
    this->workDir = File(_workDir);
}

// split one line into a list of arguments
void ChildProcess::splitCmdLine(const char *cmdLine, std::vector<std::string> &out)
{
    if (!cmdLine || *cmdLine == 0 )       // invalid argument
        throw Exception("Invalid argument: 'cmdLine' is NULL or empty");

    out.clear();                          // remove all previous entries
    const char *c = cmdLine;

    bool done = false;
    do {
      char arg[1024];
      size_t i = 0;

      while (*c == ' ')                       // skip leading blanks
          ++c;

      while (*c != 0 && *c != ' ')            // find end of word
      {
          if (*c == '\'' || *c == '"')
          {
              char start = *c;
              ++c;
              while (*c != 0 && *c != start)   // find end of string
              {
                  arg[i++] = *c;
                  if (i > sizeof(arg) - 5)    // argument to large
                      throw Exception("String terminator not found: command-Line too long!");
                  ++c;
              }
          }
          else {
              if (*c == '\\' && *(c+1) != 0)
                  ++c;
              arg[i++] = *c;
              if (i > sizeof(arg) - 5)       // argument to large
                  throw Exception("Command-Line too long!");
              ++c;
          }
      }

      arg[i] = 0;                           // terminate word
      if (*c == 0)
          done = true;                      // end reached
      else
          ++c;                              // set to next valid char

      out.push_back(std::string(arg));
    } while (!done);
}

/**
 * Now really start the child process.
 */
void ChildProcess::start()
{
    // check if active
    if (active)
        throw Exception("ChildProcess still active, cannot start!");

    active = true;
    rc     = -1;
    pid    = -1;

    // cleanup if the process is about to be restarted
    if (th)
    {
        th->join();
        delete th;
        th = NULL;
        redirect[0].closePipe();
        redirect[1].closePipe();
        redirect[2].closePipe();
    }

    try {
        tryToStart();
    }
    catch (...) {
        active = false;
        throw;
    }
}


/**
 * Start child process without taking care of the 'active' flag.
 */
void ChildProcess::tryToStart()
{
    bool changeWorkDir = workDir.getFullname().compare(".") != 0;
    std::string exe = programAndArgs[0];

    // path is absolute or relative to workdir
    if (changeWorkDir)
    {
        if (!workDir.exists())
            throw Exception(workDir.getFullname() + ": does not exist!");
        if (!workDir.isDirectory())
            throw Exception(workDir.getFullname() + ": is not a valid directory!");
    }

    if (exe.find(File::PATH_SEP_CHAR) == std::string::npos)
    {
        // path is not absolute or relative to workdir => determine EXE via 'PATH' environment
        try {
            File out = File::which("PATH", exe.c_str());
            exe = out.getAbsolutePath();
        }
        catch (const Exception &e) {
            throw Exception(programAndArgs[0] + ": not found");
        }
    }
    else if (exe[0] != File::PATH_SEP_CHAR) {
        // if not absolute: find relative path
        File target(workDir, exe);
        if (!target.exists())
            throw Exception(target.getFullname() + ": not found");
        exe = target.getAbsolutePath();
    }

    // check path's
    File exeFile(exe);

    if (!exeFile.exists())
        throw Exception(exe + ": no such executable!");
    if (!exeFile.isRegularFile())
        throw Exception(exe + ": is not a regular file!");
    if (!exeFile.canExecute())
        throw Exception(exe + ": is not an executable!");

    int childPid = fork();
    if (childPid == 0)
    {
        redirect[0].openInOut(STDIN_FILENO);
        redirect[1].openInOut(STDOUT_FILENO);
        redirect[2].openInOut(STDERR_FILENO);

        // this is the child process: change workDir if it is set
        if ( changeWorkDir && 0 != chdir(workDir.getFullname().c_str()) )
        {
            fprintf(::stderr, "chdir(%s) failed: %s\n", workDir.getFullname().c_str(), strerror(errno));
            _exit(126);
        }

        // copy array + terminating NULL entry ...
        char *cmd[MAX_ARGS];
        size_t i;
        for (i = 1; i < programAndArgs.size(); ++i)
            cmd[i] = (char*) programAndArgs[i].c_str();
        cmd[0] = (char*) exe.c_str();
        cmd[i] = NULL;

        execv(cmd[0], cmd);
        fprintf(::stderr, "execv(%s) failed: %s\n", cmd[0], strerror(errno));
        _exit(127);
        return;
    }

    if (childPid > 0)
    {
        // this is the parent process
        pid = childPid;

        if (redirect[0].type == Redirect::PIPE)
        {
            // close write side of the pipe
            close(redirect[0].pipeHandles[0]);
            redirect[0].pipeHandles[0] = -1;
        }
        if (redirect[1].type == Redirect::PIPE)
        {
            // close read side of the pipe
            close(redirect[1].pipeHandles[1]);
            redirect[1].pipeHandles[1] = -1;
        }
        if (redirect[2].type == Redirect::PIPE)
        {
            // close read side of the pipe
            close(redirect[2].pipeHandles[1]);
            redirect[2].pipeHandles[1] = -1;
        }

        // creating a thread can throw an exception ...
        th = new std::thread(&ChildProcess::watchChild, this);
    }
    else {
        // PID < 0: start of process failed ...
        active = false;

        int err = errno;
        char buffer[2048];
        sprintf(buffer, "Start of '%s' failed", programAndArgs[0].c_str());
        throw utils::RuntimeError(buffer, err);
    }
}

/**
 * Redirect childs stdin to a stream
 */
std::ostream& ChildProcess::stdin()
{
    validateRedirect();
    redirect[0].openPipe();
    redirect[0].out = new OutputStream(redirect[0].pipeHandles[1]);
    return *redirect[0].out;
}

/**
 * Redirect childs stdout to a stream
 */
std::istream& ChildProcess::stdout()
{
    validateRedirect();
    redirect[1].openPipe();
    redirect[1].in = new InputStream(redirect[1].pipeHandles[0]);
    return *redirect[1].in;
}

/**
 * Redirect childs stderr to a stream
 */
std::istream& ChildProcess::stderr()
{
    validateRedirect();
    redirect[2].openPipe();
    redirect[2].in = new InputStream(redirect[2].pipeHandles[0]);
    return *redirect[2].in;
}

/**
 * Check if redirection of pipes is still possible
 */
void ChildProcess::validateRedirect()
{
    if (active)
        throw Exception("Child process already active, cannot change redirect any more!");

}

/**
 * Wait for the child process 'ms' milliseconds to terminate, if 'ms' is 0,
 * wait endlessly (be careful). If the method returns because the child has terminated,
 * TRUE is returned, otherwise FALSE (this means, the method returns '!active').
 */
bool ChildProcess::waitFor(unsigned ms)
{
    if (!active)
        return true;

    std::unique_lock<std::mutex> l(childMutex);
    if (ms > 0)
    {
        std::chrono::milliseconds rel_time(ms);
        childEvent.wait_for(l, rel_time);
    }
    else {
        childEvent.wait(l);
    }
    return !active;
}

/**
 * Wait for child process to finish ...
 */
void ChildProcess::watchChild()
{
    // printf("watchChild this = %p ...\n", this);

    int status = 0;
    int waitRc = waitpid(pid, &status, 0);
    if (waitRc == -1)
    {
        rc = -200;
        errorMsg = std::string("waitpid() failed: ") + utils::strings::strerror(errno);
    }
    active = false;

    if ( WIFEXITED(status) )
        rc = WEXITSTATUS(status);
    else if ( WIFSIGNALED(status) ) {
        char msg[128];
        sprintf(msg, "Process terminated with signal %d", rc);
        rc = WTERMSIG(status);
        errorMsg = msg;
        rc *= -1;              // signal number as negative value
    }
    else {
        rc = -201;
        errorMsg = "Undefined exit() status of child-process";
    }
    childEvent.notify_all();
}

} /* namespace utils */
