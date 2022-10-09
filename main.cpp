#include "Genome.h"

int main()
{
    Genome gene(3, 1, Node::ActFunc::Linear);
    gene.PrintGenotype();

    double input[] = {1, 2, 3};

    std::vector<double> out = gene.Propagate(input, 3);

    for(unsigned int i = 0; i < out.size(); i++)
        std::cout << out[i] << " ";
    std::cout << std::endl;

    return 0;
}