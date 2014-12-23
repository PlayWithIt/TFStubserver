/*
 * FileVisitor.cpp
 *
 * Copyright (C) 2014 Holger Grosenick
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

#include <stdexcept>
#include "File.h"
#include "FileVisitor.h"

namespace utils {

FileVisitor::~FileVisitor() {
}


FileVisitor::VisitResult FileCollector::visitFile(const File &f)
{
    if (recursionCount == 0)
        throw std::logic_error("fileStack is empty, missing call to 'visitDirectory' !");

    if (!filter || (*filter)(f))
    {
        // no filter present: always add
        result->push_back(f);
        ++added;
    }
    return VisitResult::CONTINUE;
}

FileCollector::~FileCollector()
{
    if (allocated)
        delete result;
}

FileVisitor::VisitResult FileCollector::visitDirectory(const File &dir)
{
    ++recursionCount;
    return VisitResult::CONTINUE;
}

FileVisitor::VisitResult FileCollector::afterVisitDirectory(const File &)
{
    if (recursionCount == 0)
        throw std::logic_error("fileStack is empty, missing call to 'visitDirectory' !");
    --recursionCount;
    return VisitResult::CONTINUE;
}

} /* namespace utils */
