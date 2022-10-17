/**
 * @file Genome.h
 * @brief Defines the Genome class for an organism
 * @author Yetong (Tony) Li
 * @date Oct 3, 2022
 * @version 1.0.1
*/

#ifndef GENOME_H
#define GENOME_H

#include <iostream>
#include <list>
#include <time.h>
#include <iomanip>
#include <vector>
#include <cmath>
#include <map>
#include <algorithm>

/**
 * @struct Node
 * @brief Contains information of each node
*/
struct Node
{
    /**
     * @enum ActFunc
     * @brief List of activation functions
    */
    enum ActFunc{None, Linear, Sigmoid, Tanh, ReLU, Swish, Modified_Sigmoid};

    unsigned short int  ID;     /*! @brief The unique identification number of each node */
    std::string         Type;   /*! @brief The type of the node: Sensor/Hidden/Output */
    double              Val;    /*! @brief The value of the Node */
    ActFunc             Mode;   /*! @brief The activation function the node use */

    /**
     * @brief Initialize the structure of a genome
     * @param[in] _ID The unique identification number of each node
     * @param[in] _Type The type of the node: Sensor/Hidden/Output
     * @param[in] _Mode The activation function the node use
    */
    Node(const unsigned short int _ID, const std::string _Type, const ActFunc _Mode);

    /**
     * @brief A number of useful activation functions
     * @param[in] x The input value of the function
     * @return The output value of the function
    */
    static double act_Linear(const double x);
    static double act_Sigmoid(const double x);
    static double act_Tanh(const double x);
    static double act_ReLU(const double x);
    static double act_Swish(const double x);
    static double act_Modified_Sigmoid(const double x);

    /*! @brief Apply activation function to the node's output */
    void ApplyActFunc();

    bool operator==(const unsigned short int &_OtherID) const;
    bool operator==(const Node &_OtherNode) const;
    bool operator<(const Node &_OtherNode) const;
};

/**
 * @struct Connection
 * @brief Contains information of each connection
*/
struct Connection
{
    unsigned short int  Innov;  /*! @brief The global innovation number of each connection */

    unsigned short int  In;     /*! @brief The input node of the connection */
    unsigned short int  Out;    /*! @brief The output node of the connection */

    double              Weight; /*! @brief The weight of the connection */

    bool                Enable; /*! @brief Indicate whether the current connection is enabled */

    /**
     * @brief Initialize connection with random weights
     * @param[in] _Innov The Innovation number of the connection
     * @param[in] _In The input node of the connection
     * @param[in] _Out The output node of the connection
    */
    Connection(const unsigned short int _Innov, const unsigned short int _In, const unsigned short int _Out);

    /**
     * @brief Initialize connection with assigned weights
     * @param[in] _Innov The Innovation number of the connection
     * @param[in] _In The input node of the connection
     * @param[in] _Out The output node of the connection
     * @param[in] _Weight The assigned weight of the connection
    */
    Connection(const unsigned short int _Innov, const unsigned short int _In, const unsigned short int _Out, 
               double                   _Weight);

    /**
     * @brief Mutate the connection weight in two ways
     * @param[in] _isRNG When true, randomly assign new weight; When false, nudge the weight slightly
    */
    void MUTWeight(const bool _isRNG=true);

    /**
     * @brief Toggle the enable state of the connection
    */
    void MUTEnable();

    bool operator<(const Connection &_OtherConnection) const;
    bool operator==(const Connection &_OtherConnection) const;
};

/**
 * @class Genome
 * @brief Contains information of Genome
*/
class Genome
{
private:
    std::list<Node>         Nodes;          /*! @brief Node Genes list */
    std::list<Connection>   Connections;    /*! @brief Connection Genes list */
    double                  Fitness;        /*! @brief The fitness of the genome */

    /**
     * @brief Enable/Disable each connection randomly by a percent
     * @param[in] _Percent The probability of toggle a connection
    */
    void ToggleConnect(const unsigned int _Percent);

    /**
     * @brief Assign/Nudge each weight randomly by a percent
     * @param[in] _Percent The probability of mutating a connection weight
     * @param[in] _RNGPercent The probability of mutating a weight randomly
    */
    void MutateWeight(const unsigned int _Percent, const unsigned int _RNGPercent);

    /**
     * @brief Add a new node randomly by a percent
     * @param[in] _Percent The probability of adding a new node
     * @param[in] _HiddenMode The activation function of the new node
    */
    void AddNode(const unsigned int _Percent, const Node::ActFunc _HiddenMode);

    /**
     * @brief Add a new connection randomly by a percent
     * @param[in] _Percent The probability of adding a new connection 
    */
    void AddConnection(const unsigned int _Percent);

    /**
     * @brief Check if a new connection creates feedback loop
     * @param[in] _InputNode The input node of the connection
     * @param[in] _OutputNode The output node of the connection
     * @param[in] _InputNum The number of input nodes of the network
     * @return If connection pass pre-calc test, return true; else, return false
    */
    bool _PreCalculation(const unsigned int _InputNode, const unsigned int _OutputNode, const unsigned int _InputNum) const;

    /**
     * @brief Check if the connection is in the INNOV_DATABASE
     * @param[in] new_connect The new connection about to be added
     * @return If yes, add to Connection Gene list using the existing INNOV number; return true \
     * @return If not, insert new connection and new INNOV number, and add to Connection Gene list; return false
    */
    bool ConnectionProcessor(Connection new_connect);

public:
    /**
     * @brief Initialize a Genome with NO hidden units (minimal structure)
     * @param[in] _InputNodes The number of input nodes
     * @param[in] _OutputNodes The number of output nodes
     * @param[in] _OutputMode The activation function of output nodes
    */
    Genome(const unsigned int _InputNodes, const unsigned int _OutputNodes, const Node::ActFunc _OutputMode);
    
    /**
     * @brief Initialize a Genome with existing node genes and connection genes
     * @param[in] _Nodes The node gene list
     * @param[in] _Connections The connection gene list
    */
    Genome(const std::list<Node> _Nodes, const std::list<Connection> _Connections);

    /**
     * @brief Default constructor (fixed unknown issues in Species.cpp)
    */
    Genome() = default;

    /**
     * @brief Mutate the Genome in five ways: \
     * @brief Add connection; Add node; Change weight; Random weight; Disable/Enable connection. \
     * @brief If you want to disable any type of mutation, just set the percent to 0
     * @param[in] _ToggleConnect_Percent The probability of toggle a connection
     * @param[in] _MutateWeight_Percent The probability of mutating a connection weight
     * @param[in] _RNGPercent The probability of mutating a weight randomly
     * @param[in] _AddNode_Percent The probability of adding a new node
     * @param[in] _HiddenMode The activation function of the new node
     * @param[in] _AddConnection_Percent The probability of adding a new connection 
    */
    void Mutate(const unsigned int _ToggleConnect_Percent, 
                const unsigned int _MutateWeight_Percent, const unsigned int _RNGPercent, 
                const unsigned int _AddNode_Percent, const Node::ActFunc _HiddenMode, 
                const unsigned int _AddConnection_Percent);

    /**
     * @brief Propagate (a.k.a. calculate) the output of the network based on the given input
     * @param[in] *_pInputs The pointer of the input array
     * @param[in] _InputLength The size of the input array
     * @return The ordered list of outputs
    */
    std::vector<double> Propagate(double* _pInputs, const std::size_t _InputLength) const;

    /**
     * @brief Calculate the compatibility distance between two genomes
     * @param[in] &_Other The other genome used to calculate compatibility distance
     * @param[in] c1 Formula param c1
     * @param[in] c2 Formula param c2
     * @param[in] c3 Formula param c3
     * @param[in] _Normalize_Threshold Should enable formula param N
     * @return The compatibility distance between two genomes
    */
    double CompatDistance(const Genome &_Other, 
                          const double c1, const double c2, const double c3, 
                          const unsigned int _Normalize_Threshold = 20) const;

    /**
     * @brief Crossover (a.k.a. breed) between two genomes
     * @param[in] &_Other The other genome to crossover
     * @param[in] this_fitness The fitness of parent 1
     * @param[in] other_fitness The fitness of parent 2
     * @return The resulted offspring
    */
    Genome Crossover(const Genome &_Other, const double this_fitness, const double other_fitness) const;

    /**
     * @brief Calculate the adjusted fitness
     * @param[in] _Species_Num The number of organisms in the same species
    */
    void CalcAdjFitness(const unsigned int _Species_Num);

    /**
     * @brief Print the genotype of current genome to inspect
    */
    void PrintGenotype() const;

    std::list<Node> getNodes() const;
    std::list<Connection> getConnections() const;
    double getFitness() const;
    void setFitness(const double _Fitness);
    
    bool operator>(const Genome &_OtherGenome) const;
};

#endif