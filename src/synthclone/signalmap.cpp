#include "signalmap.h"

SignalMap &
SignalMap::operator<<(const SignalPair &pair)
{
    insert(pair.first, pair.second);
    return *this;
}
