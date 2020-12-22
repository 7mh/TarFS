#!/usr/bin/env python3

winlist = ((0,1,2),(3,4,5),(6,7,8),(0,3,6),(1,4,7),(2,5,8),(0,4,8),(2,4,6))

count = 0
ht    = {}

def no_winner(board, who):
   global winlist

   for w in winlist:
      if all([board[x] == who for x in w]):
         return False
   return True

def recurse(depth, board, who):
   global ht, fd

   movelist = [x for x in range(9) if board[x] == 0]
   for move in movelist:
      board[move] = who
      key = str(board)
      if key not in ht:
         ht[key] = True
         fd.write(key + '\n')
         if depth < 9 and no_winner(board,who):
            recurse(depth+1, board, 3 - who)
      board[move] = 0

if __name__ == "__main__":

   board = [0]*9
   key = str(board)
   fd = open('ttt.data','w')
   fd.write(key + '\n')
   ht[key] = True
   recurse(1, board, 1)
   print(len(ht))
