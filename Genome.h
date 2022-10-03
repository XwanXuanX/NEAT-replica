#ifndef GENOME_H
#define GENOME_H

#include <iostream>
#include <list>

// Node struct contains information of each node
struct Node
{
    unsigned short int  ID;     // The unique identification number of each node
    std::string         Type;   // The type of the node: Sensor/Hidden/Output
    double              Val;    // The value of the Node
};

// Connection struct contains information of each connection
struct Connection
{
    unsigned short int  Innov;  // IMPORTANT: The global innovation number of each connection

    unsigned short int  In;     // The input node of the connection
    unsigned short int  Out;    // The output node of the connection

    double              Weight; // The weight of the connection

    bool                Enable; // Indicate whether the current connection is enabled
};

class Genome
{
private:

public:

};

#endif