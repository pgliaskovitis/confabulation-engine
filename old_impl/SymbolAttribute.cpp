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

#include "SymbolAttribute.h"

SymbolAttribute::SymbolAttribute(float initial_source_excitation,
								 float initial_target_excitation,
								 std::unique_ptr<Synapse> initial_knowledge_link) :
	source_excitation_(initial_source_excitation),
    target_excitation_(initial_target_excitation),
    knowledge_link_(std::move(initial_knowledge_link))
{
}

float SymbolAttribute::get_source_excitation()
{
	return source_excitation_;
}

float SymbolAttribute::get_target_excitation()
{
    return target_excitation_;
}

void SymbolAttribute::set_source_excitation(float source_excitation)
{
    source_excitation_ = source_excitation;
}

void SymbolAttribute::set_target_excitation(float target_excitation)
{
    target_excitation_ = target_excitation;
}

Synapse& SymbolAttribute::get_knowledge_link()
{
    return *knowledge_link_;
}

