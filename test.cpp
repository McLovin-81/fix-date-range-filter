#include <iostream>
#include <fstream>
#include <string>


int main()
{
	std::string testString = "Hello";
	const std::string& startDate = "20240620";
	std::ifstream file("test.sfd", std::ios::binary);
	std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	size_t pos = 0;
    const char SOH = '\x01';

	size_t startPos = buffer.find("8=FIX");
	size_t endPos = buffer.find(SOH + "10=", startPos);
	endPos = buffer.find(SOH, endPos + 1);
	// endPos += 5;

	std::string subStr = buffer.substr(startPos, endPos - startPos);
	
	std::cout << subStr << std::endl;


	return 0;
}
