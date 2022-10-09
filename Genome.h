#ifndef GENOME_H
#define GENOME_H

#include <iostream>
#include <list>
#include <time.h>
#include <iomanip>
#include <vector>
#include <cmath>

// #define DEBUG

// Node struct contains information of each node
struct Node
{
    // List of activation functions
    enum ActFunc{None, Linear, Sigmoid, Tanh, ReLU, Swish};

    unsigned short int  ID;     // The unique identification number of each node
    std::string         Type;   // The type of the node: Sensor/Hidden/Output
    double              Val;    // The value of the Node
    ActFunc             Mode;   // The activation function the node use

    // Node default constructor
    Node(const unsigned short int _ID, const std::string _Type, const ActFunc _Mode);

    // Activation functions are defined here
    static double act_Linear(const double x);
    static double act_Sigmoid(const double x);
    static double act_Tanh(const double x);
    static double act_ReLU(const double x);
    static double act_Swish(const double x);

    // Member function to select the type of activation function to use
    void ApplyActFunc();
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
    void AddNode(const unsigned int _Percent, const Node::ActFunc _HiddenMode);

    // Add a new connection randomly
    void AddConnection(const unsigned int _Percent);
    bool _PreCalculation(const unsigned int _InputNode, const unsigned int _OutputNode, const unsigned int _InputNum) const;

public:
    // Initialize a Genome with NO hidden units (minimal structure)
    // Initialize 1st generation
    Genome(const unsigned int _InputNodes, const unsigned int _OutputNodes, const Node::ActFunc _OutputMode);
    // Initialize offsprings
    Genome(const std::list<Node> _Nodes, const std::list<Connection> _Connections);

    // Mutate the Genome in five possible ways
    //      * Add connection            (Done)
    //      * Add node                  (Done)
    //      * Change weight             (Done)
    //      * Random weight             (Done)
    //      * Dis/Enable connection     (Done)
    // If you want to disable any type of mutation, just set the percent to 0
    void Mutate(const unsigned int _ToggleConnect_Percent, 
                const unsigned int _MutateWeight_Percent, const unsigned int _RNGPercent, 
                const unsigned int _AddNode_Percent, const Node::ActFunc _HiddenMode, 
                const unsigned int _AddConnection_Percent);

    // Propagate (a.k.a. calculate) the output of the network based on the given input
    // double Propagate(double* _pInput) const;

    // Print the genotype of current genome to inspect
    void PrintGenotype() const;

    // Getters (if need any)
    std::list<Node> getNodes() const;
    std::list<Connection> getConnections() const;
};

#endif