/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2016 Mikhail Kurinnoi, Viewizard


	AstroMenace is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	AstroMenace is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


#include "System.h"







//------------------------------------------------------------------------------------
// все что ниже, нужно для открытия броузера
//------------------------------------------------------------------------------------








#ifdef WIN32
#include <tchar.h>
#include <shellapi.h>
static LONG GetRegistryKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
	HKEY hkey;
	LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);
	if (retval == ERROR_SUCCESS)
	{
		long datasize = MAX_PATH;
		TCHAR data[MAX_PATH];
		RegQueryValue(hkey, NULL, data, &datasize);
		_tcscpy(retdata,data);
		RegCloseKey(hkey);
	}
	return retval;
}
#endif // WIN32





#if defined(__APPLE__) && defined(__MACH__)

#include <CoreFoundation/CFBundle.h>
#include <ApplicationServices/ApplicationServices.h>

#endif // __APPLE__





// поиск пути для запуска броузера
/*
Build dependencies:
	libc6-dev
Runtime dependencies:
	libc6
*/
#ifdef __unix

#include <stdio.h>
#include <string.h>

#include <dirent.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


char **get_path(void)
{
	const char *path = getenv("PATH"), *cstr;
	char *null_path, **tokenized_path, *str;
	unsigned int parts = (path[0] != '\0') ? 1 : 0, a;

	null_path = (char *)malloc(strlen(path) + 1);

	for(cstr = path, str = null_path; *cstr; ++cstr, ++str)
	{
		switch(*cstr)
		{
			case ':':
				*str = '\0';
				++parts;
				break;
			default:
				*str = *cstr;
		}
	}
	*str = '\0';

	tokenized_path = (char **)malloc(sizeof(char *) * (parts+1));
	tokenized_path[parts] = NULL;

	for(a = 0, str = null_path; a < parts; ++a)
	{
		tokenized_path[a] = str;

		do { ++str; } while(str[-1]);
	}

	return tokenized_path;
}


int executable_exists_in_path(char **tokenized_path, const char *app_name)
{
	unsigned int a;
	int found = 0;

	for(a = 0; !found && tokenized_path[a]; ++a)
	{
		DIR *dir = opendir(tokenized_path[a]);

		if(dir)
		{
			struct dirent *dirent;

			for(dirent = readdir(dir); dirent != NULL; dirent = readdir(dir))
			{
				if(strcmp(dirent->d_name, app_name) == 0)
				{
					/* We found something with a correct name, is it a proper executable? */
					size_t full_path_length = strlen(tokenized_path[a]) + 1 + strlen(app_name) + 1;
					char *full_path = (char *)malloc(full_path_length);
					struct stat buf;

					snprintf(full_path, full_path_length, "%s/%s", tokenized_path[a], app_name);

					if(stat(full_path, &buf) == 0)
					{
						/* Is is a regular file and is it executable by anyone? */
						if(S_ISREG(buf.st_mode) && (buf.st_mode & S_IXOTH)) found = 1;

						/* FIXME It would be more accurate to test if this user can
						 * execute the file rather than if any user can.
						 */
					}

					free(full_path);
				}
			}

			closedir(dir);
		}
	}

	return found;
}

char **get_browsers(void)
{

	char **tokenized_path = get_path(), **browsers = NULL;


	if(!browsers)
	{
		static const char *browser_list[] =
		{
			"xdg-open", // первое ставим именно xdg-open
			"firefox",
			"opera",
			"chrome",
			"konqueror",
			"chromium",
			"epiphany",
			"midori",
			"dillo",
			"rekonq"
		};

		unsigned int a, b, browser_list_count = sizeof(browser_list)/sizeof(browser_list[0]);

		browsers = (char **)calloc(browser_list_count, sizeof(char *));

		for(a = b = 0; a < browser_list_count; ++a)
		{
			if(executable_exists_in_path(tokenized_path, browser_list[a]))
				browsers[b++] = strdup(browser_list[a]);
		}

		if(b == 0)
		{
			free(browsers);
			browsers = NULL;
		}
	}

	free(tokenized_path[0]);
	free(tokenized_path);

	return browsers;
}

#endif// __unix




























// открываем браузер
bool vw_OpenBrouser(const char *url)
{
#ifdef WIN32

	HINSTANCE result;
	result = ShellExecute(NULL, _T("open"), url, NULL,NULL, SW_NORMAL);
	if ((UINT)result > HINSTANCE_ERROR) return true;

	// если не получилось, делаем по второму сценарию
	TCHAR key[MAX_PATH + MAX_PATH];
	if (GetRegistryKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS)
	{
		lstrcat(key, _T("\\shell\\open\\command"));

		if (GetRegistryKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS)
		{
			TCHAR *pos;
			pos = _tcsstr(key, _T("\"%1\""));
			if (pos == NULL)
			{                     // No quotes found
				pos = strstr(key, _T("%1"));       // Check for %1, without quotes
				if (pos == NULL)                   // No parameter at all...
					pos = key+_tcslen(key)-1;
				else
					*pos = _T('\0');                   // Remove the parameter
			}
			else
				*pos = _T('\0');                       // Remove the parameter

			lstrcat(pos, _T(" "));
			lstrcat(pos, url);
			result = (HINSTANCE) WinExec(key,SW_NORMAL);
		}
	}
	else
		return false;

#endif // WIN32
#if defined(__APPLE__) && defined(__MACH__)

	CFURLRef openurl = CFURLCreateWithBytes (
				NULL,						// allocator
				(BYTE*)url,					// URLBytes
				strlen(url),				// length
				kCFStringEncodingASCII,		// encoding
				NULL);						// baseURL
	LSOpenCFURLRef(openurl,0);
	CFRelease(openurl);

#endif // __APPLE__
#ifdef __unix

	char **browsers = get_browsers();
	unsigned int a = 0;


	if(browsers)
    {
    	// не перебираем!!! берем первый броузер
    	a = 0;

        printf("%u:\t%s\n", a, browsers[a]);

        char GotoUrl[1024];
		sprintf(GotoUrl,"%s %s",browsers[a], url);


        int x;
        x = fork();

		switch(x)
		{
			case -1:
				printf("error, unable to fork process!\n");
				break;
			case 0:
				execl(getenv("SHELL"), "sh", "-c", GotoUrl, (char *)0);
				// should not be reached
				printf("Error executing process!\n");
				break;
		}
	}
	else
	{
		printf("Could not open Web page. Please, visit %s\n", url);
		return false;
	}


	if(browsers)
	{
		for(a = 0; browsers[a]; ++a)
			free(browsers[a]);
		free(browsers);
	}

#endif // unix


	return true;
}
