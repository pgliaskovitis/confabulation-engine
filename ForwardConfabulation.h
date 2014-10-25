#ifndef FORWARDCONFABULATION_H
#define FORWARDCONFABULATION_H

#include "ConfabulationBase.h"

class ForwardConfabulation : public ConfabulationBase
{
public:
    ForwardConfabulation(size_t num_modules, const std::string &symbol_file, const std::string &master_file);
    ForwardConfabulation(const ForwardConfabulation& rhs) = delete;
    ForwardConfabulation& operator=(const ForwardConfabulation& rhs) = delete;
    ForwardConfabulation(ForwardConfabulation&& rhs) = delete;
    ForwardConfabulation&& operator=(ForwardConfabulation&& rhs) = delete;

protected:
    virtual int AutoIndexToComplete(const std::vector<std::string>& symbols);
    virtual bool CheckIndex(const std::vector<std::string>& symbols, int index_to_complete);
    virtual bool CheckArguments(const std::vector<std::string>& symbols, int index_to_complete);
};

#endif // FORWARDCONFABULATION_H
