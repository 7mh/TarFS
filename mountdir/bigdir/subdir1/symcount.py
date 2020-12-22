#!/usr/bin/env python3

from canonize import canonize

BSIZE = 9

count = 0

winnerlist = ((0,1,2),(3,4,5),(6,7,8),(0,3,6),(1,4,7),(2,5,8),(0,4,8),(2,4,6))

def iswinner(board, who):
   for win in winnerlist:
      if all([board[x] == who for x in win]):
         return True
   return False

def game_over(board, who):
   if iswinner(board,who):
      return True
   if 0 in board:
      return False
   return True

def recurse(board, who):  # who = 1 or 2
   global htab, count

   movelist = [x for x in range(BSIZE) if board[x] == 0]
   for m in movelist:
      board[m] = who
      canon = canonize(board)
      key = str(canon)
      if key not in htab:
         count += 1
         htab[key] = float(who % 2) if iswinner(board,who) else 0.5
         if not game_over(board,who):
            recurse(board, 3 - who)     # hack
      board[m] = 0

if __name__ == "__main__":

   board = [0]*BSIZE
   key = str(board)
   htab = {key : 0.5}
   recurse(board, 1)   # players are 1 and 2
   print(len(htab))
   fd = open('sym.data','w')
   for key in htab:
      fd.write("{0:s} {1:4.1f}\n".format(key,htab[key]))
   fd.close()

