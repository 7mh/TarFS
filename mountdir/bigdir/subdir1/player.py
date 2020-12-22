
import sys

winnerlist = [
 (0,1,2), (3,4,5), (6,7,8),
 (0,3,6), (1,4,7), (2,5,8),
 (0,4,8), (2,4,6)
]

valuelist = [1,10,100]

class Player:

   def __init__(self,board,playerno):
      self.whoami = playerno
      self.opponent = 3 - playerno

   def getmove(self,board):

# first find value of blocking each winning triple
      hit = [False]*len(winnerlist)
      value = [0]*len(winnerlist)
      for i in range(len(winnerlist)):
         score = 0
         w = winnerlist[i]
         oppocount = 0
         for j in range(3):
            if board[w[j]] == self.whoami:
               hit[i] = True
               break
            if board[w[j]] == self.opponent:
               oppocount += 1
         if hit[i]:
            value[i] = 0
         elif oppocount == 3:
            print('game should be over:',board)
            sys.exit(0)
         else:
            value[i] = valuelist[oppocount]

      movelist = [x for x in range(9) if board[x] == 0]
      best = -1
      move = -1
      for m in movelist:
         if board[m] != 0:
            continue
         tot = 0
         for i in range(len(winnerlist)):
            if not hit[i]:
               if m in winnerlist[i]:
                  tot += value[i]
         print('value of',m,'is',tot)
         if tot > best:
            best = tot
            move = m
      print('choosing',move)
      return move

