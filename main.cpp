#include "Genome.h"
#include <algorithm>

void print(Node& Element)
{
    std::cout << Element.ID << "\t" << Element.Type << std::endl;
}

int main()
{
    Genome gene(3, 1);
    std::list<Node> node = gene.getNodes();
    std::for_each(node.begin(), node.end(), print);
    return 0;
}