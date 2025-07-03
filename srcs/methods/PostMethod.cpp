#include <fstream>
#include "GetMethod.hpp"
#include "../../_includes/client.hpp"


std::map<std::string, std::string> createMimeTypeMap() {
    std::map<std::string, std::string> inverseMimeMap;

    // Text
    inverseMimeMap["text/html"] = ".html";
    inverseMimeMap["text/css"] = ".css";
    inverseMimeMap["text/xml"] = ".xml";
    inverseMimeMap["text/plain"] = ".txt";
    inverseMimeMap["text/mathml"] = ".mml";
    inverseMimeMap["text/vnd.sun.j2me.app-descriptor"] = ".jad";
    inverseMimeMap["text/vnd.wap.wml"] = ".wml";
    inverseMimeMap["text/x-component"] = ".htc";

    // Images
    inverseMimeMap["image/gif"] = ".gif";
    inverseMimeMap["image/jpeg"] = ".jpg";  // Prefer .jpg over .jpeg
    inverseMimeMap["image/png"] = ".png";
    inverseMimeMap["image/svg+xml"] = ".svg";
    inverseMimeMap["image/tiff"] = ".tiff";
    inverseMimeMap["image/vnd.wap.wbmp"] = ".wbmp";
    inverseMimeMap["image/webp"] = ".webp";
    inverseMimeMap["image/x-icon"] = ".ico";
    inverseMimeMap["image/x-jng"] = ".jng";
    inverseMimeMap["image/x-ms-bmp"] = ".bmp";
    inverseMimeMap["image/avif"] = ".avif";

    // Fonts
    inverseMimeMap["font/woff"] = ".woff";
    inverseMimeMap["font/woff2"] = ".woff2";

    // Application
    inverseMimeMap["application/javascript"] = ".js";
    inverseMimeMap["application/json"] = ".json";
    inverseMimeMap["application/java-archive"] = ".jar";
    inverseMimeMap["application/mac-binhex40"] = ".hqx";
    inverseMimeMap["application/msword"] = ".doc";
    inverseMimeMap["application/pdf"] = ".pdf";
    inverseMimeMap["application/postscript"] = ".ps";
    inverseMimeMap["application/rtf"] = ".rtf";
    inverseMimeMap["application/vnd.apple.mpegurl"] = ".m3u8";
    inverseMimeMap["application/vnd.google-earth.kml+xml"] = ".kml";
    inverseMimeMap["application/vnd.google-earth.kmz"] = ".kmz";
    inverseMimeMap["application/vnd.ms-excel"] = ".xls";
    inverseMimeMap["application/vnd.ms-fontobject"] = ".eot";
    inverseMimeMap["application/vnd.ms-powerpoint"] = ".ppt";
    inverseMimeMap["application/vnd.oasis.opendocument.graphics"] = ".odg";
    inverseMimeMap["application/vnd.oasis.opendocument.presentation"] = ".odp";
    inverseMimeMap["application/vnd.oasis.opendocument.spreadsheet"] = ".ods";
    inverseMimeMap["application/vnd.oasis.opendocument.text"] = ".odt";
    inverseMimeMap["application/vnd.openxmlformats-officedocument.presentationml.presentation"] = ".pptx";
    inverseMimeMap["application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"] = ".xlsx";
    inverseMimeMap["application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = ".docx";
    inverseMimeMap["application/vnd.wap.wmlc"] = ".wmlc";
    inverseMimeMap["application/wasm"] = ".wasm";
    inverseMimeMap["application/x-7z-compressed"] = ".7z";
    inverseMimeMap["application/x-cocoa"] = ".cco";
    inverseMimeMap["application/x-java-archive-diff"] = ".jardiff";
    inverseMimeMap["application/x-java-jnlp-file"] = ".jnlp";
    inverseMimeMap["application/x-makeself"] = ".run";
    inverseMimeMap["application/x-perl"] = ".pl";
    inverseMimeMap["application/x-pilot"] = ".prc";
    inverseMimeMap["application/x-rar-compressed"] = ".rar";
    inverseMimeMap["application/x-redhat-package-manager"] = ".rpm";
    inverseMimeMap["application/x-sea"] = ".sea";
    inverseMimeMap["application/x-shockwave-flash"] = ".swf";
    inverseMimeMap["application/x-tcl"] = ".tcl";
    inverseMimeMap["application/x-x509-ca-cert"] = ".der";
    inverseMimeMap["application/x-xpinstall"] = ".xpi";
    inverseMimeMap["application/xhtml+xml"] = ".xhtml";
    inverseMimeMap["application/xspf+xml"] = ".xspf";
    inverseMimeMap["application/zip"] = ".zip";
    inverseMimeMap["application/x-www-form-urlencoded"] = ".urlencoded";
    inverseMimeMap["application/octet-stream"] = ".bin";  // Default for unknown binaries

    // Audio
    inverseMimeMap["audio/midi"] = ".mid";
    inverseMimeMap["audio/mpeg"] = ".mp3";
    inverseMimeMap["audio/ogg"] = ".ogg";
    inverseMimeMap["audio/x-m4a"] = ".m4a";
    inverseMimeMap["audio/x-realaudio"] = ".ra";

    // Video
    inverseMimeMap["video/3gpp"] = ".3gp";
    inverseMimeMap["video/mp2t"] = ".ts";
    inverseMimeMap["video/mp4"] = ".mp4";
    inverseMimeMap["video/mpeg"] = ".mpeg";
    inverseMimeMap["video/quicktime"] = ".mov";
    inverseMimeMap["video/webm"] = ".webm";
    inverseMimeMap["video/x-flv"] = ".flv";
    inverseMimeMap["video/x-m4v"] = ".m4v";
    inverseMimeMap["video/x-mng"] = ".mng";
    inverseMimeMap["video/x-ms-asf"] = ".asf";
    inverseMimeMap["video/x-ms-wmv"] = ".wmv";
    inverseMimeMap["video/x-msvideo"] = ".avi";  // Your requested case

    return inverseMimeMap;
}

std::string RandomString(size_t len)
{
	srand(static_cast<unsigned int>(time(0)));
	std::string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::string newstr;
	int pos;
	while(newstr.size() != len)
	{
		pos = ((rand() % (str.size() - 1)));
		newstr += str.substr(pos,1);
	}
	return newstr;
}

std::string getExtension(data_request data)
{
    std::string tex;
	std::map<std::string, std::string> mimeMap = createMimeTypeMap();
    std::map<std::string, std::string>::iterator it = mimeMap.find(data.headers["content-type"]);
    if(it != mimeMap.end())
    {
        return (mimeMap[data.headers["content-type"]]);
    }
    return "";
}

void post(client &client, std::string buffer)
{
	std::string name_file = client.data_rq.bodyNameFile;// + getExtension(client.data_rq);
	std::ofstream file(name_file.c_str(), std::ios::app);
	std::cout << name_file << "========== " << file.is_open() << "\n";
	if (!file.is_open())
	{
		std::cout << name_file << " not open \n";
		throw(500);
	}
	file << buffer;
	file.close();
	struct stat stat_buff;// chunked 
	if(stat(name_file.c_str(), &stat_buff) == 0)
	{
		// std::cout << "size of our file " << stat_buff.st_size << " octets \n";
		// std::cout << "max body size " << client.myServer.MaxBodySize << " octets \n";
		if(stat_buff.st_size > client.myServer.MaxBodySize)
			throw(413); // Payload Too Larg
	}
}
