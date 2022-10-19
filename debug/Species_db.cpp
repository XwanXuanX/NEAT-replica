/**
 * @file Species_db.cpp
 * @brief Code used to debug Species.cpp file
 * @author Yetong (Tony) Li
 * @date Oct 18, 2022
 * @version 1.0.1
*/

#include "debug.h"

#if (defined DEBUG) && (defined SPECIES)
#include "Species.h"
#define TEST_POP 10000
unsigned int node_tracker = 6;

int main()
{
    srand((unsigned int)time(NULL));

#ifdef EXCESS
    Genome test_pop[TEST_POP];
    for(unsigned int i = 0; i < TEST_POP; i++)
    {
        Genome gene(3, 2, Node::ActFunc::Modified_Sigmoid);
        gene.Mutate(0, 80, 10, 50, Node::ActFunc::Modified_Sigmoid, 70);
        gene.setFitness(rand() % 3);
        test_pop[i] = gene;
    }

    Species spe(test_pop[0]);
	CompatDistParams ComParam(1.0, 1.0, 0.4, 0);
    MutateParams param(0, 50, 50, 50, Node::ActFunc::Modified_Sigmoid, 50);
    for(unsigned int i = 1; i < TEST_POP; i++)
    {
        spe.AddOrganism(test_pop[i], 100, ComParam);
    }
    spe.PrintSpeciesInfo();

    double totalfit = spe.TotalAdjFitness();
    std::cout << totalfit << std::endl;
    spe.CalcAdjFitness();
    totalfit = spe.TotalAdjFitness();
    std::cout << totalfit << std::endl;

    std::vector<Genome> offsp;
    offsp = spe.Reproduce(TEST_POP, 0.90, 0.01, param);
    assert(offsp.size() == TEST_POP);
    std::cout << "offsp == 10000 finished" << std::endl;

    std::cout << spe.CheckMaxFitGen(1) << std::endl;
    std::cout << spe.getMaxFit() << " " << spe.getMaxFitGen() << std::endl;
    spe.ClearSpecies();
    assert(spe.getSpeciesSize() == 0);

    for(unsigned int i = 0; i < TEST_POP; i++)
    {
        offsp[i].setFitness(0);
        spe.AddOrganism(offsp[i], 100, ComParam);
    }
    spe.PrintSpeciesInfo();
    std::cout << spe.CheckMaxFitGen(1) << std::endl;
    std::cout << spe.getMaxFit() << " " << spe.getMaxFitGen() << std::endl;
    spe.ClearSpecies();
    assert(spe.getSpeciesSize() == 0);
#endif
#ifdef EMPTY
    MutateParams param(0, 50, 50, 50, Node::ActFunc::Modified_Sigmoid, 50);

    Genome gene(3, 2, Node::ActFunc::Modified_Sigmoid);
    gene.setFitness(2);
    Species spe(gene);
    assert(spe.getSpeciesSize() == 1);
    spe.PrintSpeciesInfo();

    spe.CalcAdjFitness();
    assert(spe.TotalAdjFitness() == 2);
    std::vector<Genome> Offsp;
    Offsp = spe.Reproduce(TEST_POP, 0.5, 0.01, param);
    assert(Offsp.size() == TEST_POP);

    std::cout << spe.CheckMaxFitGen(1) << std::endl;
    std::cout << spe.getMaxFit() << " " << spe.getMaxFitGen() << std::endl;
    spe.ClearSpecies();
    assert(spe.getSpeciesSize() == 0);

    spe.PrintSpeciesInfo();
    spe.CalcAdjFitness();
    assert(spe.TotalAdjFitness() == 0);
    Offsp = spe.Reproduce(TEST_POP, 0.5, 0.01, param);
    assert(Offsp.size() == 0);

    std::cout << spe.CheckMaxFitGen(1) << std::endl;
    std::cout << spe.getMaxFit() << " " << spe.getMaxFitGen() << std::endl;
    spe.ClearSpecies();
    assert(spe.getSpeciesSize() == 0);
#endif

    return 0;
}
#endif