#include "hxxp.h"


//////////////////////////////////////////////////////////////////////////
void curlpp::hxxp::globalInit()
{
	curl_global_init(CURL_GLOBAL_ALL);
}

void curlpp::hxxp::globalClean()
{
	curl_global_cleanup();
}

curlpp::result curlpp::hxxp::get()
{
	return m_impl.get(m_net_data);
}

curlpp::result curlpp::hxxp::post()
{
	return m_impl.post(m_net_data);
}

curlpp::result curlpp::hxxp::download() const
{
	return m_impl.download(m_net_data);
}

curlpp::hxxp& curlpp::hxxp::headers(const std::vector<std::string>& val)
{
	m_net_data.headers(val);
	return *this;
}

curlpp::hxxp& curlpp::hxxp::outfile_name(const std::string& val)
{
	m_net_data.outfile_name(val);
	return *this;
}

curlpp::hxxp& curlpp::hxxp::add_post_params(const std::string& key, const std::string& value)
{
	m_net_data.post_params_write().add_params(key, value);
	return *this;
}

curlpp::hxxp& curlpp::hxxp::url(const std::string& val)
{
	m_net_data.url(val);
	return *this;
}

curlpp::hxxp& curlpp::hxxp::timeout(int val)
{
	m_net_data.timeout(val);
	return *this;
}

curlpp::hxxp& curlpp::hxxp::add_header(const std::string& val)
{
	m_net_data.append_header(val);
	return *this;
}

curlpp::hxxp& curlpp::hxxp::download_path(const std::string& val)
{
	m_net_data.download_path(val);
	return *this;
}

curlpp::hxxp& curlpp::hxxp::md5(const std::string &val) {
    m_net_data.md5(val);
    return *this;
}
