/**
 * @file Generation_db.cpp
 * @brief Code used to debug Generation.cpp file
 * @author Yetong (Tony) Li
 * @date Oct 19, 2022
 * @version 1.0.1
*/

#include "debug.h"

#if (defined DEBUG) && (defined GENERATION)
#include "Generation.h"
#define TEST_POP 10000
unsigned int node_tracker = 6;

double Test(const Genome* gene)
{
    double input[3] = {1, 2, 3};
    gene->Propagate(input, 3);
    return 2;
}

int main()
{
    srand((unsigned int)time(NULL));

    GenomeLayout layout(3, 2, Node::ActFunc::Modified_Sigmoid);
    MutateParams param(0, 50, 50, 50, Node::ActFunc::Modified_Sigmoid, 50);
    CompatDistParams dist(1, 1, 1, 0);
    Generation gen(layout, TEST_POP);

    gen.Mutate(param);
    gen.Evaluate(&Test);
    gen.Speciate(2, dist);
    gen.PrintGenInfo();

    gen.Reproduce(0.5, 1, 0.01, param);
    gen.Mutate(param);
    gen.Evaluate(&Test);
    gen.Speciate(3, dist);
    gen.PrintGenInfo();

    return 0;
}
#endif