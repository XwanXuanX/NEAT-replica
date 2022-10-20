/**
 * @file Generation.h
 * @brief Defines generation container for entire population
 * @author Yetong (Tony) Li
 * @date Oct 14, 2022
 * @version 1.0.1
*/

#ifndef GENERATION_H
#define GENERATION_H

#include "Species.h"

/**
 * @struct GenomeLayout
 * @brief Contains the layout of the initial population
*/
struct GenomeLayout
{
    unsigned int InputNodes;    /*! @brief The number of input nodes for initial population */
    unsigned int OutputNodes;   /*! @brief The number of output nodes for initial population */
    Node::ActFunc OutputMode;   /*! @brief The activation function of output nodes */

    /**
     * @brief Constructor to initialize the structure of initial population
     * @param[in] _InputNodes The number of input nodes
     * @param[in] _OutputNodes The number of output nodes
     * @param[in] _OutputMode The activation function of output nodes
    */
    GenomeLayout(const unsigned int _InputNodes, const unsigned int _OutputNodes, Node::ActFunc _OutputMode);
};

/**
 * @class Generation
 * @brief Mutating, testing, speciating, and crossing-over entire population
*/
class Generation
{
private:
    unsigned int         Pop_Num;       /*! @brief Keep track of the size of the population */
    Genome*              Population;    /*! @brief Pointer to a dynamically allocated array containing all Genomes */
    std::vector<Species> AllSpecies;    /*! @brief List of all species within the generation */
    unsigned int         Gen_number;    /*! @brief The current number of generation */
    double               BestFitness;   /*! @brief The best fitness of entire population */
    Genome*              BestOrganism;  /*! @brief The pointer to the best organism of entire population */

public:
    /**
     * @brief Constructor to create the initial population of the generation
     * @param[in] _Layout The network layout of initial population
     * @param[in] _Pop_Num The number of organisms contained in the population
    */
    Generation(const GenomeLayout _Layout, const unsigned int _Pop_Num);

    /**
     * @brief Destructor: free the dynamically allocated memory
    */
    ~Generation();

    /**
     * @brief Mutate every member in the population
     * @param[in] _Params Parameters used to mutate an organism
    */
    void Mutate(const MutateParams _Params);

    /**
     * @brief Test and evaluate every member in the population
     * @param[in] _Test User-customed function to test and evaluate an organism; return the fitness
    */
    void Evaluate(double (*_Test)(const Genome*));

    /**
     * @brief Print out the information about this generation
    */
    void PrintGenInfo() const;

    /**
     * @brief Speciate based on compatibility distance
     * @param[in] _CompatThreshold The compatibility threshold
     * @param[in] _Params Parameters used to calculate compatibility formula
    */
    void Speciate(const double _CompatThreshold, const CompatDistParams _Params);

    /**
     * @brief Reproduce within species and refresh population
     * @param[in] _Kill_Percent The percentage of organisms to be killed
     * @param[in] _Threshold_Gen If the max fitness of a species does not increase for 
     *                           [_Threshold_Gen] generations, the species is not allowed to reproduce
     * @param[in] _Mut_Percent The percentage of offsprings that resulted from mutation instead of crossover
     * @param[in] _Params Parameters used to mutate an organism
    */
    void Reproduce(const double _Kill_Percent, const unsigned int _Threshold_Gen, 
                   const double _Mut_Percent, const MutateParams _Params);

    Genome* getChampion() const;
};

#endif