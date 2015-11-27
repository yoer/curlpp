#include "xttp_impl.h"


#include <sstream>
#include <algorithm>
#include <regex>
#include <stdlib.h>
#include <sys/stat.h>

#include "xttp_struct.h"
#define MAX_PATH_LEN 260


size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
	std::string data((const char*)ptr, (size_t)size * nmemb);
	*((std::stringstream*)stream) << data;
	return size * nmemb;
}

//检查文件夹是否存在，不存在的话，创建之
void check_dir(const char* fileName) {
    
    if (nullptr == fileName) {
        return;
    }
    
    for(const char* tag = fileName; *tag != '\0'; tag++ )
    {
        if (*tag == '/')
        {
            char buf[MAX_PATH_LEN];
            char path[MAX_PATH_LEN];
            strcpy(buf,fileName);
            buf[strlen(fileName) - strlen(tag) + 1] = NULL;
            strcpy(path,buf);
            if (access(path,6) == -1)
            {
                mkdir(path, 0744);
            }
        }
    }
}

static size_t write_file(void *buffer, size_t size, size_t nmemb, void *stream)
{
	curlpp::download_file *out = (curlpp::download_file *)stream;

	if (nullptr != out && nullptr == out->stream) {
        check_dir(out->filename);
		out->stream = fopen(out->filename, "wb");
        //如果有md5验证、那么就开启md5验证
        if (out->need_degist) {
            out->pMd5 = new MD5();
        }
		if (nullptr == out->stream){
			return -1;
		}
	}
    
    if (out->need_degist && nullptr != out->pMd5) {
        out->pMd5->update((char*)buffer, (unsigned int)(size*nmemb));
    }
    
	return fwrite(buffer, size, nmemb, out->stream);
}

curlpp::xttp_impl::xttp_impl()
{
}

curlpp::xttp_impl::~xttp_impl()
{
}

curlpp::result curlpp::xttp_impl::get(const net_data& data)
{
	std::stringstream out;
	curlpp::curl_x* mycurl = normalCurl(data, &out);
	if (nullptr != mycurl && nullptr == mycurl->curl()) {
		return *curlpp::result::fail();
    } else if(nullptr == mycurl) {
        return *curlpp::result::fail();
    }

	return request(mycurl, &out);
}

curlpp::result curlpp::xttp_impl::post(const net_data& data)
{
	std::stringstream out;
	curlpp::curl_x* mycurl = normalCurl(data, &out);
    if (nullptr != mycurl && nullptr == mycurl->curl()) {
        return *result::fail();
    } else if(nullptr == mycurl) {
        return *result::fail();
    }

	curl_easy_setopt(mycurl->curl(), CURLOPT_POST, 1);
    std::string params = data.post_params();
	curl_easy_setopt(mycurl->curl(), CURLOPT_POSTFIELDS, params.c_str());

	return request(mycurl, &out);
}

std::string  curlpp::xttp_impl::parse_download_file_name(const std::string& url) const{
	std::regex download_filename_regex("^.*/(.+\..+)$", std::regex_constants::ECMAScript | std::regex_constants::icase);

	std::smatch name_match;
	std::regex_match(url, name_match, download_filename_regex);

	if (2 == name_match.size())
	{
		return name_match[1].str();
	}

	time_t t(0);
	struct tm * time_now = localtime(&t);

	return std::to_string((long long)time_now->tm_year + 1900) + "-" +
		std::to_string((long long)time_now->tm_mon + 1) + "-" +
		std::to_string((long long)time_now->tm_mday) + "-" +
		std::to_string((long long)time_now->tm_hour) + "-" +
		std::to_string((long long)time_now->tm_min) + "-" +
		std::to_string((long long)time_now->tm_sec);
}

curlpp::result curlpp::xttp_impl::download(const net_data& data) const
{
	CURL *curl = curl_easy_init();
	if (nullptr == curl) {
		return *curlpp::result::fail();
	}

	curlpp::download_file file;
    file.need_degist = data.need_degist();
	std::string download_file_name = data.download_path() + "/";
	if (data.outfile_name().empty()){
		download_file_name += parse_download_file_name(data.url()).c_str();
	}
	else{
		download_file_name += data.outfile_name();
	}
	file.filename = download_file_name.c_str();

	curl_easy_setopt(curl, CURLOPT_URL, data.url().c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1); //Prevent "longjmp causes uninitialized stack frame" bug
	curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");

	struct curl_slist *chunk = NULL;
	std::for_each(data.headers().begin(), data.headers().end(), [&](const std::string& header){
		chunk = curl_slist_append(chunk, header.c_str());
	});
    
    curlpp::curl_x mycurl = curlpp::curl_x(curl, chunk);

    CURLcode res = curl_easy_perform(curl);
    
    if (file.need_degist && NULL != file.pMd5) {
        file.pMd5->finalize();
        file.md5 = file.pMd5->hexdigest();
        if (0 == file.md5.compare(data.md5()) && res == CURLE_OK) {
            return curlpp::result(true, "");
        } else if(0 != file.md5.compare(data.md5()) && res == CURLE_OK ) {
            remove(file.filename);
            return curlpp::result(false, "md5 degist faield!");
        }
    }
    
	if (res != CURLE_OK) {
		return curlpp::result(false, curl_easy_strerror(res));
	}

	return curlpp::result(true, "");
}

curlpp::curl_x* curlpp::xttp_impl::normalCurl(const curlpp::net_data& data, std::stringstream* out) const
{
	CURL *curl = curl_easy_init();
	if (nullptr == curl) {
		return new curlpp::curl_x(nullptr, nullptr);
	}

	curl_easy_setopt(curl, CURLOPT_URL, data.url().c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, data.timeout());

	struct curl_slist *chunk = NULL;
	std::for_each(data.headers().begin(), data.headers().end(), [&](const std::string& header)
	{
		chunk = curl_slist_append(chunk, header.c_str());
	});
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	return new curlpp::curl_x(curl, chunk);
}

curlpp::result curlpp::xttp_impl::request(curlpp::curl_x *mycurl, std::stringstream* rs)
{
	curl_cleaner clean(mycurl);
	CURLcode res = curl_easy_perform(mycurl->curl());

	if (res != CURLE_OK) {
		return curlpp::result(false, curl_easy_strerror(res));
	}
    
    format_result(const_cast<char*>(rs->str().c_str()));
	return curlpp::result(true, rs->str());
}

//格式化返回结果，把最近的\r\n替换成结束符，发现部分返回\r\n后还有...0\r\n 不符合http协议
void curlpp::xttp_impl::format_result(char* rs) {
    if(nullptr == rs || 0 == strlen(rs)) return;
    size_t size = strlen(rs);
    
    for(size_t i = 0; i < size - 1; i++ ) {
        if(rs[i] == '\r' || rs[i + 1] == '\n') {
            rs[i] = '\0';
            break;
        }
    }
}

