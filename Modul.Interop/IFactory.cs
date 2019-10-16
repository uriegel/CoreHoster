using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Modul.Interop
{
	[ComVisible(true), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
	public interface IFactory
	{
		#region Interfaces

		string GetAffe();
		//IHashPassword HashPassword();

		#endregion
	}
}
