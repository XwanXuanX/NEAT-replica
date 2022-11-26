#include "../inc/node.hpp"

Node::Node(const UINT _ID, const NodeType _Type, const ActFunc _AF)
    : ID(_ID), Type(_Type), AF(_AF), Val(0.0) {}

// Activation function list
inline double Node::act_Linear(const double x) { 
    return x; 
}

inline double Node::act_Sigmoid(const double x) { 
    return (1.0 / (std::exp(-x) + 1.0)); 
}

inline double Node::act_Tanh(const double x) { 
    return std::tanh(x); 
}

inline double Node::act_ReLU(const double x) { 
    return (x >= 0 ? x : 0); 
}

inline double Node::act_Swish(const double x) { 
    return (x * Node::act_Sigmoid(x)); 
}

inline double Node::act_Modified_Sigmoid(const double x) { 
    return (1.0 / (std::exp(-4.9 * x) + 1.0)); 
}

// Apply activation function to current node
void Node::ApplyActFunc() noexcept
{
    switch (this->AF)
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

// overload operator
bool Node::operator==(const UINT &_NodeID) const {
     return (this->ID == _NodeID); 
}

bool Node::operator==(const Node &_OtherNode) const { 
    return (this->ID == _OtherNode.ID); 
}

bool Node::operator<(const Node &_OtherNode) const { 
    return (this->ID < _OtherNode.ID); 
}