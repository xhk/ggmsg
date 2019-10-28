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
			if(args[0] == "server")
			{
				Channel c = new Channel();
				c.Start(11, 9100, OnPassiveConnect, OnServerReceiveMsg);
				for (int i = 0; i < 100; ++i)
				{
					
				}
				Console.ReadKey();
				c.Stop();
			}
			else
			{
				Channel client = new Channel();
				client.Connect("127.0.0.1", 9100, OnPositiveConnect, OnClientReceiveMsg);
				var msg = "hello server";
				for (int i = 0; i < 100; ++i)
				{
					Thread.Sleep(2000);
					client.SendToService(11, Encoding.ASCII.GetBytes(msg));
				}
				client.Stop();
			}


		}

		public static void OnPositiveConnect(int nServiceID, int nConnectID) {
			Console.WriteLine("OnPositiveConnect");
		}
		public static void OnPassiveConnect(int nServiceID, int nConnectID){
			Console.WriteLine("OnPassiveConnect");
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
