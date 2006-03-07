# On typical Linux, use this: (comment / uncomment as required)
#g++ -W -Wall -DIBPP_LINUX ../tests.cpp ../../core/*.cpp -lfbclient -lcrypt -lm -lpthread

# On Darwin, use this: (comment / uncomment as required)
g++ -W -Wall -DIBPP_DARWIN ../tests.cpp ../../core/*.cpp -framework Firebird -lm -lpthread -o tests
