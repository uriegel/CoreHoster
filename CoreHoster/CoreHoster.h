// Retrieved from https://github.com/dotnet/coreclr/blob/master/src/coreclr/hosts/inc/coreclrhost.h

// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
#pragma once
#include <string>
#include <optional>

namespace Core_hoster {
	using namespace std;
	struct Configuration {
		string Runtime_path;
		string Assembly_name;
		string Clr_version = "3.0.0"s;
		string app_domain_friendly_name = "CoreHoster"s;
		optional<string> Core_clr_path;
	};

	class Hoster {
	public:
		Hoster(const Configuration& configuration) 
			: configuration(configuration) {}
		~Hoster() { shutdown(); }

		void load_clr();
		void* create_delegate(const string& type_name, const string& method_name);
		void shutdown();
	private:
		string build_tpa_list(const string& core_path);

		Configuration configuration;
		HMODULE coreClr{ nullptr };
		void* host_handle{ nullptr };
		unsigned int domain_id{ 0 };
	};
}