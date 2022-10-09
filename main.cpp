#include "Genome.h"

int main()
{
    Genome gene(3, 1, Node::ActFunc::Linear);
    gene.PrintGenotype();

    gene.Mutate(0, 0, 0, 100, Node::ActFunc::None, 0);
    gene.Mutate(0, 0, 0, 100, Node::ActFunc::None, 0);
    gene.Mutate(0, 0, 0, 100, Node::ActFunc::None, 0);

    gene.Mutate(0, 0, 0, 0, Node::ActFunc::None, 100);
    gene.Mutate(0, 0, 0, 0, Node::ActFunc::None, 100);
    gene.Mutate(0, 0, 0, 0, Node::ActFunc::None, 100);
    gene.Mutate(0, 0, 0, 0, Node::ActFunc::None, 100);

    gene.Mutate(0, 0, 0, 100, Node::ActFunc::None, 0);
    gene.Mutate(0, 0, 0, 100, Node::ActFunc::None, 0);
    gene.Mutate(0, 0, 0, 100, Node::ActFunc::None, 0);
    gene.Mutate(0, 0, 0, 100, Node::ActFunc::None, 0);

    gene.Mutate(0, 0, 0, 0, Node::ActFunc::None, 100);
    gene.Mutate(0, 0, 0, 0, Node::ActFunc::None, 100);
    gene.Mutate(0, 0, 0, 0, Node::ActFunc::None, 100);
    gene.Mutate(0, 0, 0, 0, Node::ActFunc::None, 100);

    gene.Mutate(0, 0, 0, 100, Node::ActFunc::None, 0);
    gene.Mutate(0, 0, 0, 100, Node::ActFunc::None, 0);
    gene.Mutate(0, 0, 0, 100, Node::ActFunc::None, 0);
    gene.Mutate(0, 0, 0, 100, Node::ActFunc::None, 0);

    gene.Mutate(0, 0, 0, 0, Node::ActFunc::None, 100);
    gene.Mutate(0, 0, 0, 0, Node::ActFunc::None, 100);
    gene.Mutate(0, 0, 0, 0, Node::ActFunc::None, 100);
    gene.Mutate(0, 0, 0, 0, Node::ActFunc::None, 100);
    
    gene.PrintGenotype();

    return 0;
}