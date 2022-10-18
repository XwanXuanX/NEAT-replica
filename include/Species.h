/**
 * @file Species.h
 * @brief Define a species container of multiple organisms
 * @author Yetong (Tony) Li
 * @date Oct 13, 2022
 * @version 1.0.1
*/

#ifndef SPECIES_H
#define SPECIES_H

#include "Genome.h"

/**
 * @struct CompatDistParams
 * @brief Contains necessary params for calculating Compat Distance function
*/
struct CompatDistParams
{
    double c1, c2, c3;
    unsigned int NormalThreshold;

    /**
     * @brief Constructor to initialize CompatDistParams struct
     * @param[in] _c1 Formula param c1
     * @param[in] _c2 Formula param c2
     * @param[in] _c3 Formula param c3
     * @param[in] _NormalThreshold Should enable formula param N
    */
    CompatDistParams(const double _c1, const double _c2, const double _c3,
                     const unsigned int _NormalThreshold);
};

/**
 * @struct MutateParams
 * @brief Contains the parameters to mutate a genome
*/
struct MutateParams
{   
    unsigned int ToggleConnect_Percent;     /*! @brief Probability to toggle a connection */
    unsigned int MutateWeight_Percent;      /*! @brief Probability to change a weight */
    unsigned int RNGPercent;                /*! @brief Probability to change a weight RANDOMLY */
    unsigned int AddNode_Percent;           /*! @brief Probability to add a node */
    Node::ActFunc HiddenMode;               /*! @brief The type of activation function hidden nodes should use */
    unsigned int AddConnection_Percent;     /*! @brief Probability to add a connection */
    
    /**
     * @brief Constructor to initialize MutateParams struct
     * @param[in] _ToggleConnect_Percent Probability to toggle a connection
     * @param[in] _MutateWeight_Percent Probability to change a weight
     * @param[in] _RNGPercent Probability to change a weight RANDOMLY
     * @param[in] _AddNode_Percent Probability to add a node
     * @param[in] _HiddenMode The type of activation function hidden nodes should use
     * @param[in] _AddConnection_Percent Probability to add a connection
    */
    MutateParams(const unsigned int _ToggleConnect_Percent, 
                 const unsigned int _MutateWeight_Percent, const unsigned int _RNGPercent, 
                 const unsigned int _AddNode_Percent, const Node::ActFunc _HiddenMode, 
                 const unsigned int _AddConnection_Percent);
};

/**
 * @class Species
 * @brief Species container for multiple organisms when speciation
*/
class Species
{
private:
    Genome              Represent;  /*! @brief The genome used to represent the entire species */
    std::vector<Genome> Organisms;  /*! @brief The list of genomes within the species */

    double              MaxFit;     /*! @brief The maximum fitness of the species */
    unsigned int        MaxFitGen;  /*! @brief The number of generations which the max fitness remain unchanged */

public:
    /**
     * @brief Initialize a new species with a genome
     * @param[in] _NewGenome The first organism to be added to this species
    */
    Species(const Genome _NewGenome);

    /**
     * @brief Check if a genome should be added to this species
     * @param[in] _NewGenome New organism to be added to this species
     * @param[in] _CompatThreshold Compatibility threshold to determine if it should be added
     * @param[in] _Params Parameters used to calculate the compatibility distance formula
     * @return If new organism is added, return true; else, return false
    */
    bool AddOrganism(const Genome _NewGenome, const double _CompatThreshold, const CompatDistParams _Params);

    /**
     * @brief Calculate the adjusted fitness for every organisms in the species
    */
    void CalcAdjFitness();

    /**
     * @brief Clear species for later generations
    */
    void ClearSpecies();

    /**
     * @brief Crossover to produce certain number of offsprings
     * @param[in] _Num_Offsprings The assigned number of offpsrings that a species can produce
     * @param[in] _Kill_Percent Decides how many members should be killed
     * @param[in] _Mut_Percent Decides how many offsprings should be produced through mutation
     * @param[in] _Params Parameters used to mutate an organism
     * @return A list (vector) of offsprings
    */
    std::vector<Genome> Reproduce(const unsigned int _Num_Offsprings, 
                                  const double _Kill_Percent,
                                  const double _Mut_Percent,
                                  const MutateParams _Params);

    /**
     * @brief Get the total adjusted fitness of the species
     * @return The total adjusted fitness
    */
    double TotalAdjFitness() const;

    /**
     * @brief Print basic information about the species
    */
    void PrintSpeciesInfo() const;

    /**
     * @brief Update the Maximum fitness and its generation number
     * @param[in] _Threshold_Gen If the maximum fitness of a species does not increase
     *                           for [Threshold generation] generations, it is not allowed to reproduce
     * @return If current generation exceeds the threshold, return false; else, return true
    */
    bool CheckMaxFitGen(const unsigned int _Threshold_Gen);
    
    Genome getRepresent() const;
    std::vector<Genome> getOrganisms() const;
    double getMaxFit() const;
    unsigned int getMaxFitGen() const;
    unsigned int getSpeciesSize() const;
};

#endif