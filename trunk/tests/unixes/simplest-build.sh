# On typical Linux, use this: (comment / uncomment as required)
#g++ -O2 -W -Wall -DIBPP_LINUX ../tests.cpp ../../core/all_at_once.cpp -lfbclient -lcrypt -lm -lpthread -o tests

# On Darwin, use this: (comment / uncomment as required)
g++ -O2 -W -Wall -DIBPP_DARWIN ../tests.cpp ../../core/all_at_once.cpp -framework Firebird -lm -lpthread -o tests
