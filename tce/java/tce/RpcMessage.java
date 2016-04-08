
package tce;

import tce.RpcConsts;
import java.util.*;
import java.nio.ByteBuffer;

import tce.RpcAsyncCallBackBase;
//import tce.RpcServantDelegate;
//import tce.RpcException;
import tce.RpcProxyBase;
import tce.RpcConnection;


public class RpcMessage{
	public static final  int CALL = 0x01;		
	public static final  int RETURN = 0x02;
	public static final  int TWOWAY = 0x10;
	public static final  int ONEWAY = 0x20;
	public static final  int ASYNC = 0x40;
	
	public static final String EnvelopBegin="<NaviMSG";
	public static final String EnvelopClose="</NaviMSG>";
	public static final  int MAXPACKET_SIZE = 1024*10;
	
	
	public Vector<byte[]> params = new Vector<byte[]>();
	public int type = RpcConsts.MSGTYPE_RPC;
	
	public int sequence = 0;
	public int calltype = 0;
	public int ifidx = 0;
	public int opidx = 0;
	public int errcode = RpcConsts.RPCERROR_SUCC;
	
	public int paramsize = 0; //Ê∂àÊÅØÂÜÖÈÉ®Â≠òÂú®Â§öÂ∞ëÂèÇÊï∞
	public byte[] paramstream = null;
	public RpcProxyBase prx = null;
	
	public long timeout;	//Ë∂ÖÊó∂ÈôêÂà∂
	public long issuetime;	//ÂèëÈÄÅÊó∂Èó¥
	public RpcAsyncCallBackBase async = null;
	public RpcConnection conn = null;
	//public Object cookie = null; //delicious teast
	public RpcMessage result = null ;//同步调用返回消息,返回给等待的调用者
	
	public  RpcMessage(int calltype_){
		calltype = calltype_;
		if( (calltype & CALL) !=0){
			sequence = RpcCommunicator.instance().getUniqueSequence();
		}
	}
	
	public void addParam(byte[] bytes){
		this.params.add(bytes);
	}
	
	public byte[] marshall(){
		ByteBuffer buf = ByteBuffer.allocate(0);
		for(byte[] bytes: this.params){
			buf.put(bytes);
		}
		return buf.array();
	}
	
	
	public static RpcMessage unmarshall(byte[] d){
		RpcMessage m = null ;		
		/*
		try{
			m = new RpcMessage();
			m.type = d.readUnsignedByte();
			m.sequence = d.readUnsignedInt();
			m.calltype = d.readUnsignedByte();
			m.ifidx = d.readUnsignedShort();
			m.opidx = d.readUnsignedShort();
			m.errcode = d.readInt();
			m.paramsize = d.readUnsignedByte(); //ÂèÇÊï∞‰∏™Êï∞
			m.paramstream = new ByteArray();
			m.paramstream.writeBytes(d,d.position,d.bytesAvailable);
			m.paramstream.position = 0;
			return m;
			
		}catch(e:Error){
			trace(e.toString());
		}
		return null;
		//return m;
			
		}
	
		*/
		return m;
	}
		
}

