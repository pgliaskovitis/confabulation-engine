#ifndef FORWARDCONFABULATION_H
#define FORWARDCONFABULATION_H

#include "ConfabulationBase.h"

class ForwardConfabulation : public ConfabulationBase
{
public:
    ForwardConfabulation();
    ForwardConfabulation(const ForwardConfabulation& rhs) = delete;
    ForwardConfabulation& operator=(const ForwardConfabulation& rhs) = delete;
    ForwardConfabulation(ForwardConfabulation&& rhs) = delete;
    ForwardConfabulation&& operator=(ForwardConfabulation&& rhs) = delete;
};

#endif // FORWARDCONFABULATION_H
