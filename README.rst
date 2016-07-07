=======
sharpPI
=======

.. note:: 
   This page is under construction (2016-07-06) for the QASA paper. 
   Reconstruction will be finished at begin of August.


:Author: Alexander Weigl <weigl@kit.edu>
:Subtitle: fdasfdasf
:Date:   2016-06-15
:Copyright: GPL version 3
:Version: 0.10.0
:Manual section: 1
:Manual group: text processing

SYNOPSIS
========

    sharpPI [-h|--help] {variables} filename


DESCRIPTION
===========

afdasfldasfkldjasfkldjasflkdjasf dasfdasf dasfdas fdasfdasf.
dasfdasfkldjasflkdasfdas
fdasfdas
fdasf

General
-------

--help, -h                 Outputs a help message

--verbose, -v              Activates the verbose, found input and output values are printed to the terminal.

--limit, -l INTEGER  Limits the the count of runs within the iterations modes. For support of the modes see below.

--mode M, -m M  Sets the operation mode `M`, see below for more information about the modes below.

--input V, -i V  Specifies an input variable. Multiple occurences possible.

--output V, -o V  Specifies an output variable. Multiple occurences possible.

--statistic FILE  Write a csv file with current values in each turn.


Non-determinism
----------------

For non-determinstic programs the set of random variables can be given by **--seed**

--seed V, -s V  Specifies a seed (random) variable. Multiple occurences possible.


Variable Naming
===============

**TODO**: Explain @ and normal variables


Operation Modes
===============


0. Determinstic (output->input relation)

1. Determinstic ITERATIVE (each output, limitation possible)

2. Determinstic SHUFFLE (find each input/output pair, limitation possible)

3. Determinstic SUCCESSIVE ()

4. Non-Determinstic


Getting Started
===============

1. Additional to `make` and `gcc`, you need to install `cmake` and development packages for `boost` `eigen3` and `zlib`.  On Fedora 23 use::

     sudo dnf install cmake eigen3-devel boost-devel zlib-devel

2. Ensure each submodule is checkout out::

     git submodule init
     git submodule update

3. Build::

     mdkir build; cd build
     cmake -DCMAKE_BUILD_TYPE=Release ..
     make -j 4

The `sharpPI` is in `build/`.


Problems
========

Changelog
=========

See also
========

* `Minisat <>`_
* `Glucose <>`_
* `dsharp <>`_
* `clasp <>`_

Copyright
=========

sharpPI is under GNU GPL v3::

    (c) 2015 Alexander Weigl <weigl@kit.edu>
        Application-Oriented Formal Verification
        Institute for Theoretical Informatics
        Karlsruhe Institute for Technology (KIT)
