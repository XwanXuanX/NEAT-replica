/**
 * @file Genome_db.cpp
 * @brief Code used to debug Genome.cpp file
 * @author Yetong (Tony) Li
 * @date Oct 18, 2022
 * @version 1.0.1
*/

#include "debug.h"

#if (defined DEBUG) && (defined GENOME)
#include "Genome.h"
#define TEST_POP 10000
unsigned int node_tracker = 6;

int main()
{
    Genome test_genes[TEST_POP];
    for(unsigned int i = 0; i < TEST_POP; i++)
    {
        Genome gene(3, 2, Node::ActFunc::Modified_Sigmoid);
        test_genes[i] = gene;
    }

    for(unsigned int i = 0; i < TEST_POP; i++)
    {
        test_genes[i].Mutate(0, 80, 10, 50, Node::ActFunc::Modified_Sigmoid, 70);
        std::list<Node> node = test_genes[i].getNodes();
        std::list<Connection> connect = test_genes[i].getConnections();

        for(auto iter = node.begin(); iter != node.end(); iter++)
        {
            for(auto tester = node.begin(); tester != node.end(); tester++)
            {
                if(tester == iter)
                    continue;
                assert(tester->ID != iter->ID);
            }
        }

        for(auto iter = connect.begin(); iter != connect.end(); iter++)
        {
            for(auto tester = connect.begin(); tester != connect.end(); tester++)
            {
                if(tester == iter)
                    continue;
                assert(tester->Innov != iter->Innov);
                assert(tester->In != iter->In || tester->Out != iter->Out);
            }
        }
    }

    double Input[]= {1, 2, 3};
    for(unsigned int i = 0; i < TEST_POP; i++)
    {
        std::vector<double> output;
        output = test_genes[i].Propagate(Input, 3);
        assert(output.size() == 2);
        assert(output[0] != 0 && output[1] != 0);
    }

    for(unsigned int i = 0; i < TEST_POP - 1; i++)
    {
        double compat = 0;
        compat = test_genes[i].CompatDistance(test_genes[i+1], 1, 1, 1, 0);
        assert(compat != 0);
    }

    srand((unsigned int)time(NULL));

    Genome Offsp[TEST_POP];
    for(unsigned int i = 0; i < TEST_POP; i++)
    {
        Genome parent1 = test_genes[rand() % TEST_POP];
        Genome parent2 = test_genes[rand() % TEST_POP];
        Offsp[i] = parent1.Crossover(parent2, rand() % 2, rand() % 2);
    }

    // 2nd Generation
    for(unsigned int i = 0; i < TEST_POP; i++)
    {
        Offsp[i].Mutate(0, 80, 10, 50, Node::ActFunc::Modified_Sigmoid, 70);
        std::list<Node> node = Offsp[i].getNodes();
        std::list<Connection> connect = Offsp[i].getConnections();

        for(auto iter = node.begin(); iter != node.end(); iter++)
        {
            for(auto tester = node.begin(); tester != node.end(); tester++)
            {
                if(tester == iter)
                    continue;
                assert(tester->ID != iter->ID);
            }
        }

        for(auto iter = connect.begin(); iter != connect.end(); iter++)
        {
            for(auto tester = connect.begin(); tester != connect.end(); tester++)
            {
                if(tester == iter)
                    continue;
                assert(tester->Innov != iter->Innov);
                assert(tester->In != iter->In || tester->Out != iter->Out);
            }
        }
    }

    for(unsigned int i = 0; i < TEST_POP; i++)
    {
        std::vector<double> output;
        output = Offsp[i].Propagate(Input, 3);
        assert(output.size() == 2);
        assert(output[0] != 0 && output[1] != 0);
    }

    for(unsigned int i = 0; i < TEST_POP - 1; i++)
    {
        double compat = 0;
        compat = Offsp[i].CompatDistance(Offsp[i+1], 1, 1, 1, 0);
        assert(compat != 0);
    }

    return 0;
}
#endif