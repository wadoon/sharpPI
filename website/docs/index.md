---
title: sharpPI -- Version 1.0
date: 2016-09-18
author: Alexander Weigl
---

# sharpPI

**About.** sharpPI is a tool for quantified information flow analysis with
support **Shannon Entropy** based on the SAT-based approach of
[Vladimir Klebanov](http://formal.iti.kit.edu/~klebanov).

QIF with sharpPI has following features:

* applicable for the C language
* support Shannon entropy (counts images and pre-images)
* deterministic and non-deterministic programs
* bounds calculation

**Download.**

* You can obtain the latest development from
  [github repository](https://github.com/wadoon/sharpPI).
* The [release of the QASA'16](sharpPI-qasa2016.tar.gz).
    * There are several bugs in generating the statistic.
    * You need to set `--verbose` to get the leakage information.
    * glucose is missing in the tarball.

## QIF Analysis with sharpPI

In this section we explain how you should proceed to work with sharpPI.
In general following steps are necessary:

1. Extract the part to be measured part of your program into a separate C file.
It makes sense to have a function, that takes the secret and returns the
observable output.

2. Write a `main` method that uses declares uninitialized variable (secret
   information) and the output value. Typically, this stub looks like this:

        #define N 2
        int main(int argc, char* argv[]) {
          char secret[ N ];
          for(int i = 0; i < N; i++) { I = secret[i]; };
          int output = function_under_test( secret );
          assert(0);
          return 0;
        }
   The `assert(0);` statement is always false, in this way *cbmc* creates an
   dimacs file modelling this function. The necessity of for-loop is explained
   later.

3. Translate you program into an CNF formula. For this you should obtain
   [cbmc](http://cprover.org/cbmc). Call *cbmc* with `dimcas` and `outfile`
   options.

         cbmc -D N=2 --dimacs --outfile main.cnf main.c
   With `-D N=2` we can give definitions to the pre-processor.


4. Run sharpPI on the CNF file with right secret and output variable.

        sharpPI -i @I -o O main.cnf
   We need the for-loop over the secret to get on the propositional
   variables, that encode the secret information.




## Operation Modes

Currently sharpPI supports following counting modes `-m`:


unguided
: Counting every secret/output pair.

bucket
: First, we try to find an output value, and then we count all the pre-images.

sync
: First, we try to find **all** output values, and then we successively count one input value for each output value.

rand
: Like *bucket* but support for randomness primitive.

dsharp
: Like *bucket*, but the pre-images are counted with an external #SAT-p program
    (currently `dsharp`).


The first three counting modes are presented in the QASA'16 paper.


## Variable notation

You have seen, that we specified the output value with `O` and the secret value
with `@I` in the example above.

The background is that *cbmc* decodes the variables in *single static assignment
form*. So every state of a variable is built up of different set of
propositional variables. If you we just specify an variable name (without
leading `@`) we select the last set propositional variables. With leading `@` we
select all prop. variables as corresponding.

**tl;dr;** If the secret is an array, apply the pattern above and use `@`; else
just the variable name.

## Non-determinism

**Note:** The support of non-deterministic programs is at a preliminary level.

For programs with randomness primitives, you need to specify the variables, that
are random). Use the `--seed` option to specify these. Additionally, the
variable should free (without constraints) such as the *secret*.
