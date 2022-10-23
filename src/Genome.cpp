/**
 * @file Genome.cpp
 * @brief Function bodies of function declarations in Genome.h
 * @author Yetong (Tony) Li
 * @date Oct 3, 2022
 * @version 1.0.1
*/

#include "Genome.h"

/**
 * @brief Innovation number (global throughout the project); \
 * @brief Every unique connection MUST have different innovation number; \
 * @brief Every same connection MUST have the same innovation number.
*/
UINT INNOV = 1;

/**
 * @brief Innovation number database; \
 * @brief Used to track existing innovation numbers
*/
std::map<UINT, Connection> INNOV_DATABASE;

/**
 * @brief Node ID number (global throughout the project); \
 * @brief When the same connection is broken, node with the same ID must be added
*/
extern UINT node_tracker;

/**
 * @brief Node ID number database; \
 * @brief Used to track existing node IDs
*/
std::map<UINT, Connection> NODE_DATABASE;

/*________________________________________________________________________________________________________*/
/*                                          Node Struct                                                   */

/**
 * @brief Initialize the structure of a genome
 * @param[in] _ID The unique identification number of each node
 * @param[in] _Type The type of the node: Sensor/Hidden/Output
 * @param[in] _Mode The activation function the node use
*/
Node::Node(const UINT _ID, const NodeType _Type, const ActFunc _Mode) 
    : ID(_ID), Type(_Type), Mode(_Mode), Val(0.0) { }

/**
 * @brief A number of useful activation functions
 * @param[in] x The input value of the function
 * @return The output value of the function
*/
inline double Node::act_Linear(const double x) { return x; }
inline double Node::act_Sigmoid(const double x) { return (1.0 / (std::exp(-x) + 1.0)); }
inline double Node::act_Tanh(const double x) { return std::tanh(x); }
inline double Node::act_ReLU(const double x) { return (x >= 0 ? x : 0); }
inline double Node::act_Swish(const double x) { return (x * Node::act_Sigmoid(x)); }
inline double Node::act_Modified_Sigmoid(const double x) { return (1.0 / (std::exp(-4.9 * x) + 1.0)); }

/**
 * @brief Apply activation function to the node's output 
*/
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
    case ActFunc::Modified_Sigmoid:
                            this->Val = Node::act_Modified_Sigmoid(this->Val); break;
    default:                break;
    }
}

bool Node::operator==(const UINT &_NodeID) const { return (this->ID == _NodeID); }
bool Node::operator==(const Node &_OtherNode) const { return (this->ID == _OtherNode.ID); }
bool Node::operator<(const Node &_OtherNode) const { return (this->ID < _OtherNode.ID); }

/*________________________________________________________________________________________________________*/
/*                                          Connection Struct                                             */

/**
 * @brief Initialize connection with random weights
 * @param[in] _In The input node of the connection
 * @param[in] _Out The output node of the connection
*/
Connection::Connection(const UINT & _In, const UINT &_Out)
    : In(_In), Out(_Out), Innov(0), Enable(true), 
      Weight((double)rand() / ((double)RAND_MAX / 4) - 2.0) { }

/**
 * @brief Initialize connection with assigned weights
 * @param[in] _In The input node of the connection
 * @param[in] _Out The output node of the connection
 * @param[in] _Weight The assigned weight of the connection
*/
Connection::Connection(const UINT &_In, const UINT &_Out, const double _Weight)
    : In(_In), Out(_Out), Innov(0), Enable(true), Weight(_Weight) { }

/**
 * @brief Mutate the connection weight in two ways
 * @param[in] _isRNG When true, randomly assign new weight; When false, nudge the weight slightly
*/
void Connection::MUTWeight(const bool _isRNG)
{
    if(_isRNG)  // assign completely random number between -2 and 2
        this->Weight = (double)rand() / ((double)RAND_MAX / 4) - 2.0;
    else        // nudge the weight slightly by multiplying by random number between 0 and 2
        this->Weight *= (double)rand() / ((double)RAND_MAX / 2);
}

bool Connection::operator<(const Connection &_OtherConnection) const 
{
    return (this->Innov < _OtherConnection.Innov);
}

bool Connection::operator==(const Connection &_OtherConnection) const
{
    return (this->Innov == _OtherConnection.Innov);
}

/*________________________________________________________________________________________________________*/
/*                                          Genome Class                                                  */

/**
 * @brief Initialize a Genome with NO hidden units (minimal structure)
 * @param[in] _InputNodes The number of input nodes
 * @param[in] _OutputNodes The number of output nodes
 * @param[in] _OutputMode The activation function of output nodes
*/
Genome::Genome(const UINT &_InputNodes, const UINT &_OutputNodes, const Node::ActFunc &_OutputMode)
    : Fitness(0.0)
{
    UINT node_added = 0;
    const UINT total_node = _InputNodes + _OutputNodes;

    // Add input and output nodes to the Node Gene list
    register bool isInput{};
    for(; node_added < total_node; node_added++)
    {
        isInput = (node_added < _InputNodes);
        this->Nodes.push_back(
            Node(node_added + 1, (isInput) ? Node::NodeType::Input : Node::NodeType::Output, 
                 (isInput) ? Node::ActFunc::None : _OutputMode)
        );
    }

    // Add connections to the Connection Gene list
    const UINT output_id = _InputNodes + 1;
	Connection* new_connect = nullptr;
    for(UINT input = 1; input <= _InputNodes; input++)
    {
        for(UINT output = output_id; output <= total_node; output++)
        {
			new_connect = new Connection(input, output);
			this->ConnectionProcessor(new_connect);
			delete new_connect;
        }
    }
	new_connect = nullptr;
    assert(this->Nodes.size() == total_node);
    assert(this->Connections.size() == (_InputNodes * _OutputNodes));
}

/**
 * @brief Initialize a Genome with existing node genes and connection genes
 * @param[in] _Nodes The node gene list
 * @param[in] _Connections The connection gene list
*/
Genome::Genome(const NODEVEC &_Nodes, const CNCTVEC &_Connections)
    : Nodes(_Nodes), Connections(_Connections), Fitness(0.0) { }

/**
 * @brief Check if the connection is in the INNOV_DATABASE
 * @param[in] new_connect The new connection about to be added
 * @return If yes, add to Connection Gene list using the existing INNOV number; return true \
 * @return If not, insert new connection and new INNOV number, and add to Connection Gene list; return false
*/
bool Genome::ConnectionProcessor(Connection* const new_connect)
{
    bool in_database = false;
    UINT EX_innov = 0;

    // iterate through the Innovation Database and match existing connections
    const auto iter_end = INNOV_DATABASE.end();
    for(auto database_iter = INNOV_DATABASE.begin(); database_iter != iter_end; database_iter++)
    {
        if(database_iter->second.In == new_connect->In && database_iter->second.Out == new_connect->Out)
        {
            EX_innov = database_iter->first;
            in_database = true;
            break;
        }
    }

    // If not, insert new connection and new INNOV number, and add to Connection Gene list
    if(!in_database)
    {
        INNOV_DATABASE.insert(std::pair<UINT, Connection>(INNOV, *new_connect));
        new_connect->Innov = INNOV;
        this->Connections.push_back(*new_connect);
        INNOV += 1;
    }
    // If yes, add to Connection Gene list using the existing INNOV number
    else
    {
        new_connect->Innov = EX_innov;
        this->Connections.push_back(*new_connect);
    }

    return in_database;
}

/**
 * @brief Assign/Nudge each weight randomly by a percent
 * @param[in] _Percent The probability of mutating a connection weight
 * @param[in] _RNGPercent The probability of mutating a weight randomly
*/
void Genome::MutateWeight(const UINT &_Percent, const UINT &_RNGPercent)
{
    if(1 + (rand() % 100) <= _Percent)
    {
        const UINT cnct_size = this->Connections.size();
        CNCTVEC* p_connection = &this->Connections;
        for(UINT iter = 0; iter < cnct_size; iter++)
        {
            if(1 + (rand() % 100) <= _RNGPercent)
                p_connection->at(iter).MUTWeight(true);
            else
                p_connection->at(iter).MUTWeight(false);
        }
#if (defined DEBUG) && (defined MUTATION)
        std::cout << "Genome connection weights mutation completed." << std::endl;
#endif
		p_connection = nullptr;
    }
}

/**
 * @brief Add a new node randomly by a percent
 * @param[in] _Percent The probability of adding a new node
 * @param[in] _HiddenMode The activation function of the new node
*/
void Genome::AddNode(const UINT &_Percent, const Node::ActFunc &_HiddenMode)
{
	if(1 + (rand() % 100) <= _Percent)
	{
		const UINT length = this->Connections.size();
		int* avai_i = new int[length];
		int* avai_f = avai_i;
		for(UINT i = 0; i < length; i++)
		{
			if(this->Connections.at(i).Enable)
			{
				*avai_f = i;
				avai_f++;
			}
		}
		if(avai_f == avai_i)
			return;
		const UINT avai = avai_f - avai_i;
		assert(avai != 0);
		Connection selected(this->Connections.at(avai_i[rand() % avai]));
		delete[] avai_i;
		avai_i = avai_f = nullptr;

		bool in_database = false;
		UINT Ex_ID = 0;
		// Check if the selected connection is in the node database
		const auto iter_end = NODE_DATABASE.end();
		for(auto iter = NODE_DATABASE.begin(); iter != iter_end; iter++)
		{
			if(selected.In == iter->second.In && selected.Out == iter->second.Out)
			{
				Ex_ID = iter->first;
				in_database = true;
				break;
			}
		}
		Node new_node(0, Node::NodeType::Hidden, _HiddenMode);
		if(!in_database)
		{
			NODE_DATABASE.insert(std::pair<UINT, Connection>(node_tracker, selected));
			new_node.ID = node_tracker;
			this->Nodes.push_back(new_node);
			node_tracker++;
		}
		else
		{
			new_node.ID = Ex_ID;
			this->Nodes.push_back(new_node);
		}

		// disable the connection and add two new connections
		selected.Enable = false;
		Connection* p_new_connect = new Connection(new_node.ID, selected.Out, selected.Weight);
		this->ConnectionProcessor(p_new_connect);
		delete p_new_connect;
		p_new_connect = new Connection(selected.In, new_node.ID, 1.0);
		this->ConnectionProcessor(p_new_connect);
		delete p_new_connect;
		p_new_connect = nullptr;
	}
}

/**
 * @brief Check if a new connection creates feedback loop
 * @param[in] _InputNode The input node of the connection
 * @param[in] _OutputNode The output node of the connection
 * @param[in] _InputNum The number of input nodes of the network
 * @return If connection pass pre-calc test, return true; else, return false
*/
bool Genome::_PreCalculation(const UINT &_InputNode, const UINT &_OutputNode, const UINT &_InputNum) const
{
	NODEVEC tmpNodes(this->Nodes);
	for(UINT i = 0; i < _InputNum; i++)
		tmpNodes.at(i).Val = 1.0;

    // Make a copy of the existing Connection Gene list, modify it for our purpose
	CNCTVEC tmpConnections(this->Connections);
	tmpConnections.push_back(
		Connection(_InputNode, _OutputNode)
	);
	
    // enumerate and calculate the non-input nodes one by one
    // In the most negative circumstance, it will take the network (size() - _InputNum) times to fully evaluate
    const UINT max_recur = tmpNodes.size() - _InputNum;
	register bool isvalid{};
	const auto cnct_end = tmpConnections.end();
	const UINT node_size = tmpNodes.size();
	for(UINT recur = 1; recur <= max_recur; recur++)
	{
		UINT calced = 0;
		for(UINT node = _InputNum; node < node_size; node++)
		{
			// Skip if the node has been calculated
			if(tmpNodes.at(node).Val != 0)
			{
				calced++;
				continue;
			}

			isvalid = true;
			for(auto iter = tmpConnections.begin(); iter != cnct_end; iter++)
			{
				if(!(iter->Enable) || (iter->Out != tmpNodes.at(node).ID))
					continue;
				NODEVEC::const_iterator node_finder = std::find(
					tmpNodes.begin(), tmpNodes.end(), iter->In
				);
				if(node_finder->Val == 0)
				{
					isvalid = false;
					break;
				}
			}
			if(!isvalid)
				continue;
			else
			{
				tmpNodes.at(node).Val = 1.0;
				calced++;
			}
		}
		// Check if all nodes have been calculated
		bool should_exit = (calced == max_recur) ? true : false;
		if(should_exit)
			return true;
		else
			continue;
	}
	return false;
}

/**
 * @brief Add a new connection randomly by a percent
 * @param[in] _Percent The probability of adding a new connection 
*/
void Genome::AddConnection(const UINT &_Percent)
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

    if(1 + (rand() % 100) <= _Percent)
    {
		// Get the number of input nodes
		UINT cnt_input = 0;
		const UINT node_size = this->Nodes.size();
		for(UINT i = 0; i < node_size; i++)
		{
			if(this->Nodes.at(i).Type == Node::NodeType::Input)
				cnt_input++;
		}

		UINT in_node = 0;
		while(true)
		{
			in_node = rand() % node_size;
			if(this->Nodes.at(in_node).Type != Node::NodeType::Output)
				break;
		}
		Node input_node(this->Nodes.at(in_node));

		// Check if the connection exists
		auto CheckExistence = [this](const Node &_Input, const Node &_Output) -> bool
		{
			const auto iter_end = this->getConnections().end();
			for(auto iter = this->getConnections().begin(); iter != iter_end; iter++)
			{
				if(_Input.ID == iter->In && _Output.ID == iter->Out)
				{
					iter->MUTWeight(true);
					return true;
				}
			}
			return false;
		};

		// Input node CAN connect to any nodes WITHOUT checks;
		if(input_node.Type == Node::NodeType::Input)
		{
			UINT low_b = cnt_input + 1;
			UINT out_node = (rand() % (node_size - low_b + 1)) + low_b;
			Node output_node(this->Nodes.at(out_node - 1));
			if(CheckExistence(input_node, output_node))
				return;
			else
			{
				// if the connection does not exists, append it to the Connection Gene list
				Connection* p_new_connect = new Connection(input_node.ID, output_node.ID);
				this->ConnectionProcessor(p_new_connect);
				delete p_new_connect;
				p_new_connect = nullptr;
				return;
			}
		}
		else
		{
			// stores the tried-failed nodes
			int* used_i = new int[node_size - cnt_input];
			int* used_f = used_i;
			UINT low_b = cnt_input + 1;
			// loop until find a suitable output node
			while(true)
			{
				UINT out_node = (rand() % (node_size - low_b + 1)) + low_b;
				Node output_node(this->Nodes.at(out_node - 1));

				// Exclude itself
				if(input_node.ID == output_node.ID)
					continue;
				// Exclude tried-failed nodes
				bool isvalid = true;
				for(UINT i = 0; i < node_size - cnt_input; i++)
				{
					if(output_node.ID == used_i[i])
					{
						isvalid = false;
						break;
					}
				}
				if(!isvalid)
					continue;

				// Check if exist
				if(CheckExistence(input_node, output_node))
					return;
				
				// Check if is output node
				if(output_node.Type == Node::NodeType::Output)
				{
					Connection* p_new_connect = new Connection(input_node.ID, output_node.ID);
					this->ConnectionProcessor(p_new_connect);
					delete p_new_connect;
					p_new_connect = nullptr;
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
				if(this->_PreCalculation(input_node.ID, output_node.ID, cnt_input))
				{
					Connection* p_new_connect = new Connection(input_node.ID, output_node.ID);
					this->ConnectionProcessor(p_new_connect);
					delete p_new_connect;
					p_new_connect = nullptr;
					return;
				}
				else
				{
					*used_f = output_node.ID;
					used_f++;
				}
			}
			delete[] used_i;
			used_f = used_i = nullptr;
		}
    }
}

/**
 * @brief Mutate the Genome in five ways: \
 * @brief Add connection; Add node; Change weight; Random weight; Disable/Enable connection. \
 * @brief If you want to disable any type of mutation, just set the percent to 0
 * @param[in] _MutateWeight_Percent The probability of mutating a connection weight
 * @param[in] _RNGPercent The probability of mutating a weight randomly
 * @param[in] _AddNode_Percent The probability of adding a new node
 * @param[in] _HiddenMode The activation function of the new node
 * @param[in] _AddConnection_Percent The probability of adding a new connection 
*/
void Genome::Mutate(const UINT &_MutateWeight_Percent, const UINT &_RNGPercent, 
                	const UINT &_AddNode_Percent, const Node::ActFunc &_HiddenMode, 
                	const UINT &_AddConnection_Percent)
{
    this->MutateWeight(_MutateWeight_Percent, _RNGPercent);
    this->AddNode(_AddNode_Percent, _HiddenMode);
    this->AddConnection(_AddConnection_Percent);
}

/**
 * @brief Propagate (a.k.a. calculate) the output of the network based on the given input
 * @param[in] _pInputs The pointer of the input array
 * @param[in] _InputLength The size of the input array
 * @return The ordered list of outputs
*/
DBLEVEC Genome::Propagate(const double* const _pInputs, const std::size_t _InputLength) const
{
	NODEVEC tmpNodes(this->Nodes);
	const UINT node_size = tmpNodes.size();
	// Check if the input length and input nodes match
	register UINT cnt_input = 0;
	for(UINT i = 0; i < node_size; i++)
	{
		if(tmpNodes.at(i).Type == Node::NodeType::Input)
			cnt_input++;
		else
			break;
	}
	assert(_InputLength == cnt_input);

	// Place each input sequentially into each input nodes
	for(std::size_t i = 0; i < _InputLength; i++)
	{
		assert(tmpNodes.at(i).Type == Node::NodeType::Input);
		tmpNodes.at(i).Val = _pInputs[i];
	}

	DBLEVEC outputs;	// Final output
	// holds all the inputs and its corresponding weights as Pairs
	std::map<double, double> Input_Weight; 

	const UINT max_recur = node_size - cnt_input;
	const auto iter_end = this->Connections.end();
	for(UINT recur = 1; recur <= max_recur; recur++)
	{
		UINT calced = 0;
		for(UINT node = cnt_input; node < node_size; node++)
		{
			// Check if the node has been calculated
			if(tmpNodes.at(node).Val != 0.0)
			{
				calced++;
				continue;
			}
			Input_Weight.clear();
			bool isvalid = true;
			for(auto iter = this->Connections.begin(); iter != iter_end; iter++)
			{
				if(!(iter->Enable) || (iter->Out != tmpNodes.at(node).ID))
					continue;
				NODEVEC::const_iterator node_finder = std::find(
					tmpNodes.begin(), tmpNodes.end(), iter->In
				);
				if(node_finder->Val == 0)
				{
					isvalid = false;
					break;
				}
				else
					Input_Weight.insert(
						std::pair<double, double>(node_finder->Val, iter->Weight)
					);
			}
			if(!isvalid)
				continue;
			else
			{
				// Add up each weight multiply by its corresponding input. Then apply activation function.
				const auto map_iter_end = Input_Weight.end();
				register double value = 0;
				for(auto map_iter = Input_Weight.begin(); map_iter != map_iter_end; map_iter++)
					value += (map_iter->first * map_iter->second);
				tmpNodes.at(node).Val = value;
				tmpNodes.at(node).ApplyActFunc();
				calced++;
			}
		}
        // Check if all nodes have already been calculated
		bool should_exit = (calced == max_recur) ? true : false;
		if(!should_exit)
			continue;
		else
		{
			for(UINT i = cnt_input; i < node_size; i++)
			{
				if(tmpNodes.at(i).Type == Node::NodeType::Output)
					outputs.push_back(tmpNodes.at(i).Val);
				else
					break;
			}
			return outputs;
		}
	}
	return outputs;
}

/**
 * @brief Calculate the compatibility distance between two genomes
 * @param[in] _Other The other genome used to calculate compatibility distance
 * @param[in] c1 Formula param c1
 * @param[in] c2 Formula param c2
 * @param[in] c3 Formula param c3
 * @param[in] _Normalize_Threshold Should enable formula param N
 * @return The compatibility distance between two genomes
*/
double Genome::CompatDistance(const Genome &_Other, 
                          	  const double &c1, const double &c2, const double &c3, 
                          	  const UINT &_Normalize_Threshold) const
{
	CNCTVEC this_cnct(this->Connections);
	CNCTVEC other_cnct(_Other.Connections);

	UINT N = (this_cnct.size() >= other_cnct.size()) ? this_cnct.size() : other_cnct.size();
	if(N < _Normalize_Threshold)
		N = 1;

	// Sort these two vectors according to innovation numbers in accending order
	auto cmp = [&](Connection &_this, Connection &_other) -> bool{
		return (_this < _other);
	};
	std::sort(this_cnct.begin(), this_cnct.end(), cmp);
	std::sort(other_cnct.begin(), other_cnct.end(), cmp);

	// Find the number of excess genes
	auto CalcExcess = [](CNCTVEC* Ex_connect, UINT min_innov) -> UINT
	{
		UINT ExcessGenes = 0;
		auto iter = Ex_connect->rbegin();
		const auto iter_end = Ex_connect->rend();
		while(iter != iter_end)
		{
			if(iter->Innov > min_innov)
			{
				ExcessGenes++;
				Ex_connect->pop_back();
			}
			else
				break;
			iter = Ex_connect->rbegin();
		}
		return ExcessGenes;
	};
    // Only find excess genes when two innovation numbers are different
	UINT E = 0;
	if(this_cnct.rbegin()->Innov > other_cnct.rbegin()->Innov)
		E = CalcExcess(&this_cnct, other_cnct.rbegin()->Innov);
	else if(other_cnct.rbegin()->Innov > this_cnct.rbegin()->Innov)
		E = CalcExcess(&other_cnct, this_cnct.rbegin()->Innov);

    // Use the remaining lists to find the matching genes first
	double M, _total_diff = 0;
	UINT _m_num = 0;
	const auto iter_end = this_cnct.end();
	CNCTVEC::const_iterator cnct_finder;
	for(auto this_iter = this_cnct.begin(); this_iter != iter_end; this_iter++)
	{
		cnct_finder = std::find(other_cnct.begin(), other_cnct.end(), *this_iter);
		if(cnct_finder != other_cnct.end())
		{
			_m_num++;
			_total_diff += std::abs(this_iter->Weight - cnct_finder->Weight);
		}
	}
	M = _total_diff / _m_num;

    // Finally find the number of Disjoint genes
	UINT D = (this_cnct.size() - _m_num) + (other_cnct.size() - _m_num);

    // Calculate the compatibility distance
	return ((c1 * E / N) + (c2 * D / N) + (c3 * M));
}

/**
 * @brief Crossover (a.k.a. breed) between two genomes
 * @param[in] _Other The other genome to crossover
 * @param[in] this_fitness The fitness of parent 1
 * @param[in] other_fitness The fitness of parent 2
 * @return The resulted offspring
*/
Genome Genome::Crossover(const Genome &_Other, 
						 const double &this_fitness, const double &other_fitness) const
{
	CNCTVEC this_cnct(this->Connections);
	CNCTVEC other_cnct(_Other.Connections);

	auto AssignMatching = [](CNCTVEC* _New, const CNCTVEC &_Else) -> void
	{
		const auto iter_end = _New->end();
		CNCTVEC::const_iterator cnct_finder;
		for(auto iter = _New->begin(); iter != iter_end; iter++)
		{
			cnct_finder = std::find(_Else.begin(), _Else.end(), *iter);
			if(cnct_finder != _Else.end() && (1 + (rand() % 100)) <= 50)
				iter->Weight = cnct_finder->Weight;
		}
	};

    // Three circumstances: this more fit; other more fit; both equally fit
	if(this_fitness > other_fitness)
	{
		AssignMatching(&this_cnct, other_cnct);
		return (Genome(this->Nodes, this_cnct));
	}
	else if(this_fitness < other_fitness)
	{
		AssignMatching(&other_cnct, this_cnct);
		return (Genome(_Other.Nodes, other_cnct));
	}
	else	// Both genomes are equally fit
	{
		// Modify the matching genes
		AssignMatching(&this_cnct, other_cnct);
        // Merge the E/D genes from another parent
		const auto iter_end = other_cnct.end();
		CNCTVEC::const_iterator cnct_finder;
		for(auto other_iter = other_cnct.begin(); other_iter != iter_end; other_iter++)
		{
			// Check if the connection already exist
			cnct_finder = std::find(this_cnct.begin(), this_cnct.end(), *other_iter);
			if(cnct_finder == this_cnct.end())
				this_cnct.push_back(*other_iter);
		}
        // Derive the node genes from the connection genes
		NODEVEC new_nodes(this->Nodes);
		new_nodes.insert(new_nodes.end(), _Other.Nodes.begin(), _Other.Nodes.end());
		std::sort(new_nodes.begin(), new_nodes.end(), 
			[&](const Node &_this, const Node &_other) -> bool{
				return (_this < _other);
			});
		new_nodes.erase(
			std::unique(new_nodes.begin(), new_nodes.end(),
			[&](const Node &_this, const Node &_other) -> bool{
				return (_this == _other);
			}), new_nodes.end());
		// To prevent from naming different node with the same ID, delete the record from database
		const auto iter_end = this_cnct.end();
		for(auto iter = this_cnct.begin(); iter != iter_end; iter++)
		{
			if(!iter->Enable)
				continue;
			for(auto node_finder = NODE_DATABASE.begin(); node_finder != NODE_DATABASE.end(); node_finder++)
			{
				if(iter->In == node_finder->second.In && iter->Out == node_finder->second.Out)
				{
					NODE_DATABASE.erase(node_finder);
					break;
				}
			}
		}
		return (Genome(new_nodes, this_cnct));
	}
}

/**
 * @brief Calculate the adjusted fitness
 * @param[in] _Species_Num The number of organisms in the same species
*/
void Genome::CalcAdjFitness(const UINT &_Species_Num)
{
	this->Fitness /= _Species_Num;
}

/**
 * @brief Print the genotype of current genome to inspect
*/
void Genome::PrintGenotype() const
{
    std::cout << "Node Genes: " << std::endl;
    std::cout << "ID" << "\t" << "TYPE" << "\t" << "VAL" << "\t\t" << "AF" << std::endl;
	for(UINT i = 0; i < this->Nodes.size(); i++)
	{
		Node tmpNode(this->Nodes.at(i));
        std::string node_af{};
        switch (tmpNode.Type)
        {
        case Node::ActFunc::None:       node_af = "None";       break;
        case Node::ActFunc::Linear:     node_af = "Linear";     break;
        case Node::ActFunc::Sigmoid:    node_af = "Sigmoid";    break;
        case Node::ActFunc::Tanh:       node_af = "Tanh";       break;
        case Node::ActFunc::Swish:      node_af = "Swish";      break;
        case Node::ActFunc::Modified_Sigmoid:
                                        node_af = "M_Sigmoid";  break;
        default:                                                break;
        }
		std::cout << tmpNode.ID << "\t" << tmpNode.Type << "\t" << tmpNode.Val << "\t"
				  << node_af << std::endl;
	}
	std::cout << std::endl;
    std::cout << "Connection Genes: " << std::endl;
    std::cout << "INNOV" << "\t" << "IN" << "\t" << "OUT" << "\t" << "WEIGHT" << "\t\t" << "ENA" << std::endl;
	for(UINT i = 0; i < this->Connections.size(); i++)
	{
		Connection tmpCnct(this->Connections.at(i));
        std::cout << std::fixed << std::setprecision(6);
		std::cout << tmpCnct.Innov << "\t" << tmpCnct.In << "\t"
				  << tmpCnct.Out << "\t" << tmpCnct.Weight << "\t"
				  << ((tmpCnct.Enable) ? "True" : "False") << std::endl;
	}
}

NODEVEC Genome::getNodes() const
{
	return this->Nodes;
}

CNCTVEC Genome::getConnections() const
{
	return this->Connections;
}

double Genome::getFitness() const
{
    return this->Fitness;
}

void Genome::setFitness(const double _Fitness)
{
	this->Fitness = _Fitness;
}

bool Genome::operator>(const Genome &_OtherGenome) const
{
    return (this->Fitness > _OtherGenome.Fitness);
}