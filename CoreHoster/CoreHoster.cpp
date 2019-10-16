// CoreHoster.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <type_traits>
#include <windows.h>
#include "coreclrhost.h"
using namespace std;

const string FS_SEPARATOR{ "\\"s };
const string PATH_DELIMITER{ ";" };
const string CORECLR_FILE_NAME{ "coreclr.dll" };
const string MANAGED_ASSEMBLY{ "ConsoleApp.dll" };

string BuildTpaList(string runtimePath, string directory, string extension)
{
	string tpaList;
	// This will add all files with a .dll extension to the TPA list.
	// This will include unmanaged assemblies (coreclr.dll, for example) that don't
	// belong on the TPA list. In a real host, only managed assemblies that the host
	// expects to load should be included. Having extra unmanaged assemblies doesn't
	// cause anything to fail, though, so this function just enumerates all dll's in
	// order to keep this sample concise.
	string searchPath(directory);
	searchPath += FS_SEPARATOR;
	searchPath += "*";
	searchPath += extension;

	WIN32_FIND_DATAA findData;
	HANDLE fileHandle = FindFirstFileA(searchPath.c_str(), &findData);

	if (fileHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			// Append the assembly to the list
			tpaList += directory;
			tpaList += FS_SEPARATOR;
			tpaList += findData.cFileName;
			tpaList += PATH_DELIMITER;

			// Note that the CLR does not guarantee which assembly will be loaded if an assembly
			// is in the TPA list multiple times (perhaps from different paths or perhaps with different NI/NI.dll
			// extensions. Therefore, a real host should probably add items to the list in priority order and only
			// add a file if it's not already present on the list.
			//
			// For this simple sample, though, and because we're only loading TPA assemblies from a single path,
			// and have no native images, we can ignore that complication.
		} while (FindNextFileA(fileHandle, &findData));
		FindClose(fileHandle);
	}

	tpaList += runtimePath;
	tpaList += FS_SEPARATOR;
	tpaList += "ConsoleApp.dll";
	//tpaList += PATH_DELIMITER;
	//tpaList += R"(C:\Sourcen\Main\projects\Test\CoreHoster\ConsoleApp\bin\Debug\netcoreapp3.0\Library.dll)";
	return tpaList;
}

int main(int argc, char* argv[])
{
    cout << "Hello World!\n";
	auto crpath = R"(C:\Program Files\dotnet\shared\Microsoft.NETCore.App\3.0.0)"s;

	string coreClrPath(crpath);
	coreClrPath += FS_SEPARATOR;
	coreClrPath += CORECLR_FILE_NAME;

	auto coreClr = LoadLibraryExA(coreClrPath.c_str(), nullptr, 0);

	using report_callback_ptr = add_pointer<int(int progress)>::type;
	using doWork_ptr = add_pointer<char*(const char* jobName, int iterations, int dataSize, double* data, report_callback_ptr callbackFunction)>::type;
	using load_ptr = add_pointer<bool(const char* assemblyName)>::type;

	auto initializeCoreClr = (coreclr_initialize_ptr)GetProcAddress(coreClr, "coreclr_initialize");
	auto createManagedDelegate = (coreclr_create_delegate_ptr)GetProcAddress(coreClr, "coreclr_create_delegate");
	auto shutdownCoreClr = (coreclr_shutdown_ptr)GetProcAddress(coreClr, "coreclr_shutdown");

	// Construct the managed library path
	string runtimePath;
	runtimePath.resize(MAX_PATH + 1);
	GetFullPathNameA(argv[0], (DWORD)runtimePath.length(), (char*)runtimePath.data(), nullptr);

	auto pos = runtimePath.find_last_of('\\');
	runtimePath = runtimePath.substr(0, pos);
	pos = runtimePath.find_last_of('\\');
	runtimePath = runtimePath.substr(0, pos);
	pos = runtimePath.find_last_of('\\');
	runtimePath = runtimePath.substr(0, pos);
	runtimePath += R"(\ConsoleApp\bin\Debug\netcoreapp3.0)"s;

	void* hostHandle;
	unsigned int domainId;

	const char* propertyKeys[] = {
		"TRUSTED_PLATFORM_ASSEMBLIES",
		"APP_PATHS" 
	};	

	auto tpaList = BuildTpaList(runtimePath, crpath, ".dll");
	
	const char* propertyValues[] = {
		tpaList.c_str(),
		runtimePath.c_str()
	};

	// This function both starts the .NET Core runtime and creates
	// the default (and only) AppDomain
	int hr = initializeCoreClr(
		runtimePath.c_str(), // App base path
		"SampleHost",        // AppDomain friendly name
		sizeof(propertyKeys)/sizeof(char*),   // Property count
		propertyKeys,       // Property names
		propertyValues,     // Property values
		&hostHandle,        // Host handle
		&domainId);         // AppDomain ID

	load_ptr managedLoadDelegate;

	// The assembly name passed in the third parameter is a managed assembly name
	// as described at https://docs.microsoft.com/dotnet/framework/app-domains/assembly-names
	hr = createManagedDelegate(
		hostHandle,
		domainId,
		//"ManagedLibrary, Version=1.0.0.0",
		"ConsoleApp",
		"ConsoleApp.Klasse",
		"Load",
		(void**)&managedLoadDelegate);
	auto wahr = managedLoadDelegate("Das ist eine Assembly");
/*	string managedLibraryPath(runtimePath);

	//managedLibraryPath.append(FS_SEPARATOR.c);
	managedLibraryPath.append(MANAGED_ASSEMBLY);*/
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
