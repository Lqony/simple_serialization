#include <iostream>
#include "serializer.h"

struct test_pod
{
	char msg[10];
	uint32_t num;
};

int main(int argc, char **argv)
{
	std::string msg("Test message");
	size_t num = 111999333444;
	test_pod tpod;

	std::string s1("Hello Pod");
	memcpy(tpod.msg, s1.data(), sizeof(tpod.msg));
	tpod.num = 13;

	serializer s;

	s.store_int(num);
	s.store_bytes(msg.data(), msg.size());
	s.store_pod(tpod);

	
	std::string msg2;
	msg2.resize(msg.size());
	size_t num2;
	test_pod tpod2;

	num2 = s.load_int();	
	s.load_bytes(const_cast<char*>(msg2.data()), msg.size());
	tpod2 = s.load_pod<test_pod>();

	std::cout << msg2 << " " << num2 << std::endl;
	std::cout << tpod2.msg << " " << tpod2.num << std::endl;
	return 0;
}
