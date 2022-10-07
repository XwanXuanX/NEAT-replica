#include "Genome.h"

/////////////////////////////////////////////////////////////////
// IMPORTANT: Innovation number (global throughout the project)//
unsigned int INNOV = 0;                                        //
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
void Connection::MUTWeight(const bool _isRNG)
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
    unsigned int count_innov = 0;
    for(unsigned int input = 1; input <= _InputNodes; input++)
    {
        for(unsigned int output = _InputNodes + 1; output <= (_InputNodes + _OutputNodes); output++)
        {
            count_innov++;
            Connection new_connect(count_innov, input, output);
            this->Connections.push_back(new_connect);
        }
    }
    // Assign the global innovation number the used connections
    INNOV = count_innov++;
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
        if(1 + (rand() % 100) <= _Percent)
            iter->MUTEnable();
    }
}

// Assign/Nudge each weight randomly by a percent
void Genome::MutateWeight(const unsigned int _Percent, const unsigned int _RNGPercent)
{
    if(_Percent > 100 || _Percent < 0 || _RNGPercent > 100 || _RNGPercent < 0)
    {
        std::cout << "Percentage not in range [0, 100]" << std::endl;
        throw std::domain_error("BAD VALUE PASSED!");
    }

    if(1 + (rand() % 100) <= _Percent)
    {
        std::list<Connection>::iterator iter = this->Connections.begin();
        for(; iter != this->Connections.end(); iter++)
        {
            if(1 + (rand() % 100) <= _RNGPercent)    iter->MUTWeight(true);
            else                                     iter->MUTWeight(false);
        }
    }
}

// Add a new node randomly
void Genome::AddNode(const unsigned int _Percent)
{
    if(_Percent > 100 || _Percent < 0)
    {
        std::cout << "Percentage not in range [0, 100]" << std::endl;
        throw std::domain_error("BAD VALUE PASSED!");
    }

    if(1 + (rand() % 100) <= _Percent)
    {
        // add a new node into the Node Gene list (at the very end)
        unsigned int new_id = this->Nodes.size();
        new_id += 1;
        Node new_node(new_id, "hidden");
        this->Nodes.push_back(new_node);

        // randomly access a connection and disable it
        std::list<Connection>::iterator iter = this->Connections.begin();
        for(unsigned i = 0; i < (rand() % (this->Connections.size())); i++)
        {
            iter++; // increment the list iterator
        }
        iter->Enable = false;   // disable the connection

        // add two new connections
        Connection connect_front(INNOV, new_id, iter->Out, iter->Weight);
        INNOV += 1;
        Connection connect_back(INNOV, iter->In, new_id, 1.0);
        INNOV += 1;

        // Push new connections with innov number into Connection Gene list
        this->Connections.push_back(connect_front);
        this->Connections.push_back(connect_back);
    }
}

// Add a new connection randomly
void Genome::AddConnection(const unsigned int _Percent)
{
    if(_Percent > 100 || _Percent < 0)
    {
        std::cout << "Percentage not in range [0, 100]" << std::endl;
        throw std::domain_error("BAD VALUE PASSED!");
    }

    if(1 + (rand() % 100) <= _Percent)
    {
        // Question: How to avoid feed-backward network when randomly adding connections?
        // Answer is provided as follows: 
        // 
        // Case analysis:
        // There are three cases which a node can be randomly selected: Input node; Hidden node; Output node
        // To prevent feed-backward loop in our network, we have four rules:
        //      * Input node CAN connect to any nodes WITHOUT checks;
        //      * Output node CANNOT connect to any nodes WITHOUT checks;
        //      * Hidden node CAN connect to output node, but not input nodes, WITHOUT checks;
        //      * Hidden node MAY connect to another hidden node, but WITH checks.
        //
        // Necessary checks to prevent feed-backward loop:
        //      * Perform the pre-calculation operation
        //      * If the calculation get the output                           -> NO error!
        //      * If the calculation DOES NOT evaluate at least one node once -> ERROR!
        //      * If error is detected, reverse the connection
        //      * If reversed connection exists, try new ones randomly

        std::list<Node>::const_iterator iter;
        while(true)
        {
            // Randomly select a node in the Node Gene list
            unsigned int in_node = rand() % (this->Nodes.size());
            iter = this->Nodes.begin();
            for(unsigned int i = 0; i < in_node; i++)
            {
                iter++; // increment list iterator
            }

            // Output node CANNOT connect to any nodes WITHOUT checks;
            if(iter->Type != "output") 
                break;
        }

        // Keep track of the number of input nodes
        unsigned int cnt_input = 0; // the counter variable counts the number of input nodes
        std::list<Node>::const_iterator input_iter = this->Nodes.begin();
        for(; input_iter->Type == "input"; cnt_input++)
        {
            input_iter++;
        }
        cnt_input += 1; // Next avaliable node ID

        // Input node CAN connect to any nodes WITHOUT checks;
        if(iter->Type == "input")
        {
            // Randomly select a non-input node; random range = [cnt_input, size()]
            unsigned int out_node = (rand() % (this->Nodes.size() - cnt_input + 1)) + cnt_input;

            // Check if this connection already exists
            std::list<Connection>::iterator valid_iter = this->Connections.begin();
            for(; valid_iter != this->Connections.end(); valid_iter++)
            {
                // If the connection already exists
                if(iter->ID == valid_iter->In && out_node == valid_iter->Out)
                {
                    valid_iter->MUTWeight(true);    // Only assign the connection a new weight
                    return;                         // Exit the function
                }
            }
            // if the connection does not exists, append it to the Connection Gene list
            Connection new_connect(INNOV, iter->ID, out_node);
            INNOV++;
            this->Connections.push_back(new_connect);

            return; // Exit the function
        }
        else if(iter->Type == "hidden")
        {
            std::vector<unsigned int> used_nodes;   // stores the tried-failed nodes
            // loop until find a suitable output node
            while(true)
            {
                // Exclude input nodes from the available options
                unsigned int out_node = (rand() % (this->Nodes.size() - cnt_input + 1)) + cnt_input;

                // Exclude the tried-failed nodes
                bool isvalid = true;
                for(unsigned int i = 0; i < used_nodes.size(); i++)
                {
                    if(out_node == used_nodes[i])
                        isvalid = false; break;
                }
                if(!isvalid) continue;

                // Check if this connection already exists: if yes, then must be VALID!
                std::list<Connection>::iterator valid_iter = this->Connections.begin();
                for(; valid_iter != this->Connections.end(); valid_iter++)
                {
                    // If the connection already exists
                    if(iter->ID == valid_iter->In && out_node == valid_iter->Out)
                    {
                        valid_iter->MUTWeight(true);
                        return;
                    }
                }

                // If the connection does not exists, do extra checks
                // Get the output node instance
                std::list<Node>::const_iterator output_iter = this->Nodes.begin();
                for(unsigned int i = 0; i < (out_node - 1); i++)
                {
                    output_iter++;
                }

                // Check if the output is the output nodes: if yes, then must be VALID!
                if(output_iter->Type == "output")
                {
                    Connection new_connect(INNOV, iter->ID, output_iter->ID);
                    INNOV++;
                    this->Connections.push_back(new_connect);
                    return;
                }

                // Real Challenge: if the program gets here, then the output nodes must be:
                //      * Not input node
                //      * Not tried before
                //      * Form a NEW connection
                //      * Not output node
                // Thus, we need to use the final weapon: pre-calculation

                
            }
        }
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