# INET-3101---Module-4-Assignment

# Problem Statement & Persistence Design:
  Problem: The original program would make it so reservations are lost when it's closed, so now it was made to save and load them back each time you run.
  Formation: It's made so the seat arrays are written straight to flight_data.bin with fwrite and read back with fread.
  Startup: The program will check for the file and if it's there it will load the data, if it's missing both flights will start with empty seats and if it's corrupted the program will tell you and start a fresh one. When you quit with option c it will save both flights before the program exits.
  Layout: 24 outbound seats, 24 inbound seats, each seat stores its seat number and if it's taken and what the passenger's first and last name is.

# File Validation & Error Recovery Analysis:
  Before it runs the file, the program will check its size, makes sure every read got all its data, and verifies the label, seat numbers, taken/empty values, and names. Data will then load into a temporary copy first and only replaces the real seats if every check passes.

# Pros & Cons of Solution:
  Binary is short and fast and easy to check for damage, but you can't read the file yourself and it might not work on another computer. Text/CSV is readable and portable but needs more code to read.

# AI Fuzzing Reflection:
  I asked the AI to make a program that creates broken saved files like ones cut short or full of junk or with bad names and seat numbers. Afterwards when I loaded each one, my program caught the problem and started fresh instead of crashing. Without these checks in place the bad files could have shown fake passengers or printed garbage or crashed my program. 
  
