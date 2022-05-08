/*
 * SignalHandlers.h
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

#ifndef UTILS_SIGNALHANDLERS_H_
#define UTILS_SIGNALHANDLERS_H_

namespace utils {

/**
 * This class offers static methods to install signal handlers where the
 * event of the signal is checked only from time to time and no immediate
 * callback is necessary.
 */
class SignalHandlers {

    SignalHandlers();
    SignalHandlers(const SignalHandlers &other) = delete;
    SignalHandlers(SignalHandlers &&other) = delete;
    SignalHandlers& operator=(const SignalHandlers &other) = delete;
    SignalHandlers& operator=(SignalHandlers &&other) = delete;

public:

    /**
     * Signal types used below in the methods, internally mapped to OS signals.
     * Other signal are not allowed in the below methods.
     */
    enum Signal : int {
        SIG_HANGUP = 1,   // hangup == reload data / config
        SIG_INT,          // interrupt == ctrl-c
        SIG_USER1,        // user defined signal 1
        SIG_USER2,        // user defined signal 2
        SIG_TERM          // terminate application, similar to SIG_INT
    };

    /**
     * Activate a signal handler for SIG_INT + SIG_TERM which sets the finish
     * flag so that 'shouldFinish()' returns true.
     */
    static void activateSigTermFlag();

    /**
     * Activate some defaults for signal handlers:
     * sigTerm = true => catch SIGTERM + SIGINT to set a flag for 'shouldFinish'
     * sigFault = true => catch SIGKILL and some other which log the signal and exit
     *                    do not mix this with the StackTracer !
     * ignoreSigPipe = true => ignore broken pipe signal (unchanged if set to false)
     */
    static void activateDefaults(bool sigTerm, bool sigFault, bool ignoreSigPipe = false);

    /**
     * Activate a signal handler to set a flag which indicates that the given
     * signal was received, this can be queried using 'gotSignal()'.
     */
    static void activateSignal(Signal sigNum);

    /**
     * Reset the signal flag which should be used after a signal was processed,
     * e.g. a SIGHUP was processed and the application can continue.
     * After a call of this method 'gotSignal()' returns false for this signal number.
     */
    static void clearSignal(Signal sigNum);

    /**
     * Was the given signal received after the last call to 'clearSignal()' ?
     */
    static bool gotSignal(Signal sigNum);

    /**
     * Set signal handler for example for SIGUSR1 or similar.
     * The handler function takes three arguments, the second is 'siginfo_t' ...
     * Do not mix a call to this method with "activateSignal" for the same signal.
     */
    static void setHandler(Signal signalNo, void (*function)(int signal, void *sigInfo, void *context));

    /**
     * Convert the operating system signal number into readable format ...
     */
    static const char *signal2char(int signum);

    /**
     * Converts the signal enum into a signal number from the operating system,
     * Returns -1 if the signal is not supported by the current OS.
     */
    static int signal2osSignal(Signal s);

    /**
     * Returns true if Ctrl-C was pressed / SIGINT was triggered: soft exit.
     * This method is equal to "hasSignal(SIG_TERM) || hasSignal(SIG_INT)".
     */
    static bool shouldFinish();

};

} /* namespace utils */

#endif /* UTILS_SIGNALHANDLERS_H_ */
