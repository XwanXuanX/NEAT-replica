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

#include "node.hpp"

// copy constructor
Node::Node(const Node& __other) noexcept
    : m_ID(__other.m_ID), 
    m_type(__other.m_type), 
    m_val(0.0),
    m_func(__other.m_func) {}

// copy assignment op
Node& Node::operator=(const Node& __other) noexcept
{
    this->m_ID = __other.m_ID;
    this->m_type = __other.m_type;
    this->m_val = 0;
    this->m_func = __other.m_func;
    return *this;
}

// user-defined ctor
Node::Node(const uint __ID, const NodeType __type, const ActFunc __func) noexcept
    : m_ID(__ID), 
    m_type(__type), 
    m_val(0.0), 
    m_func(__func) {}

// member functions
void Node::ApplyActFunc() noexcept
try
{
    switch(this->m_func)
    {
        case ActFunc::None:
        case ActFunc::Linear:
            break;

        case ActFunc::Sigmoid:
            this->m_val = (1.0 / (std::exp(-this->m_val) + 1.0));
            break;

        case ActFunc::Tanh:
            this->m_val = std::tanh(this->m_val);
            break;

        case ActFunc::ReLU:
            this->m_val = (this->m_val >= 0 ? this->m_val : 0);
            break;

        case ActFunc::Swish:
            this->m_val = (this->m_val * (1.0 / (std::exp(-this->m_val) + 1.0)));
            break;

        case ActFunc::Modified_Sigmoid:
            this->m_val = (1.0 / (std::exp(-4.9 * this->m_val) + 1.0));
            break;
        
        default:
            throw std::range_error("Unknown activation function... \n");
            break;
    }
}
catch(const std::range_error& e)
{
    std::cout << "File: " << __FILE__ << '\n'
              << "Line: " << __LINE__ << '\n'
              << e.what() << '\n';
    throw;
}

// operator reload
inline bool Node::operator==(const uint __other) const noexcept
{
    return this->m_ID == __other;
}

inline bool operator==(const Node& __lhs, const Node& __rhs) noexcept
{
    return __lhs.m_ID == __rhs.m_ID;
}

inline bool operator<(const Node& __lhs, const Node& __rhs) noexcept
{
    return __lhs.m_ID < __rhs.m_ID;
}