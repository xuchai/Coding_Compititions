HOMEWORK 6: CARCASSONNE RECURSION


NAME:  < Xu Chai >



COLLABORATORS AND OTHER RESOURCES:
List the names of everyone you talked to about this assignment
(classmates, TAs, ALAC tutors, upperclassmen, students/instructor via
LMS, etc.), and all of the resources (books, online reference
material, etc.) you consulted in completing this assignment.

< Taoran Li, Aritra B, Steven/ google, cplusplus >

Remember: Your implementation for this assignment must be done on your
own, as described in "Academic Integrity for Homework" handout.



ESTIMATE OF # OF HOURS SPENT ON THIS ASSIGNMENT:  < 35 >



ANALYSIS OF PERFORMANCE OF YOUR ALGORITHM:
(order notation & concise paragraph, < 200 words)
The dimensions of the board (h and w)? 
The number of tiles (t)? 
The number of road (r) and city (c) edges? 
The number of duplicate tiles?
Whether rotations are allowed?  
Etc. 

My code structure is basically as follows:
In the main function, I have two situations: The first one is base case(just find one solution), the other is all_rotations
or all_solutions. In each situation, I have a recursion function called “Can_place”, which is used to check whether all tiles 
can be placed. In the recursion function, I have two check functions: the first one (Chech_the_whole_board) is used for checking
the whole layout of the board after all the tiles have been used up, the second one(Check_tile) is used for checking the requirements
of the current tile. Then, after the recursion function, I have a “ check_duplicate code “, which is used for checking whether two 
solutions are the same. The order notations of each part are as follows:

Order notations:
———————————————————————————————
Check_the_whole_board: O(h*w)
Check_tile: O(1)
Can_place(recursion): if not allow_rotations: O((h*w)^(2t))
                      if allow_rotations: O((4*h*w)^(2t))
The total order notation of the program:if not allow_rotations: O((total_solutions*t^2)*(h*w)^(2t))
                                        if allow_rotations: O((total_solutions*t^2)*(4*h*w)^(2t))

SUMMARY OF PERFORMANCE OF YOUR PROGRAM ON THE PROVIDED PUZZLES:
# of solutions & approximate wall clock running time for different
puzzles with various command line arguments.
If not allow_rotations:
PUZZLEs          num_solutions
puzzle6.txt        4
puzzle9.txt        9
All the puzzles can be run within 1 minutes even thought the board is very big. Expect that:
In my code, I can run out all the correct solutions of puzzle8 and puzzle9 with rotations. but there is two problems:
(1) There are duplicate solutions, I’m not able to check the duplicate solutions perfectly after rotations.

MISC. COMMENTS TO GRADER:  
Thank you for your grading!!
(optional, please be concise!)


