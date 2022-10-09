#include "Genome.h"

int main()
{
    // random seed used to generate random numbers
    srand((unsigned int)time(nullptr));

    Genome gene1(3, 1, Node::ActFunc::Linear);
    gene1.PrintGenotype();

    Genome gene2(3, 1, Node::ActFunc::Linear);
    gene2.PrintGenotype();

    /*
    double input[] = {1, 2, 3};

    std::vector<double> out = gene.Propagate(input, 3);

    for(unsigned int i = 0; i < out.size(); i++)
        std::cout << out[i] << " ";
    std::cout << std::endl;
    */

    return 0;
}