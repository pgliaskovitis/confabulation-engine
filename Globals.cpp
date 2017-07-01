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

// Sentence delimiters are considered to enclose some full meaning
// Token delimiters are noise in the raw documents

const std::string Globals::kSentenceDelimiters = "\",.!?;:";
const std::string Globals::kTokenDelimiters = " `\",.!?;:@#$%^&*()-/\\_=+~[]{}<>0123456789";
const std::vector<std::string> Globals::kCommonAbbreviations = {"Mr.", "Mrs.", "Dr.", "etc.", "Jr.", "Sr.", "Co.", "Ltd.", "A.D.", "B.C.", "ch.", "Ch.", "CH."};
const std::string Globals::kDummy = "DummySymbol";
const std::vector<std::string> Globals::kPersistenceDelimiters = {":::", "{--->", "|||", "}--->"};

const float Globals::kBaseProb = 0.0001f;
const float Globals::kBandGap = 1.0f;
const float Globals::kSynapseAmplifier = 1.0f;
const uint8_t Globals::kMaxMultiWordSize = 4;
const uint8_t Globals::kReferenceFrameSize = 15 - 1;
const uint8_t Globals::kMaxCompletionSize = 7;
const size_t Globals::kMaxSingleWordSymbols = 65535;
const size_t Globals::kMaxMultiWordSymbols = 65535;
const bool Globals::kDelimitersAsSymbols = false;
const bool Globals::kFavorLargerPhrases = false;
const bool Globals::kNormalizeInputs = true; // normalization of excitation vectors before being given as input to a knowledge base
const bool Globals::kNormalizeTransfers = false; // activates normalization of excitation vectors produced by a knowledge base
const bool Globals::kUseSingleThreadedVariation = false; // activates full swirl per index, instead of over multiple indices
const bool Globals::kUseMultiThreading = true; // full swirl over multiple indices, with one thread per advanced position
