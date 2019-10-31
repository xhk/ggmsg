using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using nggmsg;

namespace nggmsgTest
{
	class Program
	{
		static void Main(string[] args)
		{
			Channel c = new Channel();
			c.OnPassiveConnect += OnPassiveConnect;
			c.OnPositiveConnect += OnPositiveConnect;
			//c.OnPassiveConnect += OnPassiveConnect1;
			int nServiceID = 10;
			short port = 9010;
			if (args[0] == "server")
			{
				Console.WriteLine("Server Start");
				c.OnReceiveMsg += OnServerReceiveMsg;
				c.Start(nServiceID, port);
			}
			else
			{
				Console.WriteLine("Client Start");
				c.OnReceiveMsg += OnClientReceiveMsg;
				c.Connect("127.0.0.1", port);
				var msg = "hello server";
				for (int i = 0; i < 100; ++i)
				{
					Thread.Sleep(2000);
					c.SendToService(nServiceID, Encoding.ASCII.GetBytes(msg));
				}
			}

			Console.ReadKey();
			c.Stop();
		}

		public static void OnPositiveConnect(int nServiceID, int nConnectID) {
			Console.WriteLine("OnPositiveConnect");
		}

		public static void OnPassiveConnect(int nServiceID, int nConnectID){
			Console.WriteLine("OnPassiveConnect");
		}
		public static void OnPassiveConnect1(int nServiceID, int nConnectID)
		{
			Console.WriteLine("OnPassiveConnect1");
		}
		public static void OnServerReceiveMsg(int nServiceID, int nConnectID, byte [] msg)
		{
			Console.WriteLine($"ServiceID:{nServiceID},ConnectID:{nConnectID},msg:{Encoding.ASCII.GetString(msg)}");
		}

		public static void OnClientReceiveMsg(int nServiceID, int nConnectID, byte[] msg)
		{
			Console.WriteLine($"ServiceID:{nServiceID},ConnectID:{nConnectID},msg:{Encoding.ASCII.GetString(msg)}");
		}


	}
}
