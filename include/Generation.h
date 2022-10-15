#ifndef GENERATION_H
#define GENERATION_H

#include "Species.h"

#define DEBUG
#define SPECIATE

// This structure contains the layout of the initial population
struct GenomeLayout
{
    unsigned int InputNodes;
    unsigned int OutputNodes;
    Node::ActFunc OutputMode;
    
    // Constructor to initialize the structure
    GenomeLayout(const unsigned int _InputNodes, const unsigned int _OutputNodes, Node::ActFunc _OutputMode);
};

// This structure contains the parameters to mutate a genome
struct MutateParams
{   
    // Probability to toggle a connection
    unsigned int ToggleConnect_Percent;

    // Probability to change a weight
    unsigned int MutateWeight_Percent; 
    unsigned int RNGPercent;  // Probability to change a weight RANDOMLY

    // Probability to add a node
    unsigned int AddNode_Percent;
    Node::ActFunc HiddenMode; // The type of activation function hidden nodes should use

    // Probability to add a connection
    unsigned int AddConnection_Percent;

    // Constructor to initialize the structure
    MutateParams(const unsigned int _ToggleConnect_Percent, 
                 const unsigned int _MutateWeight_Percent, const unsigned int _RNGPercent, 
                 const unsigned int _AddNode_Percent, const Node::ActFunc _HiddenMode, 
                 const unsigned int _AddConnection_Percent);
};


class Generation
{
private:
    unsigned int         Pop_Num;    // Keep track of the size of the population
    Genome*              Population; // Pointer to a dynamically allocated array containing all Genomes
    std::vector<Species> AllSpecies; // List of all species within the generation

public:
    // Constructor to create the initial population of the generation
    Generation(const GenomeLayout _Layout, const unsigned int _Pop_Num);

    // Destructor: free the dynamically allocated memory
    ~Generation();

    // Mutate the entire population based on some parameters
    void Mutate(const MutateParams _Params);

    // Test and evaluate (to be determined...)

    // Speciate based on compatibility distance
    void Speciate(const double _CompatThreshold, const CompatDistParams _Params);

    // Reproduce within species and refresh population
    void Reproduce(const double _Kill_Percent);
};

#endif