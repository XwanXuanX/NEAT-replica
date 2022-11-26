#ifndef CONNECT_H
#define CONNECT_H

#include "preh.hpp"
#include "utility.hpp"

struct Connection
{
    UINT Innov;
    UINT In;
    UINT Out;
    double Weight;
    bool Enable;

    // with random weight
    Connection(const UINT & _In, const UINT &_Out);
    // with assigned weight
    Connection(const UINT &_In, const UINT &_Out, const double _Weight);

    // mutate connection weight
    void MUTWeight(const bool _isRNG) noexcept(true);

    // operator overloading
    bool operator<(const Connection &_OtherConnection) const;
    bool operator==(const Connection &_OtherConnection) const;
};

#endif