/*
 * SymbolAttribute.cpp
 *
 *  Created on: Jun 1, 2013
 *      Author: Periklis G. Liaskovitis
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

