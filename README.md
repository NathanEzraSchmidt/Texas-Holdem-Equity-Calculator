# Texas-Holdem-Equity-Calculator
Very fast equity calculator for Texas Holdem

This is a console app, written in C. Instructions on how to use it are very simple and appear on the screen when the program launches. After the user has specified the hole cards for up to 16 players, it gives the exact winning and all tying frequencies, both in combinations and in percentages, for each player. It's very fast because it only searches through all distinct combinations of board cards ignoring suits, then uses combinatorics to weight each distribution and account for flushes and straight flushes. 
