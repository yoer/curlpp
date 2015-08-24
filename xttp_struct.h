#pragma once


#include <string>
#include <vector>

#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/curlbuild.h>


namespace curlpp
{
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
	class net_default_data{
	public:
		static int timeout();
		static void timeout(int val);

		static std::string& download_path();
		void download_path(std::string val);

	private:
		static int g_timeout;
		static std::string m_download_path;

	};

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	class url_post_params{
	private:
		typedef std::pair<std::string, std::string> post_prarm_type;
		typedef std::vector<post_prarm_type> post_params_type;

	public:
		void add_params(const std::string& key, const std::string& value);
		std::string format() const;
		void clear();

	private:
		post_params_type m_values;
	};

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	class net_data{
	public:
		net_data();

	public:
		int timeout() const;
		void timeout(int val);

		const std::string& url() const;
		void url(const std::string& url);


		const std::string& download_path() const;
		void download_path(const std::string& val);

		const std::string& outfile_name() const;
		void outfile_name(const std::string& outfile_name);

		const std::vector<std::string>& headers() const;
		void append_header(const std::string& header) ;
		void headers(const std::vector<std::string>& headers);

		curlpp::url_post_params& post_params_write() ;
		std::string post_params() const;

	private:
		int m_timeout;
		std::string m_url;
		url_post_params m_post_params;
		std::string m_download_path;
		std::string m_outfile_name;
		std::vector<std::string> m_headers;
	};


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	class result{
	public:
		result(bool state, const std::string& value);

	public:
		static result* fail();

	public:
		bool state() const;
		void state(bool val);

		const std::string& value() const;
		void value(std::string val);

	private:
		bool m_state;
		std::string m_value;

	private:
		static result* g_curlFailed;
	};

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	class curl_x{
	public:
		curl_x(CURL *curl, struct curl_slist *chunk);

	public:
		CURL * curl() const { return m_curl; }
		struct curl_slist * chunk() const { return m_chunk; }

	private:
		CURL *m_curl;
		struct curl_slist *m_chunk;
		
	};

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	class curl_cleaner{
	public:
		curl_cleaner(curl_x *mycurl) :mycurl(mycurl){}
		virtual ~curl_cleaner();
	private:
		curl_x *mycurl;
	};


}