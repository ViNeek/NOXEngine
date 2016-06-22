#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <ctime>

#include "Utils.h"

void Utils::Debug::init_debug_console(){

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF );

	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	int consoleHandleR, consoleHandleW;
	long stdioHandle;
	FILE *fptr;

	AllocConsole();
	std::string strW = "Dev Console";
	SetConsoleTitle((LPCSTR) strW.c_str());

	EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_GRAYED);
	DrawMenuBar(GetConsoleWindow());

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);

	stdioHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	consoleHandleR = _open_osfhandle(stdioHandle, _O_TEXT);
	fptr = _fdopen(consoleHandleR, "r");
	*stdin = *fptr;
	setvbuf(stdin, NULL, _IONBF, 0);

	stdioHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	consoleHandleW = _open_osfhandle(stdioHandle, _O_TEXT);
	fptr = _fdopen(consoleHandleW, "w");
	*stdout = *fptr;
	setvbuf(stdout, NULL, _IONBF, 0);

	stdioHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	*stderr = *fptr;
	setvbuf(stderr, NULL, _IONBF, 0);

}

// Random Generator Engine
static boost::random::mt19937 gc_RandomGen(std::time(0));
nxInt32 Utils::RandomInRange(nxInt32 min, nxInt32 max) {
	boost::random::uniform_int_distribution<> dist(min, max);
	return dist(gc_RandomGen);
}

std::string Utils::GetFilename(const std::string& p_Path) {
	std::size_t l_FoundPos = p_Path.find_last_of("/\\");

	if (l_FoundPos == std::string::npos)
		return p_Path;

	std::string l_Filename = p_Path.substr(l_FoundPos + 1);

	return l_Filename;
}

std::string Utils::GetParentDirectory(const std::string& p_File) {
	std::size_t l_FoundPos = p_File.find_last_of("/\\");

	if (l_FoundPos == std::string::npos)
		return p_File;

	std::string l_Path = p_File.substr(0, l_FoundPos);

	return l_Path;
}

/*
std::string Utils::GetFilename(const std::string& p_Path) {
    std::size_t l_FoundPos = p_Path.find_last_of("/\\");
    std::string l_Filename = p_Path.substr(l_FoundPos + 1);

    return l_Filename;
}

std::string Utils::GetParentDirectory(const std::string& p_File) {
    std::size_t l_FoundPos = p_File.find_last_of("/\\");
    std::string l_Path = p_File.substr(0, l_FoundPos);

    return l_Path;
}
*/
