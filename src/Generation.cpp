/**
 * @file Generation.cpp
 * @brief Function bodies of function declarations in Generation.h
 * @author Yetong (Tony) Li
 * @date Oct 14, 2022
 * @version 1.0.1
*/

#include "Generation.h"

/*_______________________________________________________________________________________________________*/
/*                                          GenomeLayout Struct                                          */

/**
 * @brief Constructor to initialize the structure of initial population
 * @param[in] _InputNodes The number of input nodes
 * @param[in] _OutputNodes The number of output nodes
 * @param[in] _OutputMode The activation function of output nodes
*/
GenomeLayout::GenomeLayout(const unsigned int _InputNodes, const unsigned int _OutputNodes, Node::ActFunc _OutputMode)
{
    this->InputNodes = _InputNodes;
    this->OutputNodes = _OutputNodes;
    this->OutputMode = _OutputMode;
}

/*_______________________________________________________________________________________________________*/
/*                                          Generation Class                                             */

/**
 * @brief Constructor to create the initial population of the generation
 * @param[in] _Layout The network layout of initial population
 * @param[in] _Pop_Num The number of organisms contained in the population
*/
Generation::Generation(const GenomeLayout _Layout, const unsigned int _Pop_Num)
{
    // Keep track of the number of population
    this->Pop_Num = _Pop_Num;

    // Dynamically allocate an array with length _Pop_Num
    this->Population = new Genome[_Pop_Num];

    // Assign new genomes into the array
    for(unsigned int i = 0; i < _Pop_Num; i++)
    {
        Genome gene(_Layout.InputNodes, _Layout.OutputNodes, _Layout.OutputMode);
        this->Population[i] = gene;
    }

    // Initialize the species list to none
    this->AllSpecies = {};
    this->Gen_number = 1;
    this->BestFitness = 0;
    this->BestOrganism = nullptr;
}

/**
 * @brief Destructor: free the dynamically allocated memory
*/
Generation::~Generation()
{
    delete[] this->Population;
    this->Population = nullptr;
    this->BestOrganism = nullptr;
}

/**
 * @brief Mutate every member in the population
 * @param[in] _Params Parameters used to mutate an organism
*/
void Generation::Mutate(const MutateParams _Params)
{
    // Mutate every genome within the population
    for(unsigned int i = 0; i < this->Pop_Num; i++)
    {
        this->Population[i].Mutate(_Params.ToggleConnect_Percent,
                                   _Params.MutateWeight_Percent, _Params.RNGPercent,
                                   _Params.AddNode_Percent, _Params.HiddenMode,
                                   _Params.AddConnection_Percent);
#if (defined DEBUG) && (defined GENMUTATE)
        std::cout << i + 1 << ": Mutated succesfully" << std::endl;
#endif
    }
}

/**
 * @brief Test and evaluate every member in the population
 * @param[in] _Test User-customed function to test and evaluate an organism; return the fitness
*/
void Generation::Evaluate(double (*_Test)(const Genome*))
{
    // Evaluate every genome within the population, using the custom function
    for(unsigned int i = 0; i < this->Pop_Num; i++)
    {
        double Fitness = _Test(&this->Population[i]);
        this->Population[i].setFitness(Fitness);

        if(Fitness > this->BestFitness)
        {
            this->BestFitness = Fitness;
            this->BestOrganism = &this->Population[i];
        }
    }
}

/**
 * @brief Print out the information about this generation
*/
void Generation::PrintGenInfo() const
{
    std::cout << "_____________________" << std::endl;
    std::cout << "Generation: " << "\t" << this->Gen_number << std::endl;
    std::cout << "Population: " << "\t" << this->Pop_Num << std::endl;
    double total_fitness = 0;
    for(unsigned int i = 0; i < Pop_Num; i++)
    {
        total_fitness += this->Population[i].getFitness();
    }
    std::cout << "Ave Fitness: " << "\t" << (total_fitness / Pop_Num) << std::endl;
    std::cout << "Num Species: " << "\t" << this->AllSpecies.size() << std::endl;
}

/**
 * @brief Speciate based on compatibility distance
 * @param[in] _CompatThreshold The compatibility threshold
 * @param[in] _Params Parameters used to calculate compatibility formula
*/
void Generation::Speciate(const double _CompatThreshold, const CompatDistParams _Params)
{
    for(unsigned int i = 0; i < this->Pop_Num; i++)
    {
        // If there is initially no species, create a new one
        if(this->AllSpecies.size() == 0)
        {
            Species new_species(this->Population[i]);
            this->AllSpecies.push_back(new_species);
#if (defined DEBUG) && (defined SPECIATE)
            std::cout << this->AllSpecies.size() << ": [" << i + 1 << "] creates new species" << std::endl;
#endif
            continue;
        }
        // Compare the organism with the representitive of each species
        else
        {
            bool should_create = true;
            for(unsigned int j = 0; j < this->AllSpecies.size(); j++)
            {
                if(this->AllSpecies.at(j).AddOrganism(this->Population[i], _CompatThreshold, _Params))
                {
                    should_create = false;
#if (defined DEBUG) && (defined SPECIATE)
                    std::cout << this->AllSpecies.size() << ": [" << i + 1 << "] was added to species " 
                              << "[" << j + 1 << "]" << std::endl;
#endif
                    break;
                }
            }

            if(should_create)
            {
                Species new_species(this->Population[i]);
                this->AllSpecies.push_back(new_species);
#if (defined DEBUG) && (defined SPECIATE)
                std::cout << this->AllSpecies.size() << ": [" << i + 1 << "] creates new species" << std::endl;
#endif
                continue;
            }
        }
    }
}

/**
 * @brief Reproduce within species and refresh population
 * @param[in] _Kill_Percent The percentage of organisms to be killed
 * @param[in] _Threshold_Gen If the max fitness of a species does not increase for 
 *                           [_Threshold_Gen] generations, the species is not allowed to reproduce
 * @param[in] _Mut_Percent The percentage of offsprings that resulted from mutation instead of crossover
 * @param[in] _Params Parameters used to mutate an organism
*/
void Generation::Reproduce(const double _Kill_Percent, const unsigned int _Threshold_Gen, 
                           const double _Mut_Percent, const MutateParams _Params)
{
    std::vector<Genome> Offsprings;
    // Calculate the modified fi for all species and sum up all the fis
    std::vector<unsigned int> Unfit_Species;
    double TotalFitness = 0;
    for(unsigned int i = 0; i < this->AllSpecies.size(); i++)
    {
        this->AllSpecies.at(i).CalcAdjFitness();
        if(this->AllSpecies.at(i).CheckMaxFitGen(_Threshold_Gen))
            TotalFitness += this->AllSpecies.at(i).TotalAdjFitness();
        else
        {
            Unfit_Species.push_back(i);
#if (defined DEBUG) && (defined GENREPRODUCE)
            std::cout << "Unfit: " << i + 1 << std::endl;
#endif
        }
    }

    // Assign the potential number of offsprings for each species, except for the last species
    unsigned int Num_Offsprings = 0;
    std::vector<unsigned int>::iterator find_unfit;
    for(unsigned int i = 0; i < this->AllSpecies.size() - 1; i++)
    {
        // Check if the species is unfit
        find_unfit = std::find(Unfit_Species.begin(), Unfit_Species.end(), i);
        if(find_unfit != Unfit_Species.end())
        {
#if (defined DEBUG) && (defined GENREPRODUCE)
            std::cout << "Excluded: " << i + 1 << std::endl;
#endif
            continue;
        }

        Num_Offsprings = std::round((this->AllSpecies.at(i).TotalAdjFitness() / 
                                     TotalFitness) * this->Pop_Num);
        std::vector<Genome> tmp = \
            this->AllSpecies.at(i).Reproduce(Num_Offsprings, _Kill_Percent, _Mut_Percent, _Params);
        
        Offsprings.insert(Offsprings.end(), tmp.begin(), tmp.end());
    }

    // For the last species
    find_unfit = std::find(Unfit_Species.begin(), Unfit_Species.end(), this->AllSpecies.size() - 1);
    if(find_unfit == Unfit_Species.end())
    {
        Num_Offsprings = this->Pop_Num - Offsprings.size();
        // Check with theoretical value
        unsigned int Theory = std::round((this->AllSpecies.at(this->AllSpecies.size() - 1).TotalAdjFitness() / 
                                          TotalFitness) * this->Pop_Num);
        Theory = std::abs((int)(Num_Offsprings - Theory));

        std::vector<Genome> tmp = \
            this->AllSpecies.at(this->AllSpecies.size() - 1).Reproduce(Num_Offsprings, _Kill_Percent, _Mut_Percent, _Params);
        
        Offsprings.insert(Offsprings.end(), tmp.begin(), tmp.end());
    }

#if (defined DEBUG) && (defined GENREPRODUCE)
    std::cout << "Offspring size: " << Offsprings.size() << std::endl;
#endif
    assert(Offsprings.size() == this->Pop_Num);

    // Replace the existing population with new offsprings
    for(unsigned int i = 0; i < this->Pop_Num; i++)
    {
        this->Population[i] = Offsprings.at(i);
    }

    // Clear all species
    for(unsigned int i = 0; i < this->AllSpecies.size(); i++)
    {
        this->AllSpecies.at(i).ClearSpecies();
    }

    // Increase generation number
    this->Gen_number++;
}

Genome* Generation::getChampion() const
{
    return this->BestOrganism;
}