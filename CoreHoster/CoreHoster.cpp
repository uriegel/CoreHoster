#include <Windows.h>
#include <shlobj_core.h>
#include <array>
#include <type_traits>
#include "CoreHoster.h"

namespace Core_hoster {

	void Hoster::load_clr() {
		// TODO: scan versions
		array<char, MAX_PATH> buffer;
		SHGetFolderPathA(nullptr, CSIDL_PROGRAM_FILES, nullptr, SHGFP_TYPE_DEFAULT, buffer.data());
		string core_path(buffer.data());
		core_path += R"(\dotnet\shared\Microsoft.NETCore.App\)"s + configuration.Clr_version;
		auto core_clr = core_path + R"(\coreclr.dll)"s;
		coreClr = LoadLibraryExA(core_clr.c_str(), nullptr, 0);
		if (coreClr == 0)
			// TODO:
			return;
		
		using coreclr_initialize_ptr = add_pointer<int __stdcall(
			const char* runtime_path,
			const char* app_domain_friendly_name,
			int property_count,
			const char** property_keys,
			const char** property_values,
			void** host_handle,
			unsigned int* domain_id)>::type;
		//using doWork_ptr = add_pointer<char*(const char* jobName, int iterations, int dataSize, double* data, report_callback_ptr callbackFunction)>::type;
		// using load_ptr = add_pointer<bool(const char* assemblyName)>::type;
		
	
		const char* property_keys[] = {
			"TRUSTED_PLATFORM_ASSEMBLIES",
			"APP_PATHS" 
		};	
		auto tpa_list = build_tpa_list(core_path);
		const char* property_values[] = {
			tpa_list.c_str(),
			configuration.Runtime_path.c_str()
		};		
		auto initialize_core_clr = (coreclr_initialize_ptr)GetProcAddress(coreClr, "coreclr_initialize");
		auto result = initialize_core_clr(configuration.Runtime_path.c_str(), configuration.app_domain_friendly_name.c_str(), 
			sizeof(property_keys)/sizeof(char*), property_keys, property_values, &host_handle, &domain_id);
	}

	void* Hoster::create_delegate(const string& type_name, const string& method_name) {
		using coreclr_create_delegate_ptr = add_pointer<int __stdcall(
			void* host_handle, 
			unsigned int domain_id,
			const char* assembly_name,
			const char* type_name,
			const char* method_name,
			void** delegate)>::type;
		auto create_delegate = (coreclr_create_delegate_ptr)GetProcAddress(coreClr, "coreclr_create_delegate");
		void* delegate{ nullptr };
		auto pos = configuration.Assembly_name.find_last_of('.');
		auto assembly_name = configuration.Assembly_name.substr(0, pos);
		auto result = create_delegate(host_handle, domain_id,
			//"ManagedLibrary, Version=1.0.0.0",
			assembly_name.c_str(),
			type_name.c_str(),
			method_name.c_str(),
			&delegate);
		return delegate;
	}

	void Hoster::shutdown() {
		if (host_handle != nullptr) {
			using coreclr_shutdown_ptr = add_pointer<int __stdcall(
				void* host_handle,
				unsigned int domain_id)>::type;
			auto coreclr_shutdown = (coreclr_shutdown_ptr)GetProcAddress(coreClr, "coreclr_shutdown");
			coreclr_shutdown(host_handle, domain_id);
		}
		host_handle = nullptr;
		FreeLibrary(coreClr);
	}
	
	string Hoster::build_tpa_list(const string& core_path) {
		const string FS_SEPARATOR{ "\\"s };
		const string PATH_DELIMITER{ ";" };
		const string CORECLR_FILE_NAME{ "coreclr.dll" };
		const string MANAGED_ASSEMBLY{ "ConsoleApp.dll" };

		string tpaList;
		// This will add all files with a .dll extension to the TPA list.
		// This will include unmanaged assemblies (coreclr.dll, for example) that don't
		// belong on the TPA list. In a real host, only managed assemblies that the host
		// expects to load should be included. Having extra unmanaged assemblies doesn't
		// cause anything to fail, though, so this function just enumerates all dll's in
		// order to keep this sample concise.
		string searchPath(core_path);
		searchPath += FS_SEPARATOR;
		searchPath += "*.dll";
		
		WIN32_FIND_DATAA findData;
		HANDLE fileHandle = FindFirstFileA(searchPath.c_str(), &findData);
		
		if (fileHandle != INVALID_HANDLE_VALUE) {
			do {
				// Append the assembly to the list
				tpaList += core_path;
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
		
		tpaList += configuration.Runtime_path;
		tpaList += FS_SEPARATOR;
		tpaList += configuration.Assembly_name;
		return tpaList;
	}
}

