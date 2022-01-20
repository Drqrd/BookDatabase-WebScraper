#pragma comment(lib,"wininet.lib")

#include <iostream>
#include <Windows.h>
#include <WinInet.h>
#include <fstream>

#include "web_scraper.h"

/*
	
	SOURCE
	http://www.cplusplus.com/forum/windows/109799/

	DATA FROM
	https://libraryof1000books.wordpress.com/the-list-of-1000-books/

*/

using namespace std;



int WebScraper::Download()
{
	HINTERNET connect = InternetOpen(L"MyBrowser", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	if (!connect)
	{	
		cout << "Internet failed to connect.\n";
		return -1;
	}

	HINTERNET OpenAddress = InternetOpenUrl(connect,L"https://libraryof1000books.wordpress.com/the-list-of-1000-books/", NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0);

	if (!OpenAddress)
	{
		DWORD ErrorNum = GetLastError();
		string str = "Failed to open URL \nError No: " + ErrorNum;
		cout << str << endl;
		InternetCloseHandle(connect);
		return -1;
	}

	char DataReceived[4096];
	DWORD NumberOfBytesRead = 0;

	ofstream ofile;
	ofile.open("Data/site.html", ios::binary);

	while (InternetReadFile(OpenAddress, DataReceived, 4096, &NumberOfBytesRead) && NumberOfBytesRead)
	{
		ofile.write((char*)DataReceived, sizeof(DataReceived));
	}

	ofile.close();

	ifstream ifile;
	ofstream dataFile;
	ifile.open("Resource Files/site.html");
	dataFile.open("Resource Files/book_data.txt");

	string line;

	while (getline(ifile, line))
	{
		if (line.find("<li>") == 0)
		{
			// Cut the <li>
			line = line.substr(4);

			// If has URL, remove
			line = RemoveURL(line);
			line = CutIMG(line);
			line = ParseHTML(line);
			line = RemoveUndated(line);
			line = RemoveErrors(line);

			if (line != "")
			{
				line = Format(line);
				if (line.find("(") == -1) { dataFile << line + "\n"; }
			}
		}
	}

	ifile.close();
	dataFile.close();

	InternetCloseHandle(OpenAddress);
	InternetCloseHandle(connect);

	return 0;
}

string WebScraper::CutIMG(string str)
{
	int pos = str.find("img");
	if (pos >= 0) { return str.substr(0, pos - 1); }
	else { return str; }
}

string WebScraper::ParseHTML(string str)
{

	// -
	int	pos = str.find("&#8211;");
	while (pos >= 0) { str.replace(pos, 7, "-"); pos = str.find("&#8211;"); }

	// '
	pos = str.find("&#8217");
	while (pos >= 0) { str.replace(pos, 7, "'"); pos = str.find("&#8217;"); }

	// also '
	pos = str.find("#8217;");
	while (pos >= 0) { str.replace(pos, 6, "'"); pos = str.find("#8217"); }

	// left "
	pos = str.find("&#8220;");
	while (pos >= 0) { str.replace(pos, 7, "\""); pos = str.find("&#8220;"); }

	// right "
	pos = str.find("&#8221;");
	while (pos >= 0) { str.replace(pos, 7, "\""); pos = str.find("&#8221;"); }

	// R
	pos = str.find("&#82");
	while (pos >= 0) { str.replace(pos, 4, "R"); pos = str.find("&#82"); }

	// &
	pos = str.find("&amp;");
	while (pos >= 0) { str.replace(pos, 5, "&"); pos = str.find("&amp;"); }

	return str;
}

string WebScraper::RemoveURL(string str)
{
	if (str.find("<a") != -1)
	{
		return "";
		int pos = str.find(">");
		str = str.substr(pos + 1);
	}

	return str;
}

string WebScraper::RemoveUndated(string str)
{
	if (str.find("(") == -1)
	{
		return "";
	}
	else { return str; }
}

string WebScraper::RemoveErrors(string str)
{
	if (str.find("libraryof1000books") != -1) {return ""; }
	if (str.find("The Golmalink=") != -1) { return ""; }
	if (str.length() > 100) { return ""; }
	if (str.find(";") != -1) { return ""; }

	return str;
}

string WebScraper::Format(string str)
{
	size_t pos1 = str.find(" - ");
	size_t pos2 = str.find(" - ") + 1;
	size_t pos3 = str.find("(") + 1;
	size_t pos4 = str.find(")");

	string title, authorName, date;

	title = str.substr(0, pos1);
	authorName = str.substr(pos2, pos3 - pos2 - 2).replace(0, 2, "");
	date = str.substr(pos3, pos4 - pos3);

	return title + ";" + authorName + ";" + date;
}