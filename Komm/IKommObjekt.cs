using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Komm
{
	[ComVisible(true), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
	public interface IKommObjekt
	{
		string Test(string eingabe);
	}
}
