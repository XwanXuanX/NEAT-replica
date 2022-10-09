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

    // Mutate gene1 randomly
    gene1.Mutate(0, 0, 0, 100, Node::ActFunc::Tanh, 100);
    gene1.Mutate(0, 0, 0, 100, Node::ActFunc::Tanh, 100);
    gene1.Mutate(0, 0, 0, 100, Node::ActFunc::Tanh, 100);
    gene1.Mutate(0, 0, 0, 100, Node::ActFunc::Tanh, 100);
    gene1.Mutate(0, 0, 0, 100, Node::ActFunc::Tanh, 100);
    gene1.Mutate(0, 0, 0, 100, Node::ActFunc::Tanh, 100);
    gene1.Mutate(0, 0, 0, 100, Node::ActFunc::Tanh, 100);
    gene1.Mutate(0, 0, 0, 100, Node::ActFunc::Tanh, 100);

    // Mutate gene2 randomly
    gene2.Mutate(0, 0, 0, 100, Node::ActFunc::Tanh, 100);
    gene2.Mutate(0, 0, 0, 100, Node::ActFunc::Tanh, 100);
    gene2.Mutate(0, 0, 0, 100, Node::ActFunc::Tanh, 100);
    gene2.Mutate(0, 0, 0, 100, Node::ActFunc::Tanh, 100);
    gene2.Mutate(0, 0, 0, 100, Node::ActFunc::Tanh, 100);
    gene2.Mutate(0, 0, 0, 100, Node::ActFunc::Tanh, 100);
    gene2.Mutate(0, 0, 0, 100, Node::ActFunc::Tanh, 100);
    gene2.Mutate(0, 0, 0, 100, Node::ActFunc::Tanh, 100);

    gene1.PrintGenotype();
    std::cout << std::endl;
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