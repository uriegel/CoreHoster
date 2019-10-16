#include <Windows.h>
#include <array>
#include "CoreHoster.h"
using namespace Core_hoster;

int main(int argc, char* argv[]) {
	array<char, MAX_PATH> buffer;
	GetFullPathNameA(argv[0], static_cast<DWORD>(buffer.size()), buffer.data(), nullptr);
	string runtimePath(buffer.data());

	auto pos = runtimePath.find_last_of('\\');
	runtimePath = runtimePath.substr(0, pos);
	pos = runtimePath.find_last_of('\\');
	runtimePath = runtimePath.substr(0, pos);
	pos = runtimePath.find_last_of('\\');
	runtimePath = runtimePath.substr(0, pos);
	runtimePath += R"(\ConsoleApp\bin\Debug\netcoreapp3.0)"s;

	Hoster hoster({
		runtimePath,
		"ConsoleApp.dll"s
		//"2.1.0"s
	});

	
	hoster.load_clr();
	auto delegate = hoster.create_delegate("ConsoleApp.Klasse", "Load");

	using load_ptr = add_pointer<bool(const char* assemblyName)>::type;
	auto load = static_cast<load_ptr>(delegate);
	auto wahr = load("Das ist der Assembly name");
	
	return 0;
}
