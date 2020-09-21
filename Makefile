# the compiler to use
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall
  
# the name to use for both the target source file, and the output file:
TARGET = Schedular
file = FinalCode


all:
	$(CC) $(file).cpp -o $(TARGET)
	./$(TARGET) -N 8 −B 4 −p 0.5 −queue KOUQ −K 4 −out outputfile −T 10000


# -N = Representing Total No of input and output port in a switch
# -B = Representing Buffer size at each input and output port
# -p = Representing Packet Generation Probability
# -queue = Representing Queue Type INQ | KOUQ | ISLIP
# -K = Representing Output buffer size in case of KOUQ algorithm(irrelevant in case of INQ and ISLIP but has to be specified)
# -out = Represents output file name
# -T  = Representing Maximum Time Slots
