//2016.10.13
//qiangwei.su
//

#include <iostream>

#include <stdio.h>
#include <string>

int main(int argc, char* argv[])
{
	std::string line;
	while(getline(std::cin, line))
	{
		printf("%s\n", line.c_str());
	}
	//getline(std::cin, line);
	return 0;
}


