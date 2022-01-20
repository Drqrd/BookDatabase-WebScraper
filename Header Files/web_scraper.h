#ifndef WEB_SCRAPER_H
#define WEB_SCRAPER_H

#include <string>

using namespace std;

class WebScraper
{
	public:
		int Download();
		string ParseHTML(string);
		string CutIMG(string);
		string RemoveURL(string);
		string RemoveUndated(string);
		string Format(string);
		string RemoveErrors(string);
};
#endif // !WEB_DOWNLOADER_H
