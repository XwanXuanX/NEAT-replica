#include "Generation.h"

// Constructor to initialize the structure
GenomeLayout::GenomeLayout(const unsigned int _InputNodes, const unsigned int _OutputNodes, Node::ActFunc _OutputMode)
{
    this->InputNodes = _InputNodes;
    this->OutputNodes = _OutputNodes;
    this->OutputMode = _OutputMode;
}

// Constructor to create the initial population of the generation
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
}

// Destructor: free the dynamically allocated memory
Generation::~Generation()
{
    delete[] this->Population;
}

// Mutate the entire population based on some parameters
void Generation::Mutate(const MutateParams _Params)
{
    // Mutate every genomes within the population
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

// Test and evaluate (to be determined...)


// Speciate based on compatibility distance
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

// Reproduce within species and refresh population
void Generation::Reproduce(const double _Kill_Percent)
{
    // Calculate the modified fi for all species and sum up all the fis
    double TotalFitness = 0;
    for(unsigned int i = 0; i < this->AllSpecies.size(); i++)
    {
        this->AllSpecies.at(i).CalcAdjFitness();
        TotalFitness = this->AllSpecies.at(i).TotalAdjFitness();
    }

    // Assign the potential number of offsprings for each species
    for(unsigned int i = 0; i < this->AllSpecies.size(); i++)
    {
        unsigned int Num_Offsprings = 0;
        //Num_Offsprings = 
    }
}
