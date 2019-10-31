using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace nggmsg
{
    public class Channel
    {
		public event FnOnPassiveConnect OnPassiveConnect;
		public event FnOnPassiveDisConnect OnPassiveDisConnect;

		public event FnOnPositiveConnect OnPositiveConnect;
		public event FnOnPositiveDisConnect OnPositiveDisConnect;
		public event FnOnReceiveMsg OnReceiveMsg;

		private long handle;
		public Channel()
		{
			handle = ggmsg_Create();
		}

		 ~Channel() {
			if (handle > 0)
			{
				ggmsg_Destory(handle);
			}
		}
		public void Start(int nServiceID, short port)
		{
			ggmsg_Start(handle, nServiceID, port, OnPassiveConnect, OnPassiveDisConnect, InternalOnReceiveMsg);
		}

		public bool Connect(string strHost, short port)
		{
			var pszHost = Marshal.StringToHGlobalAnsi(strHost);
			var nRet = ggmsg_Connect(handle, pszHost, port, OnPositiveConnect, OnPositiveDisConnect, InternalOnReceiveMsg);
			Marshal.FreeHGlobal(pszHost);
			return nRet == 0;
		}

		public void Stop()
		{
			ggmsg_Stop(handle);
		}

		public bool SendToService(int nServiceID, byte [] msg)
		{
			var pMsg = Marshal.AllocHGlobal(msg.Length);
			var p = pMsg;
			for(int i = 0; i < msg.Length; ++i)
			{
				Marshal.WriteByte(p, msg[i]);
				p += 1;
			}
			int nRet = ggmsg_SendToService(handle, nServiceID, pMsg, msg.Length);
			Marshal.FreeHGlobal(pMsg);
			return nRet == 0;
		}

		public bool SendToConnect(int nConnectID, byte[] msg)
		{
			var pMsg = Marshal.AllocHGlobal(msg.Length);
			var p = pMsg;
			for (int i = 0; i < msg.Length; ++i)
			{
				Marshal.WriteByte(p, msg[i]);
				p += 1;
			}

			int nRet = ggmsg_SendToConnect(handle, nConnectID, pMsg, msg.Length);
			Marshal.FreeHGlobal(pMsg);
			return nRet == 0;
		}

		private void InternalOnReceiveMsg(int nServiceID, int nConnectID, IntPtr pMsg, int nMsgLen)
		{
			var msg = new byte[nMsgLen];
			var p = pMsg;
			for(int i = 0; i < nMsgLen; ++i)
			{
				msg[i] = Marshal.ReadByte(p);
				p += 1;
			}

			OnReceiveMsg?.Invoke(nServiceID, nConnectID, msg);
		}

		public delegate void FnOnReceiveMsg(int nServiceID, int nConnectID, byte[] msg);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		public delegate void FnOnPositiveConnect(int nServiceID, int nConnectID);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		public delegate void FnOnPassiveConnect(int nServiceID, int nConnectID);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		public delegate void FnOnPositiveDisConnect(int nServiceID, int nConnectID);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		public delegate void FnOnPassiveDisConnect(int nServiceID, int nConnectID);

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void InternalFnOnReceiveMsg(int nServiceID, int nConnectID, IntPtr pMsg, int nMsgLen);

		[DllImport("ggmsg.dll", CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
		private static extern long ggmsg_Create();

		[DllImport("ggmsg.dll", CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
		private static extern void ggmsg_Destory(long c);

		[DllImport("ggmsg.dll", CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
		private static extern void ggmsg_Start(
			long c, int nServiceID, short port,
			FnOnPassiveConnect fnOnPassiveConnect,
			FnOnPassiveDisConnect fnOnPassiveDisConnect,
			InternalFnOnReceiveMsg fnOnReceiveMsg);

		[DllImport("ggmsg.dll", CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
		private static extern int ggmsg_Connect(
			long c, IntPtr pszHost, short port,
			FnOnPositiveConnect fnOnPositiveConnect,
			FnOnPositiveDisConnect fnOnPositiveDisConnect,
			InternalFnOnReceiveMsg fnOnReceiveMsg);

		[DllImport("ggmsg.dll", CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
		private static extern void ggmsg_Stop(long c);

		[DllImport("ggmsg.dll", CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
		private static extern int ggmsg_SendToService(long c, int nServiceID, IntPtr pMsg, int nMsgLen);

		[DllImport("ggmsg.dll", CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
		private static extern int ggmsg_SendToConnect(long c, int nConnectID, IntPtr pMsg, int nMsgLen);
	}
}
