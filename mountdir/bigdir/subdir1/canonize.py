#!/usr/bin/env python3

import numpy as np

#
# Finds canonical board equivalent to a given board.
#
# Numpy is hidden from calling module:
#
#     input:  Python list
#     output: Python list
#

def canonize(b):

   x  = np.array(b).reshape(3,3)
   li = []
   for i in range(4):
      li.append(x.flatten().tolist())
      li.append(np.transpose(x).flatten().tolist())
      x = np.rot90(x)

   return min(li)

if __name__ == '__main__':

   print(canonize([1,2,0, 0,1,0, 0,0,0]))

