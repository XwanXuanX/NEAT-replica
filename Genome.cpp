#include "Genome.h"

// Initialize a new node with initial value 0
Node::Node(const unsigned short int _ID, const std::string _Type)
{
    this->ID    = _ID;
    this->Type  = _Type;
    this->Val   = 0;
}

// Initialize a new connection with random weight or 1.0
Connection::Connection(const unsigned short int _Innov, 
                       const unsigned short int _In, 
                       const unsigned short int _Out, 
                       const bool               _isRNG)
{
    this->Innov  = _Innov;
    this->In     = _In;
    this->Out    = _Out;
    this->Enable = true;

    if(_isRNG)
    {
        srand((unsigned)time(NULL));    // random seed used to generate random numbers
        this->Weight = static_cast<double>(rand() / RAND_MAX);
    }
    else
        this->Weight = 1.0;
}

Genome::Genome(const unsigned int _InputNodes, const unsigned int _OutputNodes, 
               const std::list<Node> _Nodes, const std::list<Connection> _Connections, 
               const bool _isOffspring)
{
    if(_isOffspring)
    {
        this->Nodes       = _Nodes;
        this->Connections = _Connections;
    }
    else
    {
        unsigned int node_added = 0;

        // Add input and output nodes to the Node Gene list
        for(; node_added < (_InputNodes + _OutputNodes); node_added++)
        {
            Node new_node(node_added + 1, (node_added < _InputNodes) ? "input" : "output");
            this->Nodes.push_back(new_node);
        }

        // Add connections to the Connection Gene list
        for(unsigned int input = 1; input <= _InputNodes; input++)
        {
            for(unsigned int output = _InputNodes + 1; output <= (_InputNodes + _OutputNodes); output++)
            {
                Connection new_connect(0, input, output, true);
                this->Connections.push_back(new_connect);
            }
        }
    }
}