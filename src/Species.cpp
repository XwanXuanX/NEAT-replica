#include "Species.h"

CompatDistParams::CompatDistParams(const double _c1, const double _c2, const double _c3,
                                   const unsigned int _NormalThreshold)
{
    this->c1 = _c1;
    this->c2 = _c2;
    this->c3 = _c3;
    this->NormalThreshold = _NormalThreshold;
}

// Constructor: Initialize a new species with a genome
Species::Species(const Genome _NewGenome)
{
    // When a new species is created, the first genome will be the represent
    this->Represent = _NewGenome;
    this->Organisms.push_back(_NewGenome);
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
        Organisms[i].CalcAdjFitness(this->Organisms.size());
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
std::vector<Genome> Species::Reproduce(const unsigned int _Num_Offsprings, const double _Kill_Percent)
{
    // list used to hold reproduced offsprings
    std::vector<Genome> Offsprings;

    // If a species does not contain any members after speciation, it will not reproduce
    if(this->Organisms.size() == 0)
        return Offsprings;

    // Kill the lowest performing members
    double _Keep_Number = this->Organisms.size() * (1.0 - _Kill_Percent);
    _Keep_Number = (_Keep_Number <= 1.0) ? 1 : std::round(_Keep_Number);

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
    while(true)
    {
        // If the species contains only one member, self crossover is allowed
        if(this->Organisms.size() == 1)
        {
            Genome Self = this->Organisms.at(0);
            Genome new_genome = Self.Crossover(Self, Self.getFitness(), Self.getFitness());
            Offsprings.push_back(new_genome);
#if (defined DEBUG) && (defined REPRODUCE)
            std::cout << Offsprings.size() << ": New genome created by self-crossover" << std::endl;
#endif
            // Check if enough offsprings are bread
            if(Offsprings.size() >= _Num_Offsprings)
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
                                                           this->Organisms.at(i).getFitness(),
                                                           this->Organisms.at(j).getFitness());
                    Offsprings.push_back(new_genome);
#if (defined DEBUG) && (defined REPRODUCE)
                    std::cout << Offsprings.size() << ": New genome created by crossing-over "
                              << i + 1 << " and " << j + 1 << std::endl;
#endif
                    if(Offsprings.size() >= _Num_Offsprings)
                    {
                        should_exit = true;
                        break;
                    }
                    continue;
                }
                if(should_exit) break;
            }
            if(should_exit) break;
        }
    }

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

// Getters (if need any)
Genome Species::getRepresent() const
{
    return this->Represent;
}

std::vector<Genome> Species::getOrganisms() const
{
    return this->Organisms;
}