/**
 * @file Species.cpp
 * @brief Function bodies of function declarations in Species.h
 * @author Yetong (Tony) Li
 * @date Oct 13, 2022
 * @version 1.0.1
*/

#include "Species.h"

/*_______________________________________________________________________________________________________*/
/*                                      CompatDistParams Struct                                          */

/**
 * @brief Constructor to initialize CompatDistParams struct
 * @param[in] _c1 Formula param c1
 * @param[in] _c2 Formula param c2
 * @param[in] _c3 Formula param c3
 * @param[in] _NormalThreshold Should enable formula param N
*/
CompatDistParams::CompatDistParams(const double _c1, const double _c2, const double _c3,
                                   const unsigned int _NormalThreshold)
{
    this->c1 = _c1;
    this->c2 = _c2;
    this->c3 = _c3;
    this->NormalThreshold = _NormalThreshold;
}

/*_______________________________________________________________________________________________________*/
/*                                      MutateParams Struct                                              */

/**
 * @brief Constructor to initialize MutateParams struct
 * @param[in] _ToggleConnect_Percent Probability to toggle a connection
 * @param[in] _MutateWeight_Percent Probability to change a weight
 * @param[in] _RNGPercent Probability to change a weight RANDOMLY
 * @param[in] _AddNode_Percent Probability to add a node
 * @param[in] _HiddenMode The type of activation function hidden nodes should use
 * @param[in] _AddConnection_Percent Probability to add a connection
*/
MutateParams::MutateParams(const unsigned int _ToggleConnect_Percent, 
                           const unsigned int _MutateWeight_Percent, const unsigned int _RNGPercent, 
                           const unsigned int _AddNode_Percent, const Node::ActFunc _HiddenMode, 
                           const unsigned int _AddConnection_Percent)
{
    this->ToggleConnect_Percent = _ToggleConnect_Percent;
    this->MutateWeight_Percent  = _MutateWeight_Percent;
    this->RNGPercent            = _RNGPercent;
    this->AddNode_Percent       = _AddNode_Percent;
    this->HiddenMode            = _HiddenMode;
    this->AddConnection_Percent = _AddConnection_Percent;
}

/*_______________________________________________________________________________________________________*/
/*                                          Species Class                                                */

/**
 * @brief Initialize a new species with a genome
 * @param[in] _NewGenome The first organism to be added to this species
*/
Species::Species(const Genome _NewGenome)
{
    // When a new species is created, the first genome will be the represent
    this->Represent = _NewGenome;
    this->Organisms.push_back(_NewGenome);
    this->MaxFit = 0;
    this->MaxFitGen = 0;
}

/**
 * @brief Check if a genome should be added to this species
 * @param[in] _NewGenome New organism to be added to this species
 * @param[in] _CompatThreshold Compatibility threshold to determine if it should be added
 * @param[in] _Params Parameters used to calculate the compatibility distance formula
 * @return If new organism is added, return true; else, return false
*/
bool Species::AddOrganism(const Genome _NewGenome, const double _CompatThreshold, const CompatDistParams _Params)
{
    double compat_dist = this->Represent.CompatDistance(_NewGenome, _Params.c1, _Params.c2, _Params.c3, _Params.NormalThreshold);
    // If the new genome is not compatable with the species, return false
    if(compat_dist > _CompatThreshold)
        return false;
    else
    {
        this->Organisms.push_back(_NewGenome);
        return true;
    }
}

/**
 * @brief Calculate the adjusted fitness for every organisms in the species
*/
void Species::CalcAdjFitness()
{
    for(unsigned int i = 0; i < this->Organisms.size(); i++)
    {
        this->Organisms[i].CalcAdjFitness(this->Organisms.size());
    }
}

/**
 * @brief Clear species for later generations
*/
void Species::ClearSpecies()
{
    // If nothing inside the species
    if(this->Organisms.size() == 0)
        return;

    // Randomly select a genome as the represent of the species
    unsigned int selected = rand() % this->Organisms.size();
    this->Represent = this->Organisms.at(selected);

    // Empty the list of organisms for later use
    this->Organisms.clear();
}

/**
 * @brief Crossover to produce certain number of offsprings
 * @param[in] _Num_Offsprings The assigned number of offpsrings that a species can produce
 * @param[in] _Kill_Percent Decides how many members should be killed
 * @param[in] _Mut_Percent Decides how many offsprings should be produced through mutation
 * @param[in] _Params Parameters used to mutate an organism
 * @return A list (vector) of offsprings
*/
std::vector<Genome> Species::Reproduce(const unsigned int _Num_Offsprings, const double _Kill_Percent, 
                                       const double _Mut_Percent, const MutateParams _Params)
{
    // Some rules for reproduction
    //      * After killing the lowest performing members, each species must have at least one member
    //      * A species CANNOT go extinct
    //      * If a species does not contain any members after speciation, it will not reproduce
    //      * If there is only one organism left after killing, it will breed with itself
    //      * If there are more than one organisms left, the species will reproduce based on order

    // list used to hold reproduced offsprings
    std::vector<Genome> Offsprings;

    // If a species does not contain any members after speciation, or its not allowed to produce, it will not reproduce
    if(this->Organisms.size() == 0 || _Num_Offsprings == 0)
        return Offsprings;

    // The organisms should be sorted in decending order based on their adjusted fitness
    std::sort(this->Organisms.begin(), this->Organisms.end(), 
              [&](Genome L, Genome R){
                return (L > R);
              });

#if (defined DEBUG) && (defined REPRODUCE)
    auto InspectMembers = [] (std::vector<Genome> Members) -> void
    {
        for(unsigned int i = 0; i < Members.size(); i++)
            std::cout << Members.at(i).getFitness() << " ";
        std::cout << std::endl;
    };
    InspectMembers(this->Organisms);
#endif

    // According to original paper, the champion of each species with more than five networks,
    // Was copied into the next generation unchanged!
    if(this->Organisms.size() > 5)
    {
        // Append champion into the next generation
        this->Organisms.at(0).setFitness(0);
        Offsprings.push_back(this->Organisms.at(0));
#if (defined DEBUG) && (defined REPRODUCE)
        std::cout << Offsprings.size() << ": Champion with fitness " 
                  << this->Organisms.at(0).getFitness() << " was added to offspring" << std::endl;
#endif
        if(Offsprings.size() >= _Num_Offsprings)
            return Offsprings;
    }

    // Kill the lowest performing members
    double _Keep_Number = this->Organisms.size() * (1.0 - _Kill_Percent);
    _Keep_Number = (_Keep_Number <= 1.0) ? 1 : std::round(_Keep_Number);

    // Start killing members from back to front
    while(this->Organisms.size() > _Keep_Number)
    {
        std::vector<Genome>::const_iterator kill_genome = this->Organisms.end();
        kill_genome--;
        this->Organisms.erase(kill_genome);
    }

#if (defined DEBUG) && (defined REPRODUCE)
    InspectMembers(this->Organisms);
#endif

    // Select the best organisms to crossover
    //      * Some resulted from sexual reproduction: Crossover
    //      * Some resulted from asexual reproduction: Mutation
    unsigned int by_crossover = 0;
    unsigned int by_mutation = std::floor(_Num_Offsprings * _Mut_Percent); 
    assert(by_mutation >= 0);
    if(Offsprings.size() == 0)
        by_crossover = _Num_Offsprings - by_mutation;
    else
        by_crossover = (_Num_Offsprings - 1) - by_mutation;

    if(by_crossover > 0)
    {
        unsigned int added = 0;
        while(true)
        {
            // If the species contains only one member, self crossover is allowed
            if(this->Organisms.size() == 1)
            {
                Genome Self = this->Organisms.at(0);
                Genome new_genome = Self.Crossover(Self, Self.getFitness(), Self.getFitness());
                Offsprings.push_back(new_genome);
                added++;
#if (defined DEBUG) && (defined REPRODUCE)
                std::cout << Offsprings.size() << ": New genome created by self-crossover" << std::endl;
#endif
                // Check if enough offsprings are bread
                if(added >= by_crossover)
                    break;
                continue;
            }
            else
            {
                bool should_exit = false;
                for(unsigned int i = 0; i < this->Organisms.size(); i++)
                {
                    for(unsigned int j = i + 1; j < this->Organisms.size(); j++)
                    {
                        Genome new_genome = this->Organisms.at(i).Crossover(this->Organisms.at(j),
                            this->Organisms.at(i).getFitness(), this->Organisms.at(j).getFitness());
                        Offsprings.push_back(new_genome);
                        added++;
#if (defined DEBUG) && (defined REPRODUCE)
                        std::cout << Offsprings.size() << ": New genome created by crossing-over "
                                << i + 1 << " and " << j + 1 << std::endl;
#endif
                        if(added >= by_crossover)
                        {
                            should_exit = true;
                            break;
                        }
                    }
                    if(should_exit)
                        break;
                }
                if(should_exit)
                    break;
            }
        }
    }

    if(by_mutation > 0)
    {
        unsigned int added = 0;
        while(true)
        {
            for(unsigned int i = 0; i < this->Organisms.size(); i++)
            {
                this->Organisms.at(i).Mutate(_Params.ToggleConnect_Percent,
                    _Params.MutateWeight_Percent, _Params.RNGPercent, _Params.AddNode_Percent,
                    _Params.HiddenMode, _Params.AddConnection_Percent);
                this->Organisms.at(i).setFitness(0);
                Offsprings.push_back(this->Organisms.at(i));
                added++;
#if (defined DEBUG) && (defined REPRODUCE)
                std::cout << Offsprings.size() << ": New genome created by mutating "
                          << i + 1 << std::endl;
#endif
                if(added >= by_mutation)
                {
                    assert(Offsprings.size() == _Num_Offsprings);
                    return Offsprings;
                }
            }
        }
    }

    assert(Offsprings.size() == _Num_Offsprings);
    return Offsprings;
}

/**
 * @brief Get the total adjusted fitness of the species
 * @return The total adjusted fitness
*/
double Species::TotalAdjFitness() const
{
    double total_fitness = 0;
    for(unsigned int i = 0; i < this->Organisms.size(); i++)
    {
        total_fitness += this->Organisms.at(i).getFitness();
    }
    return total_fitness;
}

/**
 * @brief Print basic information about the species
*/
void Species::PrintSpeciesInfo() const
{
#if (defined DEBUG)
    std::cout << "Representation Genome: " << std::endl;
    this->Represent.PrintGenotype();
#endif
    std::cout << "Number of organisms: " << this->Organisms.size() << std::endl;
    if(this->Organisms.size() == 0)
        std::cout << "Average adjusted fitness: " << 0 << std::endl;
    else
    {
        std::cout << "Average adjusted fitness: " 
                  << (this->TotalAdjFitness() / this->Organisms.size())
                  << std::endl;
    }
}

/**
 * @brief Update the Maximum fitness and its generation number
 * @param[in] _Threshold_Gen If the maximum fitness of a species does not increase
 *                           for [Threshold generation] generations, it is not allowed to reproduce
 * @return If current generation exceeds the threshold, return false; else, return true
*/
bool Species::CheckMaxFitGen(const unsigned int _Threshold_Gen)
{
    double max_fitness = 0;
    if(this->Organisms.size() != 0)
    {
        // The maximum fitness of current generation
        max_fitness = this->Organisms.at(0).getFitness();
        for(unsigned int i = 0; i < this->Organisms.size(); i++)
        {
            if(this->Organisms.at(i).getFitness() > max_fitness)
                max_fitness = this->Organisms.at(i).getFitness();
        }
    }

    if(max_fitness <= this->MaxFit)
        this->MaxFitGen += 1;
    else
    {
        this->MaxFit = max_fitness;
        this->MaxFitGen = 0;
    }
    
    return (!(MaxFitGen >= _Threshold_Gen));
}

Genome Species::getRepresent() const
{
    return this->Represent;
}

std::vector<Genome> Species::getOrganisms() const
{
    return this->Organisms;
}

double Species::getMaxFit() const
{
    return this->MaxFit;
}

unsigned int Species::getMaxFitGen() const
{
    return this->MaxFitGen;
}

unsigned int Species::getSpeciesSize() const
{
    return this->Organisms.size();
}
