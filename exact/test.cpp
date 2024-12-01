#include <iostream>

int main() {
	const int ranks = 10;
	int counts[ranks] =  {4,4,4,4,4,4,4,4,4,16};
	std::string names[ranks] = {"ace","two","three","four","five","six","seven", "eight","nine","ten"};
	for (int i = 0; i < ranks; ++i){
		std::cout << names[i] << ": " << counts[i] << std::endl;
	}
	return 0;
}
