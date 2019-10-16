#include <Windows.h>
#include <array>
#include "CoreHoster.h"
#import "..\Modul.Interop\bin\Debug\netstandard2.0\Modul.Interop.tlb"
using namespace Core_hoster;
using namespace Modul_Interop;

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
	runtimePath += R"(\Modul\bin\Debug\netstandard2.1)"s;

	Hoster hoster({
		runtimePath,
		"Modul.dll"s
		//"2.1.0"s
	});

	
	hoster.load_clr();

	auto delegate = hoster.create_delegate("Modul.Klasse", "Load");
	using load_ptr = add_pointer<bool(const char* assemblyName)>::type;
	auto load = static_cast<load_ptr>(delegate);
	auto wahr = load("Das ist der Assembly name");
	
	delegate = hoster.create_delegate("Modul.Interop.Factory", "Create");
	using create_ptr = add_pointer<IUnknown*()>::type;
	auto create = static_cast<create_ptr>(delegate);
	auto unke = reinterpret_cast<IUnknown*>(create());

	IFactoryPtr factory;
	auto hr = unke->QueryInterface(&factory);
	auto aua = factory->GetAffe();
	SysFreeString(aua);

	return 0;
}
