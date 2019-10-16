using Library;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Modul
{
	public class Klasse
	{
		[return: MarshalAs(UnmanagedType.U1)]
		public static bool Load([MarshalAs(UnmanagedType.LPStr)] string assemblyName)
		{
			var aussen = new Aussen();
			Console.WriteLine($"Wurde aufgerufen aus Standard: {aussen.GetTest(assemblyName)}");
			return true;
		}
	}
}
