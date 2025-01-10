#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include "Main.h"
//#include "HttpClass.h"
#include "base64.h"

int DownProgress = 0, DownStatus = 0;
std::string LatestDownloadedString;


bool ProgressProc(double progress)
{
	wprintf(L"Current progress: %-.1f%%\r\n", progress);
	return true;
}

struct UrlComponents {
	std::string host;
	std::string path;
};

UrlComponents parse_url(const std::string& url) {
	std::regex url_regex(R"(^(http[s]?:\/\/)?([^\/\s]+)(\/.*)?$)");
	std::smatch url_match_result;

	UrlComponents components;

	if (std::regex_match(url, url_match_result, url_regex)) {
		components.host = url_match_result[1].str() + url_match_result[2].str();
		components.path = url_match_result[3].str();
		if (components.path.empty()) {
			components.path = "/";
		}
	}
	else {
		components.host = url;
		components.path = "/";
	}

	return components;
}


std::string SendHttpPostRequest(const char* url, const char* data)
{
	if (!url || url[0] == '\0' || !data)
		return "";

	UrlComponents uril = parse_url(url);
	try
	{
		httplib::Client client(uril.host);
		client.set_follow_location(true);

		auto res = client.Post(uril.path, data, "text/plain");

		if (res)
		{
			if (res->status == httplib::StatusCode::OK_200)
			{
				DownStatus = 1;
				return res->body;
			}

			DownStatus = -1;
			return res->body;
		}
	}
	catch (std::exception& ex)
	{
		DownStatus = -3;
		return ex.what() ? ex.what() : "";
	}
	catch (...)
	{

	}
	DownStatus = -2;
	return "";
}

std::string SendHttpGetRequest(const char* host, const char* path)
{
	if (!host || host[0] == '\0' || !path)
		return "";

	try
	{
		httplib::Client client(host);

		client.set_follow_location(true);

		auto res = client.Get(path);

		if (res)
		{
			if (res->status == httplib::StatusCode::OK_200)
			{
				DownStatus = 1;
				return res->body;
			}
			DownStatus = -1;
			return res->body;
		}
	}
	catch (std::exception & ex)
	{
		DownStatus = -3;
		return ex.what() ? ex.what() : "";
	}
	catch (...)
	{

	}

	DownStatus = -2;
	return "";
}


void DownloadNewMapToFile(const char* szUrl, const char* filepath)
{
	if (FileExist(filepath))
	{
		DownStatus = 2;
		return;
	}
	DownProgress = 0;
	DownStatus = -1;

	try
	{
		httplib::Client client(szUrl);

		auto progress_callback = [&](uint64_t current, uint64_t total) {
			if (total > 0) {
				DownProgress = (int)((static_cast<double>(current) / total) * 100.0);
			}
			return true; 
			};

		auto res = client.Get("/", progress_callback);

		if (res && res->status == httplib::StatusCode::OK_200)
		{
			std::ofstream ofs(filepath, std::ios::binary);
			if (ofs.is_open()) 
			{
				ofs.write(res->body.c_str(), res->body.size());
				ofs.close();
				DownStatus = 1;
				DownProgress = 100;
			}
			else
			{
				DownStatus = -1;
				DownProgress = 0;
			}
		}
		else {
			DownStatus = -1;
			DownProgress = 0;
		}
	}
	catch (...)
	{
		DownStatus = -1;
		DownProgress = 0;
	}
	return;
}

std::string _addr;
std::string _request;
std::string _filepath;

unsigned long __stdcall SENDGETREQUEST(LPVOID)
{
	try
	{
		LatestDownloadedString = SendHttpGetRequest(_addr.c_str(), _request.c_str());
		_addr.clear();
		_request.clear();
	}
	catch (...)
	{
		DownStatus = -1;
	}
	return 0;
}


unsigned long __stdcall SENDPOSTREQUEST(LPVOID)
{
	try
	{
		LatestDownloadedString = SendHttpPostRequest(_addr.c_str(), _request.c_str());
		_addr.clear();
		_request.clear();
	}
	catch (...)
	{
		DownStatus = -1;
	}
	return 0;
}



unsigned long __stdcall SENDSAVEFILEREQUEST(LPVOID)
{
	DownloadNewMapToFile(_addr.c_str(), _filepath.c_str());
	_addr.clear();
	_request.clear();
	_filepath.clear();
	return 0;
}

int __stdcall SendGetRequest(const char* url, const  char* path)
{
	DownProgress = 0;
	_addr = url; 
	_request = path;
	DownStatus = 0;
	std::thread([&]() {
		SENDGETREQUEST(0);
		}).detach();
	return 0;
}

int __stdcall SendPostRequest(const char* url, const  char* request)
{
	DownProgress = 0;
	_addr = url ? url : "";
	_request = request ? request : "";
	DownStatus = 0;

	std::thread([&]() {
		SENDPOSTREQUEST(0);
		}).detach();

	return 0;
}


int __stdcall SendPostRequestEx(const char* url, const char * path, const  char* request)
{
	DownProgress = 0;
	_addr = url && path ? std::string(url) + std::string(path) : "";
	_request = request ? request : "";
	DownStatus = 0;

	std::thread([&]() {
		SENDPOSTREQUEST(0);
		}).detach();

	return 0;
}

int __stdcall SaveNewDotaVersionFromUrl(const  char* addr, const  char* filepath)
{
	DownProgress = 0;
	_addr = addr ? addr : ""; _filepath = filepath ? filepath : "";
	DownStatus = 0;

	std::thread([&]() {
		SENDSAVEFILEREQUEST(0);
		}).detach();

	return 0;
}

int __stdcall GetDownloadStatus(int)
{
	return DownStatus;
}

int __stdcall GetDownloadProgress(int)
{
	return DownProgress;
}

const char* __stdcall GetLatestDownloadedString(int)
{
	return LatestDownloadedString.length() > 1023 ?
		std::string(LatestDownloadedString.begin(), LatestDownloadedString.begin() + 1023).c_str()
		: LatestDownloadedString.c_str();
}

//
//std::filesystem::file_time_type GetLastFileTime( const std::vector<std::string> & files )
//{
//	std::filesystem::file_time_type retval = std::filesystem::file_time_type( );
//	std::string file = files
//	for ( auto s : files )
//	{
//		auto curfiletime = std::filesystem::last_write_time( s );
//		if ( curfiletime > retval )
//		{
//			retval = curfiletime;
//		}
//	}
//
//}

bool IsOkayLogFile(std::string file)
{
	if (fs::path(file).extension().string() == ".txt")
	{
		std::string filedata = ToLower(GetFileContent(file));
		if (filedata.length() > 0)
		{
			if (strstr(filedata.c_str(), ToLower("DotAAllstarsHelper").c_str()))
			{
				return true;
			}
			if (strstr(filedata.c_str(), ToLower("DotA Allstars").c_str()))
			{
				return true;
			}
		}
	}
	return false;
}


int __stdcall SendLatestError(const char* url)
{
	auto Errors = get_file_list("Errors", true);
	std::string sendfilename = "Errors\\lastcheck.txt";
	auto LatestError = FileExist(sendfilename.c_str()) ?
		std::filesystem::last_write_time(sendfilename) : std::filesystem::file_time_type();
	bool foundlatesterror = false;

	if (SetInfoObjDebugVal)
	{
		if (FileExist(sendfilename.c_str()))
		{
			PrintText("Found lastcheck.txt");
		}
		else
		{
			PrintText("lastcheck.txt not found");
		}
	}

	for (auto & s : Errors)
	{

		if (IsOkayLogFile(s))
		{
			if (SetInfoObjDebugVal)
			{
				PrintText("Process file:" + s);
			}
			auto curfiletime = std::filesystem::last_write_time(s);
			if (curfiletime > LatestError)
			{
				if (SetInfoObjDebugVal)
				{
					PrintText("Found new file!");
				}
				LatestError = curfiletime;
				sendfilename = s;
				foundlatesterror = true;
			}
		}
	}

	if (foundlatesterror)
	{
		if (SetInfoObjDebugVal)
		{
			PrintText("Found error: " + sendfilename);
		}
		std::string filedata = GetFileContent(sendfilename);
		std::string encodedfiledata;
		if (!Base64::Encode(filedata, &encodedfiledata))
		{
			SendPostRequest(url, encodedfiledata.c_str());
			if (SetInfoObjDebugVal)
			{
				PrintText("Error success sent!");
			}
		}
		else
		{
			if (SetInfoObjDebugVal)
			{
				PrintText("Error send file! Encode Base64 problem.");
			}
		}
	}
	else
	{
		if (SetInfoObjDebugVal)
		{
			PrintText("Error not found!");
		}
	}

	std::ofstream newFile("Errors\\lastcheck.txt");
	newFile << " Just empty file ";
	newFile.flush();
	newFile.close();

	return 1;
}
