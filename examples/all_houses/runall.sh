#!/bin/sh

SHARPPI=${SHARPPI:-../build/sharpPI}

for mode in "sync" "bucket" "unguided"; do
    for c in A B; do
            for n in 3 6 9 12; do
                /usr/bin/time $SHARPPI -m $mode -i @I -o global_consumption "all_houses_${n}_${c}.cnf"
            done
    done
done
