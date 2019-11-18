dependencyDiscoverer: dependencyDiscoverer.cpp
	clang++ -Wall -Werror -g -std=c++14 -o dependencyDiscoverer dependencyDiscoverer.cpp -lpthread

clean:
	rm -f *.o dependencyDiscoverer *~
