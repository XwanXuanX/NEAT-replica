/**
 * @file Genome.h
 * @brief Defines the Genome class for an organism
 * @author Yetong (Tony) Li
 * @date Oct 3, 2022
 * @version 1.0.1
*/

#ifndef GENOME_H
#define GENOME_H

#include "Base.h"

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

    /**
     * @enum NodeType
     * @brief Three types of nodes
    */
    enum NodeType{Input, Output, Hidden};

    UINT  		ID;     /*! @brief The unique identification number of each node */
    NodeType    Type;   /*! @brief The type of the node: Sensor/Hidden/Output */
    double      Val;    /*! @brief The value of the Node */
    ActFunc     Mode;   /*! @brief The activation function the node use */

    /**
     * @brief Initialize the structure of a genome
     * @param[in] _ID The unique identification number of each node
     * @param[in] _Type The type of the node: Sensor/Hidden/Output
     * @param[in] _Mode The activation function the node use
    */
   	Node(const UINT _ID, const NodeType _Type, const ActFunc _Mode);

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

    /**
	 * @brief Apply activation function to the node's output 
	*/
    void ApplyActFunc();

    bool operator==(const Node &_OtherNode) const;
    bool operator<(const Node &_OtherNode) const;
};

/**
 * @struct Connection
 * @brief Contains information of each connection
*/
struct Connection
{
    UINT	Innov;   /*! @brief The global innovation number of each connection */
    UINT  	In;      /*! @brief The input node of the connection */
    UINT  	Out;     /*! @brief The output node of the connection */
    double	Weight;  /*! @brief The weight of the connection */
    bool	Enable;  /*! @brief Indicate whether the current connection is enabled */

    /**
     * @brief Initialize connection with random weights
     * @param[in] _In The input node of the connection
     * @param[in] _Out The output node of the connection
    */
   	Connection(const UINT* const _In, const UINT* const _Out);

    /**
     * @brief Initialize connection with assigned weights
     * @param[in] _In The input node of the connection
     * @param[in] _Out The output node of the connection
     * @param[in] _Weight The assigned weight of the connection
    */
    Connection(const UINT* const _In, const UINT* const _Out, const double _Weight);

    /**
     * @brief Mutate the connection weight in two ways
     * @param[in] _isRNG When true, randomly assign new weight; When false, nudge the weight slightly
    */
    void MUTWeight(const bool _isRNG=true);

    bool operator<(const Connection &_OtherConnection) const;
    bool operator==(const Connection &_OtherConnection) const;
};

typedef std::vector<Node> 		NODEVEC;
typedef std::vector<Connection> CNCTVEC;

/**
 * @class Genome
 * @brief Contains information of Genome
*/
class Genome
{
private:
    NODEVEC     Nodes;          /*! @brief Node Genes list */
	CNCTVEC 	Connections;    /*! @brief Connection Genes list */
    double		Fitness;        /*! @brief The fitness of the genome */

    /**
     * @brief Assign/Nudge each weight randomly by a percent
     * @param[in] _Percent The probability of mutating a connection weight
     * @param[in] _RNGPercent The probability of mutating a weight randomly
    */
    void MutateWeight(const UINT* const _Percent, const UINT* const _RNGPercent);

    /**
     * @brief Add a new node randomly by a percent
     * @param[in] _Percent The probability of adding a new node
     * @param[in] _HiddenMode The activation function of the new node
    */
    void AddNode(const UINT* const _Percent, const Node::ActFunc* const _HiddenMode);

    /**
     * @brief Add a new connection randomly by a percent
     * @param[in] _Percent The probability of adding a new connection 
    */
    void AddConnection(const UINT* const _Percent);

    /**
     * @brief Check if a new connection creates feedback loop
     * @param[in] _InputNode The input node of the connection
     * @param[in] _OutputNode The output node of the connection
     * @param[in] _InputNum The number of input nodes of the network
     * @return If connection pass pre-calc test, return true; else, return false
    */
    bool _PreCalculation(const UINT* const _InputNode, const UINT* const _OutputNode, 
						 const UINT* const _InputNum) const;

    /**
     * @brief Check if the connection is in the INNOV_DATABASE
     * @param[in] new_connect The new connection about to be added
     * @return If yes, add to Connection Gene list using the existing INNOV number; return true \
     * @return If not, insert new connection and new INNOV number, and add to Connection Gene list; return false
    */
    bool ConnectionProcessor(Connection* const new_connect);

public:
    /**
     * @brief Initialize a Genome with NO hidden units (minimal structure)
     * @param[in] _InputNodes The number of input nodes
     * @param[in] _OutputNodes The number of output nodes
     * @param[in] _OutputMode The activation function of output nodes
    */
    Genome(const UINT* const _InputNodes, const UINT* const _OutputNodes, 
		   const Node::ActFunc* const _OutputMode);
    
    /**
     * @brief Initialize a Genome with existing node genes and connection genes
     * @param[in] _Nodes The node gene list
     * @param[in] _Connections The connection gene list
    */
   	Genome(const NODEVEC* const _Nodes, const CNCTVEC* const _Connections);

    /**
     * @brief Default constructor (fixed unknown issues in Species.cpp)
    */
    Genome() = default;

    /**
     * @brief Mutate the Genome in five ways: \
     * @brief Add connection; Add node; Change weight; Random weight; Disable/Enable connection. \
     * @brief If you want to disable any type of mutation, just set the percent to 0
     * @param[in] _MutateWeight_Percent The probability of mutating a connection weight
     * @param[in] _RNGPercent The probability of mutating a weight randomly
     * @param[in] _AddNode_Percent The probability of adding a new node
     * @param[in] _HiddenMode The activation function of the new node
     * @param[in] _AddConnection_Percent The probability of adding a new connection 
    */
    void Mutate(const UINT* const _MutateWeight_Percent, const UINT* const _RNGPercent, 
                const UINT* const _AddNode_Percent, const Node::ActFunc* const _HiddenMode, 
                const UINT* const _AddConnection_Percent);

    /**
     * @brief Propagate (a.k.a. calculate) the output of the network based on the given input
     * @param[in] _pInputs The pointer of the input array
     * @param[in] _InputLength The size of the input array
     * @return The ordered list of outputs
    */
    DBLEVEC Propagate(const double* const _pInputs, const std::size_t _InputLength) const;

    /**
     * @brief Calculate the compatibility distance between two genomes
     * @param[in] _Other The other genome used to calculate compatibility distance
     * @param[in] c1 Formula param c1
     * @param[in] c2 Formula param c2
     * @param[in] c3 Formula param c3
     * @param[in] _Normalize_Threshold Should enable formula param N
     * @return The compatibility distance between two genomes
    */
    double CompatDistance(const Genome* const _Other, 
                          const double* const c1, const double* const c2, const double* const c3, 
                          const UINT* const _Normalize_Threshold) const;

    /**
     * @brief Crossover (a.k.a. breed) between two genomes
     * @param[in] _Other The other genome to crossover
     * @param[in] this_fitness The fitness of parent 1
     * @param[in] other_fitness The fitness of parent 2
     * @return The resulted offspring
    */
    Genome Crossover(const Genome* const _Other, const double* const this_fitness, 
					 const double* const other_fitness) const;

    /**
     * @brief Calculate the adjusted fitness
     * @param[in] _Species_Num The number of organisms in the same species
    */
    void CalcAdjFitness(const UINT* const _Species_Num);

    /**
     * @brief Print the genotype of current genome to inspect
    */
    void PrintGenotype() const;

    NODEVEC getNodes() const;
    CNCTVEC getConnections() const;
    double getFitness() const;
    void setFitness(const double _Fitness);
    bool operator>(const Genome &_OtherGenome) const;
};

#endif