using System;
using System.Collections.Generic;
using System.Text;

namespace Modul.Interop
{
	public class Factory : IFactory
	{
		public static Factory Create() => new Factory();
		static Factory()
		{
		}

		public string GetAffe() => "Das ist ein Affe";

		//public IHashPassword HashPassword() => new HashPasswordInstance();
	}
}
