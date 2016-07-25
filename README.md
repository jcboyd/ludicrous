ludicrous
============

A chess engine implemented for Bachelor semester project in 2010

<pre>
Commands:
    new                     start new game
    flip                    change sides
    display                 display board position
    reveal                  display evaluations and principal variations
    set [fen]               set position with fen notation
    analyse                 set infinite search depth
    blitz                   limit think time
    {a-h}{1-8}{a-h}{1-8}    play move
    author                  dissplay author details
    help                    display help menu
    exit                    exit
> reveal
> e2e4
    t3.2e-05 : d1 : 0 : d7d5 
    t0.000254 : d2 : 0.2 : d7d5 d2d4 
    t0.000621 : d3 : 0 : d7d5 f1b5 b8c6 
    t0.00295 : d4 : 0.15 : e7e5 d2d4 d7d5 c2c4 
    t0.007466 : d5 : 0.1 : e7e5 b1c3 a7a5 d2d4 d7d5 
    t0.025102 : d6 : 0.08 : e7e5 c2c4 c7c5 a2a4 f8d6 d2d4 
    t0.210112 : d7 : 0.05 : d7d5 b1c3 d5e4 c3e4 e7e5 c2c4 c7c5 
    t0.460757 : d8 : 0.1 : d7d5 e4e5 c7c5 d2d4 d8b6 d4c5 b6c5 c2c4 
    t1.16315 : d9 : 0.05 : d7d5 e4d5 d8d5 b1c3 d5d6 d2d3 d6b4 f2f4 e7e5 
    t3.34645 : d10 : 0.05 : d7d5 e4e5 c7c5 c2c4 b8c6 f2f4 c8e6 c4d5 e6d5 d2d4 
> display
    + - + - + - + - + - + - + - + - +
    | r | n | b | q | k | b | n | r | 
    + - + - + - + - + - + - + - + - +
    | p | p | p |   | p | p | p | p | 
    + - + - + - + - + - + - + - + - +
    |   |   |   |   |   |   |   |   | 
    + - + - + - + - + - + - + - + - +
    |   |   |   | p |   |   |   |   | 
    + - + - + - + - + - + - + - + - +
    |   |   |   |   | P |   |   |   | 
    + - + - + - + - + - + - + - + - +
    |   |   |   |   |   |   |   |   | 
    + - + - + - + - + - + - + - + - +
    | P | P | P | P |   | P | P | P | 
    + - + - + - + - + - + - + - + - +
    | R | N | B | Q | K | B | N | R | 
    + - + - + - + - + - + - + - + - +
> 
</pre>
