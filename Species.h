#ifndef SPECIES_H
#define SPECIES_H

#include "Genome.h"

#define DEBUG
#define REPRODUCE

// This struct contains necessary params for calculating Compat Distance function
struct CompatDistParams
{
    double c1, c2, c3;
    unsigned int NormalThreshold;

    CompatDistParams(const double _c1, const double _c2, const double _c3,
                     const unsigned int _NormalThreshold);
};

// Species class must contain: 
// A list (vector) of genomes and a representative genome
class Species
{
private:
    Genome              Represent;  // The genome used to represent the entire species
    std::vector<Genome> Organisms;  // The list of genomes within the species

public:
    // Constructor: Initialize a new species with a genome
    Species(const Genome _NewGenome);

    // Check if a genome should be added to this species; if yes, add and return true
    bool AddOrganism(const Genome _NewGenome, const double _CompatThreshold, const CompatDistParams _Params);
    
    // Calculate the adjusted fitness for every organisms in the species
    void CalcAdjFitness();

    // Clear species for later generations
    void ClearSpecies();

    // Crossover to produce certain number of offsprings
    std::vector<Genome> Reproduce(const unsigned int _Num_Offsprings) const;

    // Get the total adjusted fitness of the species
    double TotalAdjFitness() const;

    // Getters (if need any)
    Genome getRepresent() const;
    std::vector<Genome> getOrganisms() const;
};

#endif