/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: System.cpp

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.1

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#include "System.h"






//------------------------------------------------------------------------------------
// проверка расширения файла
//------------------------------------------------------------------------------------
bool vw_TestFileExtension(const char *name, const char *extension)
{
	if(name==0||extension==0) return false;
	size_t LengthName=strlen(name), LengthString=strlen(extension);
	if(LengthName<LengthString) return false;
	for(int i=LengthName-1;i>=0;i--)
		if(name[i]=='.')
		{
			if(!strcmp(&name[i+1],extension)) return true;
			else return false;
		}
	return false;
}










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


char **get_path(void) {
	const char *path = getenv("PATH"), *cstr;
	char *null_path, **tokenized_path, *str;
	unsigned int parts = (path[0] != '\0') ? 1 : 0, a;

	null_path = (char *)malloc(strlen(path) + 1);

	for(cstr = path, str = null_path; *cstr; ++cstr, ++str) {
		switch(*cstr) {
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

	for(a = 0, str = null_path; a < parts; ++a) {
		tokenized_path[a] = str;

		do { ++str; } while(str[-1]);
	}

	return tokenized_path;
}
void free_path(char **tokenized_path) {
	free(tokenized_path[0]);
	free(tokenized_path);
}

int executable_exists_in_path(char **tokenized_path, const char *app_name) {
	unsigned int a;
	int found = 0;

	for(a = 0; !found && tokenized_path[a]; ++a) {
		DIR *dir = opendir(tokenized_path[a]);

		if(dir) {
			struct dirent *dirent;

			for(dirent = readdir(dir); dirent != NULL; dirent = readdir(dir)) {
				if(strcmp(dirent->d_name, app_name) == 0) {
					/* We found something with a correct name, is it a proper executable? */
					size_t full_path_length = strlen(tokenized_path[a]) + 1 + strlen(app_name) + 1;
					char *full_path = (char *)malloc(full_path_length);
					struct stat buf;

					snprintf(full_path, full_path_length, "%s/%s", tokenized_path[a], app_name);

					if(stat(full_path, &buf) == 0) {
						/* Is is a regular file and is it executable by anyone? */
						if(S_ISREG(buf.st_mode) && (buf.st_mode & S_IXOTH))
							found = 1;

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

char **get_browsers(void) {
	static const char *envvar_list[] = {
		"BROWSER"
	};
	static const char *metabrowser_list[] = {
		"sensible-browser" /* Debian */
	};
	static const char *browser_list[] = {
		"firefox", /* Listed in the order I feel like ;) */
        "galeon",
		"epiphany",
		"konqueror",
		"opera",
		"netscape",
		"dillo",
        "mozilla"
	};
	char **tokenized_path = get_path(), **browsers = NULL;
	unsigned int a;

	/* Check for an environment variable telling us what browser to use */
	if(!browsers) {
		for(a = 0; a < sizeof(envvar_list)/sizeof(envvar_list[0]); ++a) {
			char *value = getenv(envvar_list[a]);

			if(value != NULL) {
				// Assume the user is correct
				browsers = (char **)malloc(sizeof(char *) * (1+1));

				browsers[0] = strdup(value);
				browsers[1] = NULL;

				break;
			}
		}
	}



	/* Check for any browser */
	if(!browsers) {
		unsigned int a, b, browser_list_count = sizeof(browser_list)/sizeof(browser_list[0]);

		browsers = (char **)calloc(browser_list_count, sizeof(char *));

		for(a = b = 0; a < browser_list_count; ++a) {
			if(executable_exists_in_path(tokenized_path, browser_list[a]))
				browsers[b++] = strdup(browser_list[a]);
		}

		if(b == 0) {
			free(browsers);
			browsers = NULL;
		}
	}


	/* Check for distro-specific browser detection type stuff */
	if(!browsers) {
		for(a = 0; a < sizeof(metabrowser_list)/sizeof(metabrowser_list[0]); ++a) {
			if(executable_exists_in_path(tokenized_path, metabrowser_list[a])) {
				// Let it handle the work
				browsers = (char **)malloc(sizeof(char *) * (1+1));

				browsers[0] = strdup(metabrowser_list[a]);
				browsers[1] = NULL;

				break;
			}
		}
	}



	free_path(tokenized_path);

	return browsers;
}
void free_browsers(char **browsers) {
	unsigned int a;

	if(browsers) {
		for(a = 0; browsers[a]; ++a)
			free(browsers[a]);
		free(browsers);
	}
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


		// проверка, если установлен 93 дисплей - это компиз... открываем в 0-м
		char *value = getenv("DISPLAY");
		bool NeedSetDisplay = false;
		if (value != 0)
		{
			char *display = new char[strlen(value)+1];
			display = strdup(value);
			if (!strcmp(display, ":93"))
			{
				NeedSetDisplay = true;
				printf("DISPLAY=%s \n", display);
			}
			delete [] display;
		}


        printf("%u:\t%s\n", a, browsers[a]);

        char GotoUrl[1024];

        if (NeedSetDisplay)
			sprintf(GotoUrl,"DISPLAY=:0 %s %s",browsers[a], url);
		else
			sprintf(GotoUrl,"%s %s",browsers[a], url);



        int x;
        x = fork();

            switch(x)
            {
                case -1:
                        printf("error, unable to fork process!\n");
                        break;
                case 0:
                        //printf("This is the forked process!\n");
                        //system(GotoUrl); - плохо...
                        execl(getenv("SHELL"), "sh", "-c", GotoUrl, (char *)0);
                        // should not be reached
                        printf("Error executing process!\n");
                        break;
                //default:
                        //printf("This is the original process!\n");
            }
	}
	else
	{
		printf("Could not open Web page. Please, visit %s\n", url);
		return false;
	}



	free_browsers(browsers);

#endif // unix


	return true;
}
