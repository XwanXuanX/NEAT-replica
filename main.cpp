#include "Genome.h"

int main()
{
    // random seed used to generate random numbers
    srand((unsigned int)time(nullptr));

    Genome gene1(3, 1, Node::ActFunc::Linear);
    Genome gene2(3, 1, Node::ActFunc::Linear);

    for(unsigned int i = 0; i < 2; i++)
    {
        // Mutate gene1 randomly
        gene1.Mutate(0, 50, 50, 100, Node::ActFunc::Linear, 100);
        // Mutate gene2 randomly
        gene2.Mutate(0, 50, 50, 100, Node::ActFunc::Linear, 100);
    }
    gene1.PrintGenotype();
    std::cout << std::endl;
    gene2.PrintGenotype();

    gene1.Crossover(gene2, 2, 2);

    /*
    Genome gene3(3, 1, Node::ActFunc::Linear);


    
    double input1[] = {1, 2, 3};
    std::vector<double> out = gene1.Propagate(input1, 3);
    gene1.PrintGenotype();
    std::cout << out[0] << std::endl;

    std::cout << std::endl;
    std::cout << std::endl;

    out = gene2.Propagate(input1, 3);
    gene2.PrintGenotype();
    std::cout << out[0] << std::endl;

    std::cout << std::endl;
    std::cout << std::endl;

    out = gene3.Propagate(input1, 3);
    gene3.PrintGenotype();
    std::cout << out[0] << std::endl;
    */
    return 0;
}