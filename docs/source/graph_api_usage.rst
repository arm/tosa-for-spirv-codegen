..
   SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its affiliates <open-source-office@arm.com>
   SPDX-License-Identifier: Apache-2.0

.. _graph-api-usage:

Graph API Usage
===============

.. include:: ../generated/README.md
    :parser: myst_parser.sphinx_
    :start-after: ## Graph API Usage
    :end-before: ## Doxygen & Sphinx Documentation

Graph API Reference
-------------------

.. doxygenfunction:: tfsc::tosa::Graph::AddInput
   :project: TFSC

.. doxygenclass:: tfsc::tosa::Attribute
   :members:
   :project: TFSC

.. doxygenclass:: tfsc::tosa::Tensor
   :members:
   :project: TFSC

.. doxygenfunction:: tfsc::tosa::Graph::AddOutput
   :project: TFSC

.. doxygenfunction:: tfsc::tosa::Graph::AddMaxPool2dOperator
   :project: TFSC

.. doxygenfunction:: tfsc::tosa::Graph::FinalizeGraph
   :project: TFSC