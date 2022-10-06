#ifndef GENOME_H
#define GENOME_H

#include <iostream>
#include <list>
#include <time.h>
#include <iomanip>


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

    // Mutate weight: when true, randomly assign new weight; otherwise, nudge the weight slightly
    void MUTWeight(const bool _isRNG=true);

    // Mutate enable: toggle the enabled state of the connection
    void MUTEnable();
};


class Genome
{
private:
    std::list<Node>         Nodes;          // Node Genes list
    std::list<Connection>   Connections;    // Connection Genes list

    // Enable/Disable each connection randomly by a percent
    void ToggleConnect(const unsigned int _Percent);

    // Assign/Nudge each weight randomly by a percent
    void MutateWeight(const unsigned int _Percent, const unsigned int _RNGPercent);

    // Add a new node randomly
    void AddNode(const unsigned int _Percent);

    // Add a new connection randomly
    void AddConnection(const unsigned int _Percent);

public:
    // Initialize a Genome with NO hidden units (minimal structure)
    // Initialize 1st generation
    Genome(const unsigned int _InputNodes, const unsigned int _OutputNodes);
    // Initialize offsprings
    Genome(const std::list<Node> _Nodes, const std::list<Connection> _Connections);

    // Mutate the Genome in three possible ways
    //      * Add connection            ()
    //      * Add node                  (*)
    //      * Change weight             (*)
    //      * Random weight             (*)
    //      * Dis/Enable connection     (*)


    // Print the genotype of current genome to inspect
    void PrintGenotype() const;

    // Getters (if need any)
    std::list<Node> getNodes() const;
    std::list<Connection> getConnections() const;
};

#endif