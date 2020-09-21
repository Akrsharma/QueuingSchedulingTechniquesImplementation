Below are the steps to run Question2 of the assignment - 

1. There is a make file for this program. What you have to do to run this program is to type "make -f Makefile" in the terminal.
   
    A file are created in the same directory where you have run the make file - 

   outputfile.txt: This file contains the below structure - 
   N        P       Queue Type        Average Packet Delay      Standard Deviation of Packet Delay    Average Link Utilization
   
   
2. Default command line arguments are
    Total No of input and output port in a switch = 8
    Buffer size at each input and output port = 4
    Packet Generation Probability = 0.5
    Queue Type  = INQ
    Output buffer size = 4 in case of KOUQ algorithm(irrelevant in case of INQ and ISLIP but has to be specified)
    output file name = outputfile
    Maximum Time Slots = 10000

3. You can change Default arguments in "Makefile" (in the same directory)
