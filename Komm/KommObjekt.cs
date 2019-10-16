using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Komm
{
	[ComVisible(true), ClassInterface(ClassInterfaceType.None)]
	public class KommObjekt : IKommObjekt
	{
		public KommObjekt()
		{
		}

		public string Test(string eingabe) => $"Ausgabe: {eingabe}";
	}
}
