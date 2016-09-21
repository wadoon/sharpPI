#!/usr/bin/python3

import sys, os
import numpy as np


class Counter(object):
    def __init__(self):
        self.value = 0
    def __call__(self, *args):
        v = self.value
        self.value +=1
        return v

def readfile(fp):
    names = {}
    pvar = {}

    cnt = Counter()

    for line in fp:
        if line[0] == '%': 
            _, typ, name, time, *v = line[1:].split(" ")         
            #pos = tuple(map(cnt, v))
            #names["%s%s" % (name,time)] = pos
            pvar.update({ int(x) : cnt() for x in v })
        else:
            break



#    order = sorted(names)
#    print(names)
    #pairs = []
    #for line in fp:                 # for each line
        # pair = []
        # for o in order:             # extract each variable
        #     value = 0
        #     for pos in names[ o ]:  # find position in line
        #         value <<= 1
        #         if line[ pos ] == '1':
        #             value |= 1     
        #     pair.append(value)
        # pairs.append( tuple( pair ) )
            

        #print(names)
    #for p in pairs: print(p)
    l = [line] + list(fp)
    return pvar, tuple(l)


def NamedMatrix(object):
    def __init__(self):
        self.rows = {}
        self.columns = {}
        self.matrix = np.zeros(1,1)

        self.cntrow = Counter()
        self.cntcol = Counter()

    def _get_row(self, name):
        try:
            return self.rows[name]
        except:
            nr = self.cntrow()
            (n,m) = self.matrix.shape
            if nr >= n:
                self.matrix = np.vstack(self.matrix, 
                                        np.zeros(10, m))
            self.rows[name] = nr
            return nr

    def _get_column(self, name):
        try:
            return self.columns[name]
        except:
            nr = self.cntcol()
            (n,m) = self.matrix.shape
            if nr >= m:
                self.matrix = np.hstack(self.matrix, 
                                        np.zeros(n, 10))
            self.columns[name] = nr
            return nr
    def __getitem__(self, addr) :
        (row,column) = addr
        r, c = self._get_row(row), self._get_column(column)
        return self.matrix[r,c] 


    def __setitem__(self, addr, val ):
        (row,column)=addr
        r, c = self._get_row(row), self._get_column(column)
        self.matrix[r,c] = val
    
def project(pairs, corpus_input, corpus_output):
    def value(p, corpus):
        i = 0
        for v in corpus:
            i <<= 1
            i |= 1 if p[v] == '1' else 0
        return i

    def ensure():
        n,m = mat.shape
        
        if row >= n:
            diff = row - n 
            return np.vstack((mat, np.zeros((diff + 1,  m))))
 
        if col >= m:
            diff = col - m
            return np.hstack((mat, np.zeros((n , diff + 1))))
        
        return mat

    mat = np.zeros((10,10), dtype=float)

    for p in pairs:
        row = value(p, corpus_input)
        col = value(p, corpus_output)

        mat = ensure()
        mat[row,col] += 1
    
    print(mat)
    return mat

# % i I 3 2
# % i I 4 4
# % i I 5 6
# % i I 6 8
# % i I 7 10
# % i I 8 12
# % i I 9 14
# % i I 10 16
# % i I 11 18
# % i I 12 20
# % i I 13 22
# % i I 14 24
# % o global_consumption 26 297 298 299 300 301 302 303 304 305 306 307 308


with open(sys.argv[1]) as fp:
    pvars, lines = readfile(fp)
    print(pvars)
    pI = list(range(2,25,2))
    pO = list(range(297,309))
    I = [ pvars[x] for x in pI ]
    O = [ pvars[x] for x in pO ]

    print("Selected Input: ", I)
    print("Selected Output: ", O)

    matrix = project(lines,I,O) 

    print(sum(sum(matrix)))
    print(len(lines))
