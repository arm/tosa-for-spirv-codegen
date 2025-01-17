//
// Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

namespace tosa2spirv
{

namespace graphbuilder
{

/// Forward declarations
class Module;
class Graph;

/// Abstract Visitor class allowing Module introspection
class IVisitor
{
public:
    /// Graph Visit function, used to walk to the Graph and perform graph specific logic
    /// @param[in] graph The graph which will be used to update the Visitor
    virtual void Visit(const Graph& graph) = 0;

    /// Module Visit function, called after Graph visit functions and adds information about the Module
    /// @param[in] module The module which will be used to update the Visitor
    virtual void Visit(const Module& module) = 0;
};

/// Abstract Visitable interface providing Accept function to Module object hierarchy
class IVisitable
{
public:

    /// Accept function for a visitor. Which will be updated with info about the object which Accepts it
    /// @param[in] visitor The visitor to be Accepted and updated as needed
    virtual void Accept(IVisitor& visitor) const = 0;

    /// Default Destructor
    virtual ~IVisitable() = default;
};

} // namespace graphbuilder

} // namespace tosa2spirv