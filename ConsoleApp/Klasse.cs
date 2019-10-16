using Library;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace ConsoleApp
{
	public class Klasse
	{
		[return: MarshalAs(UnmanagedType.U1)]
		public static bool Load([MarshalAs(UnmanagedType.LPStr)] string assemblyName)
		{
			var aussen = new Aussen();
			Console.WriteLine($"Wurde aufgerufen: {aussen.GetTest(assemblyName)}");
			return true;
		}
	}
}
