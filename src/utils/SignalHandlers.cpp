/*
 * SignalHandlers.cpp
 *
 * Copyright (C) 2021 Holger Grosenick
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

#include "SignalHandlers.h"

#include <signal.h>
#include <string.h>

#include "Log.h"


namespace utils {

// global flag to indicate that the application should finish
#define MAX_SINGALS 64
static bool calledSignal[MAX_SINGALS];
static int  signalFromPid;

/**
 * Signal handler to terminate the application if 'shouldFinish' is checked regularly.
 */
#ifndef _WIN32
static void sigTermHandler(int sig, siginfo_t *info, void *)
{
    static unsigned count = 0;

    if (++count == 5) {
        Log() << "sigTermHandler(" << sig << ") triggered 5 times => hard exit ...";
        exit(16);
    }

    signalFromPid = info ? info->si_pid : 0;
    if (count == 1)
        Log() << "sigTermHandler(" << sig << ") triggered from pid "
              << signalFromPid << " => should finish ...";
    else
        Log() << "sigTermHandler(" << sig << ") triggered (" << count << " times) from pid "
              << signalFromPid << " => should finish ...";

    calledSignal[SIGTERM] = true;
}


/**
 * Signal handler for all other signals that should just set a flag.
 */
static void anySignalHandler(int sig, siginfo_t *info, void *)
{
    Log() << "anySignalHandler(" << sig << ") triggered from pid " << signalFromPid;

    signalFromPid = info ? info->si_pid : 0;
    if (sig > 0 && sig < MAX_SINGALS)
        calledSignal[sig] = true;
}

/**
 * Called for critical signals.
 */
static void sigKillHandler(int sig, siginfo_t *, void *)
{
    Log() << "CAUGHT CRITICAL SIGNAL " << sig << " (" << SignalHandlers::signal2char(sig) << ") => abort!";
    exit(16);
}
#endif

/**
 * Activate some defaults for signal handlers:
 * sigTerm = true => catch SIGTERM + SIGINT to set a flag for 'shouldFinish'
 * sigFault = true => catch SIGKILL and some other which log the signal and exit
 *                    do not mix this with the StackTracer !
 * ignoreSigPipe = true => ignore broken pipe signal (unchanged if set to false)
 */
void SignalHandlers::activateDefaults(bool sigTerm, bool sigFault, bool ignoreSigPipe)
{
#ifndef _WIN32
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_flags = SA_SIGINFO;

    // graceful shutdown
    if (sigTerm)
        activateSigTermFlag();

    // dump trace and terminate
    if (sigFault) {
        act.sa_sigaction = sigKillHandler;
        sigaction(SIGSEGV, &act, NULL);
        sigaction(SIGILL,  &act, NULL);
        sigaction(SIGBUS,  &act, NULL);
    }

    // ignore: write() call will fail with EPIPE
    if (ignoreSigPipe)
        signal(SIGPIPE, SIG_IGN);
#endif
}


/**
 * Activate a signal handler for SIGINT + SIGTERM which sets the finish
 * flag so that 'shouldFinish()' returns true.
 */
void SignalHandlers::activateSigTermFlag()
{
#ifndef _WIN32
    struct sigaction act;
    memset(&act, 0, sizeof(act));

    // graceful shutdown
    act.sa_sigaction = sigTermHandler;
    act.sa_flags     = SA_SIGINFO;
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT,  &act, NULL);
#endif
}


/**
 * Activate a signal handler to set a flag which indicates that the given
 * signal was received, this can be queried using 'gotSignal()'.
 */
void SignalHandlers::activateSignal(Signal sigNum)
{
    int osSignal = signal2osSignal(sigNum);
    if (osSignal < 0) {
        return;
    }

#ifndef _WIN32
    struct sigaction act;
    memset(&act, 0, sizeof(act));

    // graceful shutdown
    act.sa_sigaction = anySignalHandler;
    act.sa_flags     = SA_SIGINFO;
    sigaction(osSignal, &act, NULL);
#endif
}

/**
 * Reset the signal flag which should be used after a signal was processed,
 * e.g. a SIGHUP was processed and the application can continue.
 * After a call of this method 'gotSignal()' returns false for this signal number.
 */
void SignalHandlers::clearSignal(Signal sigNum)
{
    int osSignal = signal2osSignal(sigNum);
    if (osSignal >= 0) {
        calledSignal[osSignal] = false;
    }
}

/**
 * Was the given signal received after the last call to 'clearSignal()' ?
 */
bool SignalHandlers::gotSignal(Signal sigNum)
{
    int osSignal = signal2osSignal(sigNum);
    if (osSignal < 0) {
        return false;
    }
    return calledSignal[osSignal];
}

/**
 * Returns the process id (PID) which sent the latest signal to this process.
 */
int SignalHandlers::pidOfLatestSignal()
{
    return signalFromPid;
}


// set signal handler for example for SIGUSR1 or similar
void SignalHandlers::setHandler(Signal sigNum, void (*function)(int, void *sigInfo, void *u_context))
{
    int osSignal = signal2osSignal(sigNum);
    if (osSignal < 0) {
        return;
    }
#ifndef _WIN32
    struct sigaction act;
    memset(&act, 0, sizeof(act));

    act.sa_sigaction = (void (*)(int, siginfo_t *, void *))function;
    act.sa_flags     = SA_SIGINFO;
    sigaction(osSignal, &act, NULL);
#endif
}

/**
 * Convert the signal number into readable format ...
 */
const char *SignalHandlers::signal2char(int signum)
{
    static char sigStr[64];

    if (signum == SIGILL)
        return "SIGILL";
    if (signum == SIGFPE)
        return "SIGFPE";
    if (signum == SIGSEGV)
        return "SIGSEGV";
    if (signum == SIGINT)
        return "SIGINT";
    if (signum == SIGTERM)
        return "SIGTERM";
#ifndef _WIN32
    if (signum == SIGBUS)
        return "SIGBUS";
    if (signum == SIGPIPE)
        return "SIGPIPE";
    if (signum == SIGKILL)
        return "SIGKILL";
    if (signum == SIGCHLD)
        return "SIGCHLD";
    if (signum == SIGUSR1)
        return "SIGUSR1";
    if (signum == SIGUSR2)
        return "SIGUSR2";
#endif

    snprintf(sigStr, sizeof(sigStr), "SIG(%d)", signum);
    sigStr[sizeof(sigStr) - 1] = 0;
    return sigStr;
}

/**
 * Converts the signal enum into a signal number from the operating system,
 * Returns -1 if the signal is not supported by the current OS.
 */
int SignalHandlers::signal2osSignal(Signal s)
{
    switch (s) {
    case SIG_INT:
        return SIGINT;
    case SIG_TERM:
        return SIGTERM;
#ifndef _WIN32
    case SIG_HANGUP:
        return SIGHUP;
    case SIG_USER1:
        return SIGUSR1;
    case SIG_USER2:
        return SIGUSR2;
#endif
    }
    Log::log("SignalHandlers::signal2osSignal: not supported signal", s);
    return -1;
}

/**
 * Should the application finish?
 */
bool SignalHandlers::shouldFinish()
{
    return calledSignal[SIGTERM];
}


} /* namespace utils */
