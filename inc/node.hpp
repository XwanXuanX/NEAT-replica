#ifndef NODE_H
#define NODE_H

#include "preh.hpp"
#include "utility.hpp"

struct Node
{
    enum ActFunc {None, Linear, Sigmoid, Tanh, ReLU, Swish, Modified_Sigmoid};
    enum NodeType {Input, Output, Hidden};

    // member variables
    UINT ID;
    NodeType Type;
    double Val;
    ActFunc AF;

    Node(const UINT _ID, const NodeType _Type, const ActFunc _AF);

    // activation functions
    static double act_Linear(const double x);
    static double act_Sigmoid(const double x);
    static double act_Tanh(const double x);
    static double act_ReLU(const double x);
    static double act_Swish(const double x);
    static double act_Modified_Sigmoid(const double x);

    // apply activation functions
    void ApplyActFunc() noexcept;

    // overload operator functions
    bool operator==(const UINT &_NodeID) const;
    bool operator==(const Node &_OtherNode) const;
    bool operator<(const Node &_OtherNode) const;
};

#endif