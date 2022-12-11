// The MIT License (MIT)

// Copyright (c) 2022 Yetong (Tony) Li

//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.

#ifndef NODE_H
#define NODE_H

#include <cmath>
#include <iostream>

struct Node
{
    typedef unsigned int uint;
    enum class ActFunc : uint {
        None, 
        Linear, 
        Sigmoid, 
        Tanh, 
        ReLU, 
        Swish, 
        Modified_Sigmoid
    };
    enum class NodeType : uint {
        Input, 
        Output, 
        Hidden
    };

    // member vars
    uint        m_ID;
    NodeType    m_type;
    double      m_val;
    ActFunc     m_func;

    // special ctors
    Node() = default;
    Node(const Node& __other) noexcept;
    Node& operator=(const Node& __other) noexcept;
    // user-defined ctor
    Node(const uint __ID, 
         const NodeType __type, 
         const ActFunc __func) noexcept;

    // member functions
    void ApplyActFunc() noexcept;

    // op reload
    bool operator==(const uint __other) const noexcept;
    friend bool operator==(const Node& __lhs, const Node& __rhs) noexcept;
    friend bool operator<(const Node& __lhs, const Node& __rhs) noexcept;
};

#endif