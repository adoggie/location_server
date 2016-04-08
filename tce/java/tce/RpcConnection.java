/**
 * 
 * connection‰ºö‰∫ßÁîüÊ≤°ÊúâreturnÂõûÈ¶àÊ∂àÊÅØÔºåËøô‰∫õÊ∂àÊÅØË¢´Â†ÜÁßØÂú®Êú¨Âú∞ÔºåÂØºËá¥ÂÜÖÂ≠òÊ≥ÑÈú≤
 * ÈúÄË¶ÅÊèê‰æõÂÆöÊó∂ÂõûÊî∂Ëøô‰∫õÊ≤°ÊúâÂèçÈ¶àÁöÑÂéüÂßãÊ∂àÊÅØÂØπË±° 
 * 
 * 
 */

package tce;
	

import java.net.*;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.io.*;
import java.util.*;

import tce.RpcCommAdapter;
import tce.RpcCommunicator;
import tce.RpcMessage;



public class RpcConnection{
	
	RpcCommAdapter _adapter = null;
	Hashtable<Integer,RpcMessage> _msglist = new Hashtable<Integer,RpcMessage>();
	
	
	String _host;
	int 	_port;
	

	RpcConnection(String host,int port){
		_host = host;
		_port = port;
	}
	
	
	public boolean connect() {
		return false;
	}
	
	public void close(){
		
	}
	
	
	void onError(){
		
	}
	
	
	
	public void attachAdapter(RpcCommAdapter adapter){
		_adapter = adapter;
	}
	
	void  onConnected(){
	}
	
	//clear up and free wait mutex for resuming user thread-control
	void onDisconnected(){
		synchronized(this._msglist){
			for( RpcMessage m: _msglist.values()){
				try{
					synchronized(m){
						m.errcode = RpcConsts.RPCERROR_CONNECTION_LOST;
						m.notify();
					}
				}catch(Exception e){
					RpcCommunicator.instance().getLogger().error( 
							e.getMessage()
							);
				}
			}
			_msglist.clear();
		}
	}
	
	
	public final boolean sendMessage(RpcMessage m){
		synchronized(this._msglist){
			_msglist.put(m.sequence,m);
		}
		boolean r =  sendDetail(m);
		if(!r){
			synchronized(this._msglist){
				_msglist.remove(m.sequence);
			}
		}
		return r;
	}
	
	boolean sendDetail(RpcMessage m){
		return false;
	}
	
	void doReturnMsg(RpcMessage m2){
		RpcMessage m1 = null;
		synchronized(this._msglist){
			Integer key = new Integer(m2.sequence);
			
			if( _msglist.containsKey(key) ){
				m1 = _msglist.get(key); 
				_msglist.remove(key);
			}
		}
		if(m1!=null){
			if(m1.async !=null){
				m1.async.callReturn(m1,m2); //异步调用
			}else{
				synchronized(m1){
					m1.result = m2; // assing to init-caller 
					m1.notify();
				}
			}
		}
	}
	
	void dispatchMsg(RpcMessage m){
		if( (m.calltype&RpcMessage.CALL) !=0){
			if(_adapter !=null){
				_adapter.dispatchMsg(m);
			}
		}
		if( (m.calltype&RpcMessage.RETURN)!=0){
			this.doReturnMsg(m);
		}
	}
	

	
}

