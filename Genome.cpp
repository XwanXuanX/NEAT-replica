#include "Genome.h"

//////////////////////////////////////////////////////////////////
// IMPORTANT: Innovation number (global throughout the project)
unsigned int INNOV = 1;
// IMPORTANT: Innovation number database
std::map<unsigned int, Connection> INNOV_DATABASE;
//////////////////////////////////////////////////////////////////
// IMPORTANT: New node number tracker
unsigned int node_tracker = 5;
// IMPORTANT: New node database
std::map<unsigned int, Connection> NODE_DATABASE;
//////////////////////////////////////////////////////////////////

// Initialize a new node with initial value 0
Node::Node(const unsigned short int _ID, const std::string _Type, const ActFunc _Mode)
{
    this->ID    = _ID;
    this->Type  = _Type;
    this->Val   = 0.0;
    this->Mode  = _Mode;
}

// Activation functions are defined here
inline double Node::act_Linear(const double x)
{
    return x;
}

inline double Node::act_Sigmoid(const double x)
{
    return (1.0 / (std::exp(-x) + 1.0));
}

inline double Node::act_Tanh(const double x)
{
    return std::tanh(x);
}

inline double Node::act_ReLU(const double x)
{
    return (x >= 0 ? x : 0);
}

inline double Node::act_Swish(const double x)
{
    return (x * Node::act_Sigmoid(x));
}

// Member function to select the type of activation function to use
void Node::ApplyActFunc()
{
    switch (this->Mode)
    {
    case ActFunc::None:     break;
    case ActFunc::Linear:   this->Val = Node::act_Linear(this->Val);  break;
    case ActFunc::Sigmoid:  this->Val = Node::act_Sigmoid(this->Val); break;
    case ActFunc::Tanh:     this->Val = Node::act_Tanh(this->Val);    break;
    case ActFunc::ReLU:     this->Val = Node::act_ReLU(this->Val);    break;
    case ActFunc::Swish:    this->Val = Node::act_Swish(this->Val);   break;
    default:                break;
    }
}

// Reload operator== to use find() function
bool Node::operator==(const unsigned short int &_OtherID) const
{
    return (this->ID == _OtherID);
}

bool Node::operator==(const Node &_OtherNode) const
{
    return (this->ID == _OtherNode.ID);
}

// Reload operator< to use sort() function on node gene list
bool Node::operator<(const Node &_OtherNode) const
{
    return (this->ID < _OtherNode.ID);
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

    // Goal: Prevent the node with the same ID present when spliting the same connection
    // Two circumstances here: A connection enable bit can:
    //      * True -> False: No issue; a false connection will not evolve a new node
    //      * False -> True:
    //          * If the connection was added to the database, delete the record
    //          * If the connection was not in the database, ignore
    if(this->Enable == true)
    {
        for(auto map_iter = NODE_DATABASE.begin(); map_iter != NODE_DATABASE.end(); map_iter++)
        {
            if(this->In == map_iter->second.In && this->Out == map_iter->second.Out)
            {
                NODE_DATABASE.erase(map_iter);
                break;
            }
        }
    }
}

// Reload operator< to use sort() function on Connection list
bool Connection::operator<(const Connection &_OtherConnection) const
{
    return (this->Innov < _OtherConnection.Innov);
}

// Reload operator== to use find() function on Connection innovation numbers
bool Connection::operator==(const Connection &_OtherConnection) const
{
    return (this->Innov == _OtherConnection.Innov);
}

// Initialize a Genome with NO hidden units (minimal structure)
// Initialize 1st generation
Genome::Genome(const unsigned int _InputNodes, const unsigned int _OutputNodes, const Node::ActFunc _OutputMode)
{
    unsigned int node_added = 0;
    // Add input and output nodes to the Node Gene list
    for(; node_added < (_InputNodes + _OutputNodes); node_added++)
    {
        Node new_node(node_added + 1, (node_added < _InputNodes) ? "input" : "output", 
                     (node_added < _InputNodes) ? Node::ActFunc::None : _OutputMode);
        this->Nodes.push_back(new_node);
    }

    // Add connections to the Connection Gene list
    for(unsigned int input = 1; input <= _InputNodes; input++)
    {
        for(unsigned int output = _InputNodes + 1; output <= (_InputNodes + _OutputNodes); output++)
        {
            Connection new_connect(0, input, output);
            this->ConnectionProcessor(new_connect);
        }
    }

    // Initialize fitness to be 0
    this->Fitness = 0;
}

// Initialize offsprings
Genome::Genome(const std::list<Node> _Nodes, const std::list<Connection> _Connections)
{
    this->Nodes       = _Nodes;
    this->Connections = _Connections;
    this->Fitness     = 0;
}

// Check if the connection is in the INNOV_DATABASE
//      * If not, insert new connection and new INNOV number, and add to Connection Gene list
//      * If yes, add to Connection Gene list using the existing INNOV number
bool Genome::ConnectionProcessor(Connection new_connect)
{
    bool in_database = false;
    unsigned int EX_innov = 0;
    // iterate through the Innovation Database and match existing connections
    std::map<unsigned int, Connection>::const_iterator database_iter = INNOV_DATABASE.begin();
    for(; database_iter != INNOV_DATABASE.end(); database_iter++)
    {
        if(database_iter->second.In == new_connect.In && database_iter->second.Out == new_connect.Out)
        {
            EX_innov = database_iter->first;
            in_database = true;
            break;
        }
    }

    // If not, insert new connection and new INNOV number, and add to Connection Gene list
    if(in_database == false)
    {
        INNOV_DATABASE.insert(std::pair<unsigned int, Connection>(INNOV, new_connect));
        new_connect.Innov = INNOV;
        this->Connections.push_back(new_connect);
        INNOV += 1;
    }
    // If yes, add to Connection Gene list using the existing INNOV number
    else
    {
        new_connect.Innov = EX_innov;
        this->Connections.push_back(new_connect);
    }

    return in_database;
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
        {
            iter->MUTEnable();
#if (defined DEBUG) && (defined MUTATION)
            std::cout << "Connection " << iter->In << "-" << iter->Out << ": Enable mutation completed." << std::endl;
#endif
        }
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
            if(1 + (rand() % 100) <= _RNGPercent)    
                iter->MUTWeight(true);
            else                                     
                iter->MUTWeight(false);
        }
#if (defined DEBUG) && (defined MUTATION)
        std::cout << "Genome connection weights mutation completed." << std::endl;
#endif
    }
}

// Add a new node randomly
void Genome::AddNode(const unsigned int _Percent, const Node::ActFunc _HiddenMode)
{
    if(_Percent > 100 || _Percent < 0)
    {
        std::cout << "Percentage not in range [0, 100]" << std::endl;
        throw std::domain_error("BAD VALUE PASSED!");
    }

    if(1 + (rand() % 100) <= _Percent)
    {
        // randomly access a connection and disable it
        std::list<Connection>::iterator iter;
        while(true)
        {
            iter = this->Connections.begin();
            for(unsigned i = 0; i < (rand() % (this->Connections.size())); i++)
            {
                iter++; // increment the list iterator
            }

            // Already disabled connection cannot be disabled twice
            if(iter->Enable != false) 
                break;
            else                      
                continue;
        }

        Node new_node(0, "hidden", _HiddenMode);
        bool in_database = false;
        unsigned int new_id = 0;
        // Check if the selected connection is in the node database
        std::map<unsigned int, Connection>::const_iterator database_iter = NODE_DATABASE.begin();
        for(; database_iter != NODE_DATABASE.end(); database_iter++)
        {
            if(iter->In == database_iter->second.In && iter->Out == database_iter->second.Out)
            {
                new_id = database_iter->first;
                in_database = true;
                break;
            }
        }

        // Add a new hidden node to the Node Gene list
        if(in_database == false)
        {
            NODE_DATABASE.insert(std::pair<unsigned int, Connection>(node_tracker, *iter));
            new_node.ID = node_tracker;
            this->Nodes.push_back(new_node);
            node_tracker += 1;
        }
        else
        {
            new_node.ID = new_id;
            this->Nodes.push_back(new_node);
        }

        // disable the connection
        iter->Enable = false;

        // add two new connections
        Connection connect_front(0, new_node.ID, iter->Out, iter->Weight);
        Connection connect_back(0, iter->In, new_node.ID, 1.0);
        this->ConnectionProcessor(connect_front);
        this->ConnectionProcessor(connect_back);

#if (defined DEBUG) && (defined MUTATION)
        std::cout << "Node " << new_node.ID << " is successfully added." << std::endl;
        std::cout << "Connection " << iter->In << "-" << new_node.ID << " is successfully added." << std::endl;
        std::cout << "Connection " << new_node.ID << "-" << iter->Out << " is successfully added. " << std::endl;
#endif
    }
}

bool Genome::_PreCalculation(const unsigned int _InputNode, const unsigned int _OutputNode, const unsigned int _InputNum) const
{
    // In terms of random access, vectors are more efficient than linkedlist
    std::vector<Node> tmpNodes;
    std::list<Node>::const_iterator tmpIter = this->Nodes.begin();
    for(; tmpIter != this->Nodes.end(); tmpIter++)
    {
        // transfer all the nodes into vector
        tmpNodes.push_back(*tmpIter);
    }

    for(unsigned int i = 0; i < _InputNum; i++)
    {
        // Assign the input nodes fake input values to make them non-empty
        tmpNodes[i].Val = 1.0;
    }

    // Make a copy of the existing Connection Gene list, modify it for our purpose
    std::list<Connection> tmpConnections(this->Connections);
    Connection new_connect(0, _InputNode, _OutputNode);
    tmpConnections.push_back(new_connect);  // push the fake connection into the list

#if (defined DEBUG) && (defined PRECALC)
    for(unsigned int i = 0; i < tmpNodes.size(); i++)
        std::cout << tmpNodes[i].Val << " ";
    std::cout << std::endl;
#endif

    // enumerate and calculate the non-input nodes one by one
    // In the most negative circumstance, it will take the network (size() - _InputNum) times to fully evaluate
    for(unsigned int recur = 1; recur <= (tmpNodes.size() - _InputNum); recur++)
    {
        for(unsigned int node = _InputNum; node < tmpNodes.size(); node++)
        {
            // Check if the node is non-empty
            if(tmpNodes[node].Val != 0.0)
                continue;

            // Check if the node can be calculated
            bool isvalid = true;
            std::list<Connection>::const_iterator iter = tmpConnections.begin();
            for(; iter != tmpConnections.end(); iter++)
            {
                std::vector<Node>::const_iterator node_finder = std::find(tmpNodes.begin(), tmpNodes.end(), iter->In);
                if(iter->Enable == true && iter->Out == tmpNodes[node].ID && node_finder->Val == 0.0)
                {
                    isvalid = false; 
                    break;
                }
            }
            if(!isvalid) 
                continue;
            // If the above valid-detecting system did not trigger any errors, meaning the node can be calculated
            // To maximize efficiency, making the node non-empty is sufficient
            else 
                tmpNodes[node].Val = 1.0;
        }

        // Check if all nodes have already been calculated
        bool should_exit = true;
        for(unsigned int i = 0; i < tmpNodes.size(); i++)
        {
#if (defined DEBUG) && (defined PRECALC)
            std::cout << tmpNodes[i].Val << " ";
#endif
            if(tmpNodes[i].Val == 0.0)
                should_exit = false;
        }
#if (defined DEBUG) && (defined PRECALC)
        std::cout << std::endl;
#endif
        if(should_exit == true)
        {
#if (defined DEBUG) && (defined PRECALC)
            std::cout << "Connection " << _InputNode << "-" << _OutputNode << " PASSES the pre-calc test in "
                      << recur << " iteration(s)." << std::endl;
#endif
            return true;
        }
        else
            continue;
    }

#if (defined DEBUG) && (defined PRECALC)
    std::cout << "Connection " << _InputNode << "-" << _OutputNode << " FAILS the pre-calc test." << std::endl;
#endif
    return false;
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

        // Since now the Node Genes are no longer sequential, we need to copy the Node IDs into a vector
        std::vector<Node> tmpNodes;
        std::list<Node>::const_iterator tmpIter = this->Nodes.begin();
        for(; tmpIter != this->Nodes.end(); tmpIter++)
        {
            tmpNodes.push_back(*tmpIter);
        }

        unsigned int in_node = 0;
        while(true)
        {
            // Randomly select a node in the Node Gene list
            in_node = rand() % (tmpNodes.size());
            // Output node CANNOT connect to any nodes WITHOUT checks;
            if(tmpNodes[in_node].Type != "output")
                break;
        }
        Node input_node(tmpNodes[in_node]);

        // Keep track of the number of input nodes
        unsigned int cnt_input = 0; // the counter variable counts the number of input nodes
        for(unsigned int i = 0; i < tmpNodes.size(); i++)
        {
            if(tmpNodes[i].Type == "input")
                cnt_input++;
        }

        // Input node CAN connect to any nodes WITHOUT checks;
        if(input_node.Type == "input")
        {
            unsigned int low  = cnt_input + 1;
            unsigned int high = tmpNodes.size();
            // Randomly select a non-input node;
            unsigned int out_node = (rand() % (high - low + 1)) + low;
            Node output_node(tmpNodes[out_node - 1]);

            // Check if this connection already exists
            std::list<Connection>::iterator valid_iter = this->Connections.begin();
            for(; valid_iter != this->Connections.end(); valid_iter++)
            {
                // If the connection already exists
                if(input_node.ID == valid_iter->In && output_node.ID == valid_iter->Out)
                {
                    valid_iter->MUTWeight(true);    // Only assign the connection a new weight
#if (defined DEBUG) && (defined MUTATION)
                    std::cout << "Connection " << input_node.ID << "-" << output_node.ID 
                              << " already exists. No connections were added." << std::endl;
#endif
                    return;     // Exit the function
                }
            }

            // if the connection does not exists, append it to the Connection Gene list
            Connection new_connect(0, input_node.ID, output_node.ID);
            this->ConnectionProcessor(new_connect);
#if (defined DEBUG) && (defined MUTATION)
            std::cout << "Connection " << input_node.ID << "-" << output_node.ID 
                      << " is successfully added." << std::endl;
#endif
            return;     // Exit the function
        }
        else if(input_node.Type == "hidden")
        {
            std::vector<unsigned int> used_nodes;   // stores the tried-failed nodes
            // loop until find a suitable output node
            while (true)
            {
                unsigned int low  = cnt_input + 1;
                unsigned int high = tmpNodes.size();
                // Exclude input nodes from the available options
                unsigned int out_node = (rand() % (high - low + 1)) + low;
                Node output_node(tmpNodes[out_node - 1]);

                // Exclude itself: connection cannot start and end on one node
                if(input_node.ID == output_node.ID)
                    continue;

                // Exclude the tried-failed nodes
                bool isvalid = true;
                for(unsigned int i = 0; i < used_nodes.size(); i++)
                {
                    if(output_node.ID == used_nodes[i])
                    {
                        isvalid = false;
                        break;
                    }
                }
                if(!isvalid)
                    continue;
                
                // Check if this connection already exists: if yes, then must be VALID!
                std::list<Connection>::iterator valid_iter = this->Connections.begin();
                for(; valid_iter != this->Connections.end(); valid_iter++)
                {
                    // If the connection already exists
                    if(input_node.ID == valid_iter->In && output_node.ID == valid_iter->Out)
                    {
                        valid_iter->MUTWeight(true);
#if (defined DEBUG) && (defined MUTATION)
                        std::cout << "Connection " << input_node.ID << "-" << output_node.ID
                                  << " already exists. No connections were added." << std::endl;
#endif
                        return;
                    }
                }

                // If the connection does not exists, do extra checks
                // Check if the output is the output nodes: if yes, then must be VALID!
                if(output_node.Type == "output")
                {
                    Connection new_connect(0, input_node.ID, output_node.ID);
                    this->ConnectionProcessor(new_connect);
#if (defined DEBUG) && (defined MUTATION)
                    std::cout << "Connection " << input_node.ID << "-" << output_node.ID 
                              << " is successfully added." << std::endl;
#endif
                    return;
                }

                // Real Challenge: if the program gets here, then the output nodes must be:
                //      * Not input node
                //      * Not itself
                //      * Not tried before
                //      * Form a NEW connection
                //      * Not output node
                // Thus, we need to use the final weapon: pre-calculation!
                //
                // Q: Of course, we can use pre-calc as a general detection method. So why bother listing all other probabilities?
                // A: Yes, pre-cal is a general method. However, it is less efficient to use. 
                //    If any other probabilities are satisfied, we try to skip using pre-cal as much as possible.

                if(this->_PreCalculation(input_node.ID, output_node.ID, cnt_input) == true)
                {
                    // If pre-calc method passed, this connection can be formally added
                    Connection new_connect(0, input_node.ID, output_node.ID);
                    this->ConnectionProcessor(new_connect);
#if (defined DEBUG) && (defined MUTATION)
                    std::cout << "Connection " << input_node.ID << "-" << output_node.ID 
                              << " is successfully added." << std::endl;
#endif
                    return;
                }
                else
                    used_nodes.push_back(output_node.ID);
                continue;
            }
        }
    }
}

// Mutate the Genome in five possible ways
// If you want to disable any type of mutation, just set the percent to 0
void Genome::Mutate(const unsigned int _ToggleConnect_Percent, 
                    const unsigned int _MutateWeight_Percent, const unsigned int _RNGPercent, 
                    const unsigned int _AddNode_Percent, const Node::ActFunc _HiddenMode, 
                    const unsigned int _AddConnection_Percent)
{
    // Enable/Disable each connection randomly by a percent
    this->ToggleConnect(_ToggleConnect_Percent);

    // Assign/Nudge each weight randomly by a percent
    this->MutateWeight(_MutateWeight_Percent, _RNGPercent);

    // Add a new node randomly
    this->AddNode(_AddNode_Percent, _HiddenMode);

    // Add a new connection randomly
    this->AddConnection(_AddConnection_Percent);
}

// Propagate (a.k.a. calculate) the output of the network based on the given input
std::vector<double> Genome::Propagate(double* _pInputs, const std::size_t _InputLength) const
{
    // To randomly access each node more efficiently, transfer all nodes into vector
    std::vector<Node> tmpNodes;
    std::list<Node>::const_iterator node_iter = this->Nodes.begin();
    for(; node_iter != this->Nodes.end(); node_iter++)
    {
        tmpNodes.push_back(*node_iter);
    }

    // Check if the _InputLength is equal to the number of input nodes
    unsigned int cnt_input = 0; // the counter variable counts the number of input nodes
    for(unsigned int i = 0; i < tmpNodes.size(); i++)
    {
        if(tmpNodes[i].Type == "input")
            cnt_input += 1;
    }
    if(_InputLength != cnt_input)
        // if the input nodes and the input length does not match, exit immediately
        throw std::domain_error("BAD INPUT LENGTH!");

    // Place each input sequentially into each input nodes
    for(std::size_t i = 0; i < _InputLength; i++)
    {
        tmpNodes[i].Val = _pInputs[i];
    }

#if (defined DEBUG) && (defined PROPAGATE)
    for(unsigned int i = 0; i < tmpNodes.size(); i++)
        std::cout << tmpNodes[i].Val << " ";
    std::cout << std::endl;
#endif

    std::vector<double> outputs;
    // Forward-Propagation starts!
    for(unsigned int recur = 1; recur <= (tmpNodes.size() - cnt_input); recur++)
    {
        for(unsigned int node = cnt_input; node < tmpNodes.size(); node++)
        {
            // Check if the node is non-empty
            if(tmpNodes[node].Val != 0.0)
                continue;

            // The map holds all the inputs and its corresponding weights as Pairs (Input, Weight)
            std::map<double, double> Input_Weight;

            bool isvalid = true;
            std::list<Connection>::const_iterator iter = this->Connections.begin();
            for(; iter != this->Connections.end(); iter++)
            {
                std::vector<Node>::const_iterator node_finder = std::find(tmpNodes.begin(), tmpNodes.end(), iter->In);
                if(iter->Enable == true && iter->Out == tmpNodes[node].ID && node_finder->Val == 0.0)
                {
                    isvalid = false;
                    break;
                }
                if(iter->Enable == true && iter->Out == tmpNodes[node].ID && node_finder->Val != 0.0)
                    Input_Weight.insert(std::pair<double, double>(node_finder->Val, iter->Weight));
            }
            
            if(!isvalid)
                continue;
            else
            {
                // Add up each weight multiply by its corresponding input. Then apply activation function.
                std::map<double, double>::const_iterator map_iter = Input_Weight.begin();
                for(; map_iter != Input_Weight.end(); map_iter++)
                {
                    tmpNodes[node].Val += (map_iter->first * map_iter->second);
                }
                tmpNodes[node].ApplyActFunc();
            }
        }

        // Check if all nodes have already been calculated
        bool should_exit = true;
        outputs.clear();
        for(unsigned int i = 0; i < tmpNodes.size(); i++)
        {
#if (defined DEBUG) && (defined PROPAGATE)
            std::cout << tmpNodes[i].Val << " ";
#endif
            // Push the value of the output nodes into a vector and return it
            if(tmpNodes[i].Type == "output")
                outputs.push_back(tmpNodes[i].Val);
            // Check if there are remaining zeros in the list
            if(tmpNodes[i].Val == 0.0)
                should_exit = false;
        }
#if (defined DEBUG) && (defined PROPAGATE)
        std::cout << std::endl;
#endif
        if(should_exit == true)
        {
#if (defined DEBUG) && (defined PROPAGATE)
            std::cout << "Output was calculated in " << recur << " times." << std::endl;
#endif
            return outputs;
        }
        else
            continue;
    }
#if (defined DEBUG) && (defined PROPAGATE)
    std::cout << "ERROR: output not returned properly." << std::endl;
#endif
    return outputs;
}

// Calculate the compatibility distance between two genomes
double Genome::CompatDistance(const Genome &_Other, const double c1, const double c2, const double c3, const unsigned int _Normalize_Threshold) const
{
    // Seperate the Connection Genes into different list for access later
    std::list<Connection> this_connect = this->Connections;
    std::list<Connection> other_connect = _Other.Connections;

    // Before modifying any list, record the param N
    unsigned int N = ((this_connect.size() >= other_connect.size()) ? this_connect.size() : other_connect.size());
    if(N < _Normalize_Threshold)
        N = 1;

    // Sort these two vectors according to innovation numbers in accending order
    this_connect.sort();
    other_connect.sort();

#if (defined DEBUG) && (defined COMPAT)
    auto InspectVals = [](auto this_connect, auto other_connect) -> void
    {
        for(auto tmpIter = this_connect.begin(); tmpIter != this_connect.end(); tmpIter++)
            std::cout << tmpIter->Innov << " ";
        std::cout << std::endl;
        for(auto tmpIter = other_connect.begin(); tmpIter != other_connect.end(); tmpIter++)
            std::cout << tmpIter->Innov << " ";
        std::cout << std::endl;
        std::cout << std::endl;
    };

    InspectVals(this_connect, other_connect);
#endif

    // Find the number of excess genes
    std::list<Connection>::reverse_iterator this_iter = this_connect.rbegin();
    std::list<Connection>::reverse_iterator other_iter = other_connect.rbegin();

    // Define a lambda expression to calculate the number of excess genes
    auto CalcExcess = [] (std::list<Connection> *Ex_connect, unsigned int min_innov) -> unsigned int
    {
        unsigned int ExcessGenes = 0;
        auto connect_iter = Ex_connect->rbegin();
        while(connect_iter != Ex_connect->rend())
        {
            if(connect_iter->Innov > min_innov)
            {
                ExcessGenes += 1;
                Ex_connect->pop_back();
            }
            else
                break;
            connect_iter = Ex_connect->rbegin();
        }
        return ExcessGenes;
    };

    // Only find excess genes when two innovation numbers are different
    unsigned int Excess = 0;
    if(this_iter->Innov > other_iter->Innov)
        Excess = CalcExcess(&this_connect, other_iter->Innov);
    else if(this_iter->Innov < other_iter->Innov)
        Excess = CalcExcess(&other_connect, this_iter->Innov);

#if (defined DEBUG) && (defined COMPAT)
    InspectVals(this_connect, other_connect);
    std::cout << "Excess = " << Excess << std::endl;
#endif

    // Use the remaining lists to find the matching genes first
    double Matching = 0;
    unsigned int _matching_num = 0;
    double _total_diff = 0;
    for(auto this_iter = this_connect.begin(); this_iter != this_connect.end(); this_iter++)
    {
        for(auto other_iter = other_connect.begin(); other_iter != other_connect.end(); other_iter++)
        {
            if(this_iter->Innov == other_iter->Innov)
            {
                _matching_num += 1;
                _total_diff += std::abs(this_iter->Weight - other_iter->Weight);
            }
        }
    }
    Matching = _total_diff / _matching_num;

#if (defined DEBUG) && (defined COMPAT)
    std::cout << "Matching = " << Matching << std::endl;
#endif

    // Finally find the number of Disjoint genes
    unsigned int Disjoint = 0;
    Disjoint = (this_connect.size() - _matching_num) + (other_connect.size() - _matching_num);

#if (defined DEBUG) && (defined COMPAT)
    std::cout << "Disjoint = " << Disjoint << std::endl;
#endif

    // Calculate the compatibility distance
    double CompatDistance = \
        (c1 * Excess / N) + (c2 * Disjoint / N) + (c3 * Matching);

    return CompatDistance;
}

// Crossover (a.k.a. breed) between two genomes
Genome Genome::Crossover(const Genome &_Other, const double this_fitness, const double other_fitness) const
{
    std::list<Connection> this_connect = this->Connections;
    std::list<Connection> other_connect = _Other.Connections;

    auto AssignMatching = [] (std::list<Connection> *_New, const std::list<Connection> &_Else) -> void
    {
        for(auto new_iter = _New->begin(); new_iter != _New->end(); new_iter++)
        {
            for(auto else_iter = _Else.begin(); else_iter != _Else.end(); else_iter++)
            {
                if(new_iter->Innov == else_iter->Innov && (1 + (rand() % 100)) <= 50)
                    new_iter->Weight = else_iter->Weight;
            }
        }
    };

    // Three circumstances: this more fit; other more fit; both equally fit
    // This genome is more fit than other genome
    if(this_fitness > other_fitness)
    {
        AssignMatching(&this_connect, other_connect);
        Genome new_Genome(this->Nodes, this_connect);
#if (defined DEBUG) && (defined CROSSOVER)
        new_Genome.PrintGenotype();
#endif
        return new_Genome;
    }
    // Other genome is more fit than this genome
    else if(this_fitness < other_fitness)
    {
        AssignMatching(&other_connect, this_connect);
        Genome new_Genome(_Other.Nodes, other_connect);
#if (defined DEBUG) && (defined CROSSOVER)
        new_Genome.PrintGenotype();
#endif
        return new_Genome;
    }
    // Both genomes are equally fit
    else
    {
        // Modify the matching genes and inherit the E/D genes from ANYONE regardless of fitness
        AssignMatching(&this_connect, other_connect);

        // Merge the E/D genes from another parent
        for(auto other_iter = other_connect.begin(); other_iter != other_connect.end(); other_iter++)
        {
            // Check if the connection already exist in this_connection
            std::list<Connection>::iterator tmpIter = std::find(this_connect.begin(), this_connect.end(), *other_iter);
            if(tmpIter == this_connect.end())   // If not found;
                this_connect.push_back(*other_iter);
        }

        // Derive the node genes from the connection genes
        std::list<Node> new_nodes(this->Nodes);
        std::list<Node> other_nodes(_Other.Nodes);
        new_nodes.merge(other_nodes);   // Merge the two node gene list together
        new_nodes.sort();               // Sort the node gene list in ascending order
        new_nodes.unique();             // Delete repeated nodes

        // To prevent from naming different node with the same ID, delete the record from database
        for(auto cnt_iter = this_connect.begin(); cnt_iter != this_connect.end(); cnt_iter++)
        {
            if(cnt_iter->Enable == false)
                continue;
            else
            {
                std::map<unsigned int, Connection>::iterator node_finder = NODE_DATABASE.begin();
                for(; node_finder != NODE_DATABASE.end(); node_finder++)
                {
                    if(cnt_iter->In == node_finder->second.In && cnt_iter->Out == node_finder->second.Out)
                    {
                        NODE_DATABASE.erase(node_finder);
                        break;
                    }
                }
            }
        }

        // Create new Genome
        Genome new_Genome(new_nodes, this_connect);
#if (defined DEBUG) && (defined CROSSOVER)
        new_Genome.PrintGenotype();
#endif
        return new_Genome;
    }
}

// Calculate adjusted fitness
void Genome::CalcAdjFitness(const unsigned int _Species_Num)
{
    this->Fitness = this->Fitness / _Species_Num;
}

// Print the genotype of current genome to inspect
void Genome::PrintGenotype() const
{
    // Use iterator to travers the list
    std::cout << "Node Genes: " << std::endl;
    std::list<Node>::const_iterator node_iter = this->Nodes.begin();
    std::cout << "ID" << "\t" << "TYPE" << "\t" << "VAL" << "\t\t" << "AF" << std::endl;
    for(; node_iter != this->Nodes.end(); node_iter++)
    {
        std::string node_af{};
        switch (node_iter->Mode)
        {
        case Node::ActFunc::None:       node_af = "None";       break;
        case Node::ActFunc::Linear:     node_af = "Linear";     break;
        case Node::ActFunc::Sigmoid:    node_af = "Sigmoid";    break;
        case Node::ActFunc::Tanh:       node_af = "Tanh";       break;
        case Node::ActFunc::Swish:      node_af = "Swish";      break;
        default:                                                break;
        }

        std::cout << node_iter->ID << "\t" << node_iter->Type << "\t" << node_iter->Val << "\t" << node_af << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Connection Genes: " << std::endl;
    std::list<Connection>::const_iterator conn_iter = this->Connections.begin();
    std::cout << "INNOV" << "\t" << "IN" << "\t" << "OUT" << "\t" << "WEIGHT" << "\t\t" << "ENA" << std::endl;
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

double Genome::getFitness() const
{
    return this->Fitness;
}

// Setters (Only need for fitness; need to assign fitness from outside)
void Genome::setFitness(const double _Fitness)
{
    this->Fitness = _Fitness;
}

// Overload operator> to sort genomes in descending order
bool Genome::operator>(const Genome &_OtherGenome) const
{
    return (this->Fitness > _OtherGenome.Fitness);
}