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
    this->Gen_number = 1;
}

// Destructor: free the dynamically allocated memory
Generation::~Generation()
{
    delete[] this->Population;
}

// Mutate the entire population based on some parameters
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

// Test and evaluate the population
void Generation::Evaluate(void (*_Test)(Genome*))
{
    // Evaluate every genome within the population, using the custom function
    for(unsigned int i = 0; i < this->Pop_Num; i++)
    {
        // Fitness will be assigned inside the _Test function, thus pass in as pointer
        _Test(&this->Population[i]);
    }
}

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
            Unfit_Species.push_back(i);
    }

    // Assign the potential number of offsprings for each species, except for the last species
    unsigned int Num_Offsprings = 0;
    std::vector<unsigned int>::iterator find_unfit;
    for(unsigned int i = 0; i < this->AllSpecies.size() - 1; i++)
    {
        // Check if the species is unfit
        find_unfit = std::find(Unfit_Species.begin(), Unfit_Species.end(), i);
        if(find_unfit != Unfit_Species.end())
            continue;

        Num_Offsprings = std::round((this->AllSpecies.at(i).TotalAdjFitness() / 
                                     TotalFitness) * this->Pop_Num);
        std::vector<Genome> tmp = \
            this->AllSpecies.at(i).Reproduce(Num_Offsprings, _Kill_Percent, _Mut_Percent, _Params);
        
        Offsprings.insert(Offsprings.end(), tmp.begin(), tmp.end());
    }

    // For the last species
    find_unfit = std::find(Unfit_Species.begin(), Unfit_Species.end(), this->AllSpecies.size());
    if(find_unfit == Unfit_Species.end())
    {
        Num_Offsprings = this->Pop_Num - Offsprings.size();
        // Check with theoretical value
        unsigned int Theory = std::round((this->AllSpecies.at(this->AllSpecies.size()).TotalAdjFitness() / 
                                          TotalFitness) * this->Pop_Num);
        Theory = std::abs(static_cast<int>(Num_Offsprings - Theory));
        assert(Theory <= 2);

        std::vector<Genome> tmp = \
            this->AllSpecies.at(this->AllSpecies.size()).Reproduce(Num_Offsprings, _Kill_Percent, _Mut_Percent, _Params);
        
        Offsprings.insert(Offsprings.end(), tmp.begin(), tmp.end());
    }

    assert(Offsprings.size() == this->Pop_Num);

    // Replace the existing population with new offsprings
    for(unsigned int i = 0; i < this->Pop_Num; i++)
    {
        this->Population[i] = Offsprings.at(i);
    }

    // Clear each species for later use
    for(unsigned int i = 0; i < this->AllSpecies.size(); i++)
    {
        this->AllSpecies.at(i).ClearSpecies();
    }

    this->Gen_number += 1;
}

// Print out the information about this generation
void Generation::PrintGenInfo() const
{
    std::cout << "Generation: " << this->Gen_number << " _____________________________________________" << std::endl;
    std::cout << "Population: " << this->Pop_Num << std::endl;
    double total_fitness = 0;
    unsigned int total_member = 0;
    for(unsigned int i = 0; i < this->AllSpecies.size(); i++)
    {
        total_fitness += this->AllSpecies.at(i).TotalAdjFitness();
        total_member += this->AllSpecies.at(i).getSpeciesSize();
    }
    std::cout << "Ave Fitness: " << (total_fitness / total_member) << std::endl;
    std::cout << "Num Species: " << this->AllSpecies.size() << std::endl;
}
