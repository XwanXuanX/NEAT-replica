#include "Genome.h"

/////////////////////////////////////////////////////////////////
// IMPORTANT: Innovation number (global throughout the project)//
unsigned short int INNOV = 0;                                  //
/////////////////////////////////////////////////////////////////

// Initialize a new node with initial value 0
Node::Node(const unsigned short int _ID, const std::string _Type)
{
    this->ID    = _ID;
    this->Type  = _Type;
    this->Val   = 0;
}

// Initialize connection with random weights
Connection::Connection(const unsigned short int _Innov, const unsigned short int _In, const unsigned short int _Out)
{
    this->Innov  = _Innov;
    this->In     = _In;
    this->Out    = _Out;
    this->Enable = true;

    // Generate a decimal number between -2 and 2
    this->Weight = (double)rand() / ((double)RAND_MAX / 4) - 2.0;
}

// Initialize connection with an assigned weight
Connection::Connection(const unsigned short int _Innov, const unsigned short int _In, const unsigned short int _Out, 
                       double                   _Weight)
{
    this->Innov  = _Innov;
    this->In     = _In;
    this->Out    = _Out;
    this->Enable = true;
    this->Weight = _Weight;
}

// Mutate weight: when true, randomly assign new weight; otherwise, nudge the weight slightly
void Connection::MUTWeight(const bool _isRNG=true)
{
    if(_isRNG)
        // assign completely random number between -2 and 2
        this->Weight = (double)rand() / ((double)RAND_MAX / 4) - 2.0;
    else
        // nudge the weight slightly by multiplying by random number between 0 and 2
        this->Weight *= (double)rand() / ((double)RAND_MAX / 2);
}

// Mutate enable: toggle the enabled state of the connection
void Connection::MUTEnable()
{
    this->Enable = !(this->Enable);
}

// Initialize a Genome with NO hidden units (minimal structure)
// Initialize 1st generation
Genome::Genome(const unsigned int _InputNodes, const unsigned int _OutputNodes)
{
    unsigned int node_added = 0;
    // Add input and output nodes to the Node Gene list
    for(; node_added < (_InputNodes + _OutputNodes); node_added++)
    {
        Node new_node(node_added + 1, (node_added < _InputNodes) ? "input" : "output");
        this->Nodes.push_back(new_node);
    }

    srand((unsigned int)time(nullptr));    // random seed used to generate random numbers

    // Add connections to the Connection Gene list
    for(unsigned int input = 1; input <= _InputNodes; input++)
    {
        for(unsigned int output = _InputNodes + 1; output <= (_InputNodes + _OutputNodes); output++)
        {
            Connection new_connect(0, input, output);
            this->Connections.push_back(new_connect);
        }
    }
}

// Initialize offsprings
Genome::Genome(const std::list<Node> _Nodes, const std::list<Connection> _Connections)
{
    this->Nodes       = _Nodes;
    this->Connections = _Connections;
}

// Enable/Disable each connection randomly by a percent
void Genome::ToggleConnect(const unsigned int _Percent)
{
    if(_Percent > 100 || _Percent < 0)
    {
        std::cout << "Percentage not in range [0, 100]" << std::endl;
        throw std::domain_error("BAD VALUE PASSED!");
    }

    std::list<Connection>::iterator iter = this->Connections.begin();
    for(; iter != this->Connections.end(); iter++)
    {
        if(1 + (rand() % 100) < _Percent)
            iter->MUTEnable();
    }
}

// Assign/Nudge each weight randomly by a percent
void Genome::MutateWeight(const unsigned int _RNGPercent)
{
    if(_RNGPercent > 100 || _RNGPercent < 0)
    {
        std::cout << "Percentage not in range [0, 100]" << std::endl;
        throw std::domain_error("BAD VALUE PASSED!");
    }

    std::list<Connection>::iterator iter = this->Connections.begin();
    for(; iter != this->Connections.end(); iter++)
    {
        if(1 + (rand() % 100) < _RNGPercent)    iter->MUTWeight(true);
        else                                    iter->MUTWeight(false);
    }
}

// Print the genotype of current genome to inspect
void Genome::PrintGenotype() const
{
    // Use iterator to travers the list
    std::cout << "Node Genes: " << std::endl;
    std::list<Node>::const_iterator node_iter = this->Nodes.begin();
    std::cout << "ID" << "\t" << "TYPE" << "\t" << "VAL" << std::endl;
    for(; node_iter != this->Nodes.end(); node_iter++)
    {
        std::cout << node_iter->ID << "\t" << node_iter->Type << "\t" << node_iter->Val << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Connection Genes: " << std::endl;
    std::list<Connection>::const_iterator conn_iter = this->Connections.begin();
    std::cout << "INNO" << "\t" << "IN" << "\t" << "OUT" << "\t" << "WEIGHT" << "\t\t" << "ENA" << std::endl;
    for(; conn_iter != this->Connections.end(); conn_iter++)
    {
        std::cout << std::fixed << std::setprecision(6);
        std::cout << conn_iter->Innov << "\t" << conn_iter->In << "\t" 
                  << conn_iter->Out << "\t" << conn_iter->Weight << "\t" 
                  << ((conn_iter->Enable) ? "True" : "False") << std::endl;
    }
}


// Getters (if need any)
std::list<Node> Genome::getNodes() const
{
    return this->Nodes;
}

std::list<Connection> Genome::getConnections() const
{
    return this->Connections;
}