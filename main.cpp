#include "Genome.h"

int main()
{
    Genome gene(3, 1);
    gene.PrintGenotype();

    gene.Mutate(0, 0, 0, 100, 0);
    gene.Mutate(0, 0, 0, 100, 0);
    gene.Mutate(0, 0, 0, 100, 0);

    gene.Mutate(0, 0, 0, 0, 100);
    gene.Mutate(0, 0, 0, 0, 100);
    gene.Mutate(0, 0, 0, 0, 100);
    gene.Mutate(0, 0, 0, 0, 100);

    gene.Mutate(0, 0, 0, 100, 0);
    gene.Mutate(0, 0, 0, 100, 0);
    gene.Mutate(0, 0, 0, 100, 0);
    gene.Mutate(0, 0, 0, 100, 0);

    gene.Mutate(0, 0, 0, 0, 100);
    gene.Mutate(0, 0, 0, 0, 100);
    gene.Mutate(0, 0, 0, 0, 100);
    gene.Mutate(0, 0, 0, 0, 100);

    gene.Mutate(0, 0, 0, 100, 0);
    gene.Mutate(0, 0, 0, 100, 0);
    gene.Mutate(0, 0, 0, 100, 0);

    gene.Mutate(0, 0, 0, 0, 100);
    gene.Mutate(0, 0, 0, 0, 100);
    gene.Mutate(0, 0, 0, 0, 100);
    
    gene.PrintGenotype();

    return 0;
}