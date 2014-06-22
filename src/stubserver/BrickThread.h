/*
 * BrickThread.h
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

#ifndef BRICKTHREAD_H_
#define BRICKTHREAD_H_

#include <utils/AsyncTask.h>
#include "BrickStack.h"

namespace stubserver {

/**
 * The main thread of the server which consumes the client
 * requests queue and checks for callback periodically.
 * <p>
 * It uses the functions of {@link BrickStack} in order to do this.
 */
class BrickThread : public utils::AsyncTask, public BrickStack
{
public:
    /**
     * Init the stack from properties: the filename defines the path to
     * a properties file where the BrickStack object is created from.
     */
    BrickThread(const char *filename = NULL);

    virtual void run();
};

} /* namespace stubserver */

#endif /* BRICKTHREAD_H_ */
