#include "xttp_struct.h"

//////////////////////////////////////////////////////////////////////////
xttp_rs* xttp_rs::g_curlFailed = nullptr;

xttp_rs::xttp_rs(bool state, const std::string& result) :m_state(state), m_result(result)
{

}

xttp_rs* xttp_rs::fail()
{
	if (nullptr == g_curlFailed) {
		g_curlFailed = new xttp_rs(false, "");
	}

	return g_curlFailed;
}

void xttp_rs::result(std::string val)
{
	m_result = val;
}

const std::string& xttp_rs::result() const
{
	return m_result;
}

void xttp_rs::state(bool val)
{
	m_state = val;
}

bool xttp_rs::state() const
{
	return m_state;
}

//////////////////////////////////////////////////////////////////////////

curl_ex_cleaner::~curl_ex_cleaner()
{
	if (nullptr != mycurl->curl) {
		curl_easy_cleanup(mycurl->curl);
	}
	if (nullptr != mycurl->chunk) {
		curl_slist_free_all(mycurl->chunk);
	}
}


//////////////////////////////////////////////////////////////////////////

void xttp_net_data::headers(const std::vector<std::string>& headers)
{
	m_headers = headers;
}

const std::vector<std::string>& xttp_net_data::headers() const
{
	return m_headers;
}

void xttp_net_data::outfile_name(const std::string& outfile_name)
{
	m_outfile_name = outfile_name;
}

const std::string& xttp_net_data::outfile_name() const
{
	return m_outfile_name;
}

void xttp_net_data::post_fields(const std::string& post_fields)
{
	m_post_fields = post_fields;
}

const std::string& xttp_net_data::post_fields() const
{
	return m_post_fields;
}

void xttp_net_data::url(const std::string& url)
{
	m_url = url;
}

const std::string& xttp_net_data::url() const
{
	return m_url;
}

void xttp_net_data::timeout(int val)
{
	m_timeout = val;
}

int xttp_net_data::timeout() const
{
	return m_timeout;
}
