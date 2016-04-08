
/*
 * adapter 借助connection实现消息发送和接收
 * xml的rpc在连接上只允许一个adapter存在
 * 而adapter可以允许多个connnection
 * */

package tce;


import java.util.*;
import tce.RpcServant;
import tce.RpcServantDelegate;
import tce.RpcMessage;
import tce.RpcMessageXML;

public class RpcCommAdapter{
	String _id;
	
	
	RpcCommAdapter(String id){
		_id = id;
	}
	
	boolean open(String host,int port){
		return false;
	}
	
	
	public void  addServant(RpcServant servant){
		
	}
	
	void doError(int errcode,RpcMessage m){
		RpcConnection conn = m.conn;
		/*
		var sm:RpcMessageReturn = new RpcMessageReturn();
		sm.sequence = m.sequence;
		sm.errcode = errcode;
		conn.sendMessage(sm);
		*/
	}
	
	void close(){
		
	}
	
	void join(){
		
	}
	
	void dispatchMsg(RpcMessage m){
		
	}
	
}


/*
public Object invokeMethod(Object owner, String methodName, Object[] args) throws Exception {     
  
 Class ownerClass = owner.getClass();     
  
 Class[] argsClass = new Class[args.length];     
  
 for (int i = 0, j = args.length; i < j; i++) {     
     argsClass[i] = args[i].getClass();     
 }     

  Method method = ownerClass.getMethod(methodName,argsClass);     
  
 return method.invoke(owner, args);     
	}    

	 */
	
	/*
	if(! dg.optlist.containsKey( m.opidx ) ){
		doError(RpcConsts.RPCERROR_INTERFACE_NOTFOUND,m,conn);
		return;
	}
	var func:Function;
	func = dg.optlist.getValue(m.opidx) as Function;
	var ctx:RpcContext = new RpcContext();
	
	ctx.conn = conn;
	
	ctx.msg = m;
	try{
		func( ctx );
	}catch(e:Error){
		doError(RpcConsts.RPCERROR_REMOTEMETHOD_EXCEPTION,m,conn)
		return;
	}
	*/
