
#include "stdafx.h"

#include <iostream>

#include "xttp.h"
#include "xttp_struct.h"

int main(int argc, char *argv[])
{
	xttp_rs rs = xttp().url("http://www.baidu.com").get();
	if (rs.state()){
		std::cout << rs.result();
	}else{
		std::cout << "fail:" << rs.result();
	}
	return 1;
}