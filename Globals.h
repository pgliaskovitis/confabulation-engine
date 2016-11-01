/*
 * Copyright 2014 Periklis G. Liaskovitis
 *
 * This file is part of confab-engine.
 *
 * confab-engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * confab-engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with confab-engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <string>
#include <memory>
#include <vector>

class Globals
{
public:
    static const std::string kSentenceDelimiters;
    static const std::string kTokenDelimiters;
    static const std::vector<std::string> kCommonAbbreviations;
    static const std::string kDummy;
    static const std::vector<std::string> kPersistenceDelimiters;

    static const float kBaseProb;
    static const uint8_t kBandGap;
    static const uint8_t kMaxMultiWordSize;
    static const uint8_t kReferenceFrameSize;
    static const uint8_t kMaxCompletionSize;
    static const bool kDelimitersAsSymbols;
    static const size_t kMaxSingleWordSymbols;
    static const size_t kMaxMultiWordSymbols;
    static const bool kFavorLargerPhrases;

    Globals() = delete;
    Globals(const Globals& rhs) = delete;
    Globals& operator=(const Globals& rhs) = delete;
    Globals(Globals&& rhs) = delete;
    Globals&& operator=(Globals&& rhs) = delete;
};

#endif /* GLOBALS_H_ */
