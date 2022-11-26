#include "../inc/connect.hpp"

// random weights
Connection::Connection(const UINT & _In, const UINT &_Out)
    : In(_In), Out(_Out), Innov(0), Enable(true), 
      Weight((double)rand() / ((double)RAND_MAX / 4) - 2.0) {}

// assigned weights
Connection::Connection(const UINT &_In, const UINT &_Out, const double _Weight)
    : In(_In), Out(_Out), Innov(0), Enable(true), Weight(_Weight) {}

// mutate connection weight
void Connection::MUTWeight(const bool _isRNG) noexcept(true)
{
    (_isRNG) ? \
        this->Weight = (double)rand() / ((double)RAND_MAX / 4) - 2.0 :
        this->Weight *= (double)rand() / ((double)RAND_MAX / 2);
}

// operator overloading
bool Connection::operator<(const Connection &_OtherConnection) const {
    return (this->Innov < _OtherConnection.Innov);
}

bool Connection::operator==(const Connection &_OtherConnection) const {
    return (this->Innov == _OtherConnection.Innov);
}