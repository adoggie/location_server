

package tce;

import tce.RpcConnection;
import tce.RpcCommAdapter;
import tce.RpcCommAdapterXML;
import tce.RpcConnection_SocketXML;
import tce.RpcLogger;
import tce.RpcConnection_Http;
import tce.RpcConnection_Socket;
import java.util.*;

public class RpcCommunicator{
	private static RpcCommunicator _handle = null;
	
	//private Vector<RpcConnection> _conns = new Vector<RpcConnection>();
	private RpcLogger _logger = null;
	private Hashtable<String,RpcCommAdapter> _adapters;
	private int _seq  =0;
	
	public RpcCommunicator(){
		setLogger(new RpcLogger());
	}
	
	public static  RpcCommunicator instance(){
		if(_handle == null){
			_handle = new RpcCommunicator();
		}
		return _handle;
			
	}
	
	public void setLogger(RpcLogger logger){
		_logger = logger;
	}
	
	public RpcLogger getLogger(){
		return _logger;
	}
	
	public int getUniqueSequence(){
		if(_seq >= Integer.MAX_VALUE - 0xffff){
			_seq = 0;
		}
		return ++_seq;
	}
	
	/*
	synchronized
	public void addConnection(RpcConnection conn){
		_conns.add(conn);
	}
	
	synchronized 
	public void removeConnection(RpcConnection conn){		
		for(RpcConnection n:this._conns){
			if( n == conn){
				_conns.remove(conn);
				break;
			}					
		}
	
	}
	*/
	
	public boolean init(){
		return true;
	}
	
	/**
	 * 
	 * waitForShutdown()
	 * @param sync 
	 * 	true - blocked until communicator shutdown completely
	 * 	false - create new thread and wait   
	 * 
	 * @return
	 * @exception
	 */
	
	public  int waitForShutdown(boolean sync){
		for(RpcCommAdapter adapter:_adapters.values()){
			adapter.join();
		}
		return 0;
	}
	
	public void shutdown(){
		for(RpcCommAdapter adapter:_adapters.values()){
			adapter.close();
		}
	}
	
	/**
	 * 
	 * @param type - MSG_ENCODE_XML default
	 */
	public RpcCommAdapter createAdapter(String id,int type){
		//RpcCommAdapter adapter = null;
		if( type == RpcConsts.MSG_ENCODE_XML){
			return new RpcCommAdapterXML(id);
		}
		return  new RpcCommAdapter(id);
	}
	
	public void addAdatper(RpcCommAdapter adapter){
		_adapters.put(adapter._id, adapter);
	}
	
	
	public RpcConnection createConnection(int type,String host,int port){
		if( (type&RpcConsts.CONNECTION_SOCK) !=0 ){
			if( (type&RpcConsts.MSG_ENCODE_XML)!=0){
				return new RpcConnection_SocketXML(host,port);
			}
			return new RpcConnection_Socket(host,port);
		}
		if( (type&RpcConsts.CONNECTION_HTTP)!=0){
			return new RpcConnection_Http(host,port);
		}
		return null;
	}
	
}
		
