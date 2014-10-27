/*
 * Copyright 2014 Periklis Liaskovitis
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

#include "Globals.h"
#include "text_processing/NGramHandler.h"

const std::string Globals::kSentenceDelimiters = "\".!?;:";
const std::string Globals::kTokenDelimiters = " ,`\".!?;@#$%^&*()-/\\_=+~[]{}<>0123456789";
const std::vector<std::string> Globals::kCommonAbbreviations = {"Mr.", "Mrs.", "Dr.", "etc.", "Jr.", "Sr.", "Co.", "Ltd.", "A.D.", "B.C."};
const std::string Globals::kDummy = "DummySymbol";
const std::vector<std::string> Globals::kPersistenceDelimiters = {":::", "{--->", "|||", "}--->"};

const float Globals::kBaseProb = 0.0001;
const unsigned int Globals::kBandGap = 1;
const unsigned short Globals::kHeapResults = 3;
