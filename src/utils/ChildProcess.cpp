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
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

#include <sstream>
#include <vector>

#include "ChildProcess.h"

#define MAX_ARGS 200


namespace utils {

using utils::MutexLock;


/**
 * Immediately starts a child process and returns. If starting the process
 * fails due to any reason an exception is thrown!
 */
ChildProcess::ChildProcess(const std::string &cmdline) throw(Exception)
    : rc(-1)
    , pid(-1)
    , active(false)
    , th(NULL)
    , myMutex()
{
    std::vector<std::string> args;

    splitCmdLine(cmdline.c_str(), args);
    startChild(args, NULL);
}

ChildProcess::ChildProcess(const std::string &cmdline, const std::string &workDir) throw(Exception)
    : rc(-1)
    , pid(-1)
    , active(false)
    , th(NULL)
    , myMutex()
{
    std::vector<std::string> args;

    splitCmdLine(cmdline.c_str(), args);
    startChild(args, workDir.c_str());
}

ChildProcess::ChildProcess(const std::vector<std::string> &programAndArgs) throw(Exception)
    : rc(-1)
    , pid(-1)
    , active(false)
    , th(NULL)
    , myMutex()
{
    startChild(programAndArgs, NULL);
}

ChildProcess::ChildProcess(const std::vector<std::string> &programAndArgs, const std::string &workDir) throw(Exception)
    : rc(-1)
    , pid(-1)
    , active(false)
    , th(NULL)
    , myMutex()
{
    startChild(programAndArgs, workDir.c_str());
}


/**
 * Check if the thread is still active: if not way delete it, otherwise
 * we might have a memory leak ...
 */
ChildProcess::~ChildProcess() noexcept
{
    if (!active && th) {
        th->join();
        delete th;
    }
}

/**
 * Send a specific signal to the child.
 */
bool ChildProcess::kill(int signum)
{
    if (!isActive() || signum > 32)
        return false;

    int signal = signum <= 0 ? SIGTERM : signum;
    int rc = ::kill(pid, signal);

    return rc == 0;
}

/**
 * Return the value of the 'active' flag
 */
bool ChildProcess::isActive() const
{
    MutexLock lock(myMutex);
    return active;
}

/**
 * Changes the 'active' flag to the given value.
 */
void ChildProcess::setActive(bool a)
{
    MutexLock lock(myMutex);
    active = a;
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
          if (*c == '\'')
          {
              ++c;
              while (*c != 0 && *c != '\'')   // find end of string
              {
                  arg[i++] = *c;
                  if (i > sizeof(arg) - 5)    // argument to large
                      throw Exception("Command-Line too long!");
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
 * Helper to split a string using a given delimiter
 */
static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


// start the process
void ChildProcess::startChild(const std::vector<std::string> &args, const char *workDir) throw(Exception)
{
    const char *exe = args[0].c_str();
    std::string temp;

    if (*exe != '/' && access(exe, R_OK) != 0)
    {
        // path is not absolute => determine EXE via 'PATH' environment
        std::vector<std::string> paths;
        std::string path(getenv("PATH"));
        split(path, ':', paths);

        bool found = false;
        for (std::vector<std::string>::const_iterator it = paths.cbegin(); !found && it != paths.cend(); ++it)
        {
            temp = (*it) + '/' + exe;
            if (access(temp.c_str(), R_OK) == 0)
            {
                exe = temp.c_str();
                found = true;
            }
        }
        if (!found)
            throw Exception(args[0] + ": not found");
    }

    // check path's
    if (access(exe, X_OK) != 0) {
        throw Exception(std::string(exe) + ": has no executable flag!");
    }
    if (workDir && *workDir && access(workDir, R_OK | X_OK) != 0) {
        throw Exception(std::string(workDir) + " not a valid directory!");
    }

    int childPid = fork();
    if (childPid == 0)
    {
        // this is the child process: change workDir if it is set
        if ( workDir && *workDir && 0 != chdir(workDir) )
        {
            fprintf(stderr, "chdir(%s) failed: %s\n", workDir, strerror(errno));
            _exit(126);
        }

        // copy array + terminating NULL entry ...
        char *cmd[MAX_ARGS];
        size_t i;
        for (i = 1; i < args.size(); ++i)
            cmd[i] = (char*) args[i].c_str();
        cmd[0] = (char*) exe;
        cmd[i] = NULL;

        execv(cmd[0], cmd);
        fprintf(stderr, "execv(%s) failed: %s\n", cmd[0], strerror(errno));
        _exit(127);
        return;
    }

    if (childPid > 0)
    {
        // this is the parent process
        pid = childPid;
        setActive(true);

        // creating a thread can throw an exception ...
        th = new std::thread(&ChildProcess::watchChild, this);
    }
    else {
        // PID < 0: start of process failed ...
        throw Exception(std::string("Start of '" + args[0] + "' failed: " + strerror(errno)));
    }
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
        errorMsg = std::string("waitpid() failed: ") + std::string(strerror(errno));
    }

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
    setActive(false);
}

} /* namespace utils */
