# Seam Carving Portable Gray Maps( pgm )
Uses dynamic programming to erase a specified number of verticle   
and horizontal seams of based on cummulative energy. Requires a  
pgm viewer to see results.   
   
To Compile: g++ seamCarve.cpp   
To Run: ./seamCarve.o [ filename ] [ vert ] [ hori ]   
Will remove [ vert ] verticle seams, and [ hori ] horizontal  
seams from [ filename ] and generate [ filename ]_processed.pgm   
