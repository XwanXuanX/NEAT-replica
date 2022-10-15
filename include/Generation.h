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

// Class for mutating, testing, and crossing-over entire population
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