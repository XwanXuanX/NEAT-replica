#ifndef GENOME_H
#define GENOME_H

#include <iostream>
#include <list>
#include <time.h>

// Node struct contains information of each node
struct Node
{
    unsigned short int  ID;     // The unique identification number of each node
    std::string         Type;   // The type of the node: Sensor/Hidden/Output
    double              Val;    // The value of the Node

    // Node default constructor
    Node(const unsigned short int _ID, const std::string _Type);
};

// Connection struct contains information of each connection
struct Connection
{
    unsigned short int  Innov;  // IMPORTANT: The global innovation number of each connection

    unsigned short int  In;     // The input node of the connection
    unsigned short int  Out;    // The output node of the connection

    double              Weight; // The weight of the connection

    bool                Enable; // Indicate whether the current connection is enabled

    // Connection default constructor
    Connection(const unsigned short int _Innov, 
               const unsigned short int _In, 
               const unsigned short int _Out, 
               const bool               _isRNG = true);
};


class Genome
{
private:

public:

};

#endif