
#include "stdafx.h"

#include <iostream>

#include "hxxp.h"
#include "xttp_struct.h"


int main(int argc, char *argv[])
{
	curlpp::hxxp().globalInit();

	curlpp::net_default_data default_data;
	default_data.download_path(".");
	default_data.timeout(10);

	curlpp::result get_rs = curlpp::hxxp().url("http://www.baidu.com").get();
	if (get_rs.state()){
		std::cout << "get success" << std::endl;
	}else{
		std::cout << "get fail," << get_rs.value() << std::endl;
	}

	curlpp::result post_rs = curlpp::hxxp().url("http://www.baidu.com").add_header("").add_header("").add_post_params("", "").post();
	if (get_rs.state()){
		std::cout << "post success" << std::endl;
	}else{
		std::cout << "post fail," << get_rs.value() << std::endl;
	}

	curlpp::result download_rs = curlpp::hxxp().url("http://www.baidu.com/img/bd_logo1.png").download_path(".").outfile_name("").download();
	if (download_rs.state()){
		std::cout << "download success" << std::endl;
	}
	else{
		std::cout << "download fail," << download_rs.value() << std::endl;
	}

	curlpp::hxxp().globalClean();

	return 1;
}