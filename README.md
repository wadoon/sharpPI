# sharpPI 

sharpPI is a quantified information flow analysis (*QIF*) tool supporting Shannon Entropy based on the SAT-based toolchain. 


## Getting Started

1. Additional to `make` and `gcc`, you need to install `cmake` and development
   packages for `boost` `eigen3` and `zlib`. On Fedora use
```
     sudo dnf install cmake eigen3-devel boost-devel zlib-devel
``` 
2. Ensure each submodule is checkout out
```
     git submodule init
     git submodule update
```
3. Build
```
     mdkir build; cd build
     cmake -DCMAKE_BUILD_TYPE=Release ..
     make -j 4
```

The `sharpPI` and `sharpPIg` are  in `build/` 
(Minisat resp. Glucose as SAT solver).


## Citation

```
@inproceedings{Weigl16, 
	author={Alexander Weigl}, 
	title={Efficient SAT-based Pre-image Enumeration for Quantitative Information Flow in Programs}
	note={to be appear}
}

@proceedings{QASA2016, 
	title={Data Privacy Management and Security Assurance},
	year=2016,
	editor={Giovanni Livraga and Vicenç Torra and Alessandro Aldini and Fabio Martinelli and Neeraj Suri},
        doi={10.1007/978-3-319-47072-6},
	publisher={Springer},
	number = {9963}, 
	series = {LNCS}
}
``` 

## Problems

If you have found an error, feel free to open an issues. 

## Copyright

sharpPI is under GNU GPL v3. 

``` 
(c) 2015, 2016 Alexander Weigl <weigl@kit.edu>
    Application-Oriented Formal Verification
    Institute for Theoretical Informatics
    Karlsruhe Institute for Technology (KIT)
```
