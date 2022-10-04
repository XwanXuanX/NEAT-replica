#ifndef GENOME_H
#define GENOME_H

#include <iostream>
#include <list>
#include <time.h>

// IMPORTANT: Innovation number (global throughout the project)
unsigned short int INNOV = 0;


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

    // Initialize connection with random weights
    Connection(const unsigned short int _Innov, const unsigned short int _In, const unsigned short int _Out);

    // Initialize connection with an assigned weight
    Connection(const unsigned short int _Innov, const unsigned short int _In, const unsigned short int _Out, 
               double                   _Weight);
};


class Genome
{
private:
    std::list<Node>         Nodes;
    std::list<Connection>   Connections;

public:
    // Initialize a Genome with NO hidden units (minimal structure)
    // Initialize 1st generation
    Genome(const unsigned int _InputNodes, const unsigned int _OutputNodes);
    // Initialize offsprings
    Genome(const std::list<Node> _Nodes, const std::list<Connection> _Connections);


    // Getters (if need any)
    std::list<Node> getNodes() const;
    std::list<Connection> getConnections() const;
};

#endif