#pragma once


#include <string>
#include <vector>

#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/curlbuild.h>


/************************************************************************/
/*                                                                      */
/************************************************************************/
typedef struct file {
	const char *filename;
	std::FILE *stream;
} download_file;


/************************************************************************/
/*                                                                      */
/************************************************************************/
class xttp_net_data{
public:
	int timeout() const;
	void timeout(int val);

	const std::string& url() const;
	void url(const std::string& url);

	const std::string& post_fields() const;
	void post_fields(const std::string& post_fields);

	const std::string& outfile_name() const;
	void outfile_name(const std::string& outfile_name);

	const std::vector<std::string>& headers() const;
	void headers(const std::vector<std::string>& headers);


private:
	int m_timeout;
	std::string m_url;
	std::string m_post_fields;
	std::string m_outfile_name;
	std::vector<std::string> m_headers;
};


/************************************************************************/
/*                                                                      */
/************************************************************************/
class xttp_rs{
public:
	xttp_rs(bool state, const std::string& result);

public:
	static xttp_rs* fail();

public:
	bool state() const;
	void state(bool val);

	const std::string& result() const;
	void result(std::string val);

private:
	bool m_state;
	std::string m_result;

private:
	static xttp_rs* g_curlFailed;
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
class curl_ex{
public:
	curl_ex(CURL *curl, struct curl_slist *chunk) :curl(curl), chunk(chunk){}

public:
	CURL *curl;
	struct curl_slist *chunk;
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
class curl_ex_cleaner{
public:
	curl_ex_cleaner(curl_ex *mycurl) :mycurl(mycurl){}
	virtual ~curl_ex_cleaner();
private:
	curl_ex *mycurl;
};

