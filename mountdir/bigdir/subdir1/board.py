#!/usr/bin/env python3

import sys
from tkinter import *
from player import Player

winnerlist = [
 (0,1,2), (3,4,5), (6,7,8),
 (0,3,6), (1,4,7), (2,5,8),
 (0,4,8), (2,4,6)
]

colorlist = ["#222","#c22","#22c"]    # gray, red, blue

messagelist = ["","Red wins","Blue wins","Tie game"]

class TTTBoard:

  def __init__(self,win):

    self.board = [0]*9
    self.autoplayer = Player(self.board,2)

    win.option_add("*font",("Helvetica",24))       # font for the message label
    win.title("Tic-Tac-Toe")

    boardframe = Frame(win)
    boardframe.pack(side='top')
    self.boardframe = boardframe

    flist = []                      # Make the frames (one per column of three squares)
    for i in range(3):
      flist.append(Frame(boardframe))   # make a frame, append it to list
      flist[i].pack(side='left')        # pack it into the board
    self.framelist = flist          # framelist is the list of frames

    lab = Label(win,bg="#222",fg="#f0f")  # Make the label for messages
    lab.pack(side='bottom',fill=BOTH)
    self.lab = lab

    blist = []                            # Make the buttons (actually labels), one per square
    for i in range(9):
      but = Label(flist[i % 3],text=str(i),width=5,height=2,relief=RAISED,fg="#ff0")
      but.pos = i
      but.pack(side="bottom")                 # pack buttons on bottom of label
      but.bind("<Button-1>", self.callback)   # specify function to call when the label is clicked
      blist.append(but)
    self.buttonlist = blist               # buttonlist is the list of buttons

    self.reset()                # reset for a new (the first) game

  # end of __init__

  def reset(self):                                   # reset board for a new game
    for i in range(9):                               # recolor board
      self.buttonlist[i].configure(bg=colorlist[0])
    self.status = 0
    self.movecount = 0
    self.board = [0]*9
    self.lab.configure(text="")
    self.turn = 1

  def checkwinner(self,who):
    for w in winnerlist:
      x = self.board[w[0]]
      if x == 0:
        continue
      if self.board[w[1]] == x and self.board[w[2]] == x:
        return x
    return 0

  def makemove(self, who, where):                        # update board, etc. for a move
    self.buttonlist[where].configure(bg=colorlist[who])
    self.board[where] = who
    self.movecount += 1
    if self.checkwinner(who) == who:                     # if the move wins the game
      self.lab.configure(text = messagelist[who])        # show result on the label
      self.status = who                                  # change status to indicate a winner
    elif self.movecount == 9:                            # if the move fills the board (tie)
      self.lab.configure(text = messagelist[3])          # show result on label
      self.status = 3                                    # change status to indicate tie
    
  def callback(self,event):  # respond to click (human move)
    if self.status != 0:      # if game is over, a click resets things for a new game
      self.reset()
      return
    k = event.widget.pos             # which square was clicked
    self.makemove(1, k) 
    if self.status != 0:
      return
    k = self.autoplayer.getmove(self.board)
    if self.board[k] != 0:
      print('illegal move:',k,self.board)
      sys.exit(0)
    self.makemove(2, k) 

if __name__ == "__main__":

  win = Tk()
  game = TTTBoard(win)
  win.mainloop()

