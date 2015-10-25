=======
sharpPI
=======


:Author: Alexander Weigl <weigl@kit.edu>
:Subtitle: fdasfdasf
:Date:   2015-10-25
:Copyright: GPL version 3
:Version: 0.7
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

Non-determinism
----------------

For non-determinstic programs the set of random variables can be given by **--seed**

--seed V, -s V  Specifies a seed (random) variable. Multiple occurences possible.


Variable Naming
===============



Operation Modes
===============


0. Determinstic (output->input relation)

1. Determinstic ITERATIVE (each output, limitation possible)

2. Determinstic SHUFFLE (find each input/output pair, limitation possible)

3. Determinstic SUCCESSIVE ()

4. Non-Determinstic


PROBLEMS
========

SEE ALSO
========

BUGS
====
