#include "Species.h"

CompatDistParams::CompatDistParams(const double _c1, const double _c2, const double _c3,
                                   const unsigned int _NormalThreshold)
{
    this->c1 = _c1;
    this->c2 = _c2;
    this->c3 = _c3;
    this->NormalThreshold = _NormalThreshold;
}

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

// Constructor: Initialize a new species with a genome
Species::Species(const Genome _NewGenome)
{
    // When a new species is created, the first genome will be the represent
    this->Represent = _NewGenome;
    this->Organisms.push_back(_NewGenome);
    this->MaxFit = 0;
    this->MaxFitGen = 0;
}

// Check if a genome should be added to this species; if yes, add and return true
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

// Calculate the adjusted fitness for every organisms in the species
void Species::CalcAdjFitness()
{
    for(unsigned int i = 0; i < this->Organisms.size(); i++)
    {
        this->Organisms[i].CalcAdjFitness(this->Organisms.size());
    }
}

// Clear species for later generations
void Species::ClearSpecies()
{
    // Randomly select a genome as the represent of the species
    unsigned int selected = rand() % this->Organisms.size();
    this->Represent = this->Organisms.at(selected);

    // Empty the list of organisms for later use
    this->Organisms.clear();
}

// Crossover to produce certain number of offsprings
// Some rules for reproduction
//      * After killing the lowest performing members, each species must have at least one member
//      * A species CANNOT go extinct
//      * If a species does not contain any members after speciation, it will not reproduce
//      * If there is only one organism left after killing, it will breed with itself
//      * If there are more than one organisms left, the species will reproduce based on order
std::vector<Genome> Species::Reproduce(const unsigned int _Num_Offsprings, const double _Kill_Percent, 
                                       const double _Mut_Percent, const MutateParams _Params)
{
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

// Get the total adjusted fitness of the species
double Species::TotalAdjFitness() const
{
    double total_fitness = 0;
    for(unsigned int i = 0; i < this->Organisms.size(); i++)
    {
        total_fitness += this->Organisms.at(i).getFitness();
    }
    return total_fitness;
}

// Print basic information about the species
void Species::PrintSpeciesInfo() const
{
#if (defined DEBUG)
    std::cout << "Representation Genome: " << std::endl;
    this->Represent.PrintGenotype();
#endif
    std::cout << "Number of organisms: " << this->Organisms.size() << std::endl;
    std::cout << "Average adjusted fitness: " 
              << (this->TotalAdjFitness() / this->Organisms.size())
              << std::endl;
}

// Update the Maximum fitness and its generation number
bool Species::CheckMaxFitGen(const unsigned int _Threshold_Gen)
{
    // The maximum fitness of current generation
    double max_fitness = this->Organisms.at(0).getFitness();
    for(unsigned int i = 0; i < this->Organisms.size(); i++)
    {
        if(this->Organisms.at(i).getFitness() > max_fitness)
            max_fitness = this->Organisms.at(i).getFitness();
    }

    if(max_fitness <= this->MaxFit)
        this->MaxFitGen += 1;
    else
    {
        this->MaxFit = max_fitness;
        this->MaxFitGen = 0;
    }

    if(MaxFitGen >= _Threshold_Gen)
        return false;
    else 
        return true;
}

// Getters (if need any)
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

// Setters (if need any)
void Species::setMaxFit(const double _NewMaxFit)
{
    this->MaxFit = _NewMaxFit;
}

void Species::setMaxFitGen(const unsigned int _NewMaxFitGen)
{
    this->MaxFitGen = _NewMaxFitGen;
}