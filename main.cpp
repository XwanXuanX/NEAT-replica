#include "Genome.h"

int main()
{
    // random seed used to generate random numbers
    srand((unsigned int)time(nullptr));

    Genome gene1(3, 1, Node::ActFunc::Linear);
    gene1.PrintGenotype();
    Genome gene2(3, 1, Node::ActFunc::Linear);
    gene2.PrintGenotype();

    std::cout << std::endl;

    for(unsigned int i = 0; i < 3; i++)
    {
        // Mutate gene1 randomly
        gene1.Mutate(0, 50, 50, 100, Node::ActFunc::Linear, 100);
        // Mutate gene2 randomly
        gene2.Mutate(0, 50, 50, 100, Node::ActFunc::Linear, 100);
    }

    gene1.PrintGenotype();
    gene2.PrintGenotype();

    double input1[] = {1, 2, 3};
    std::vector<double> out = gene1.Propagate(input1, 3);
    for(unsigned int i = 0; i < out.size(); i++)
        std::cout << out[i] << " ";
    std::cout << std::endl;

    out = gene2.Propagate(input1, 3);
    //for(unsigned int i = 0; i < out.size(); i++)
    //   std::cout << out[i] << " ";
    //std::cout << std::endl;

    return 0;
}