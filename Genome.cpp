#include "Genome.h"

// Initialize a new node with initial value 0
Node::Node(const unsigned short int _ID, const std::string _Type)
{
    this->ID    = _ID;
    this->Type  = _Type;
    this->Val   = 0;
}

// Initialize a new connection with random weight or 1.0
Connection::Connection(const unsigned short int _Innov, 
                       const unsigned short int _In, 
                       const unsigned short int _Out, 
                       const bool               _isRNG = true)
{
    this->Innov  = _Innov;
    this->In     = _In;
    this->Out    = _Out;
    this->Enable = true;

    if(_isRNG)
    {
        srand((unsigned)time(NULL));    // random seed used to generate random numbers
        this->Weight = static_cast<double>(rand() / RAND_MAX);
    }
    else
        this->Weight = 1.0;
}