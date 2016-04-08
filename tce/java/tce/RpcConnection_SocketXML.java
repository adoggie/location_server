package tce;

import java.io.InputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.io.*;

import tce.RpcCommAdapter;
import tce.RpcCommunicator;
import tce.RpcMessage;
//import tce.RpcConnection;
import tce.RpcMessageXML;

import tce.RpcConnection_Socket;

public class RpcConnection_SocketXML extends  RpcConnection_Socket  implements Runnable {

	Socket 	_sock = null;
	
	ByteBuffer _buff;
	RpcCommAdapter _adapter = null;
	Thread 	_workthread = null;;
	
	RpcConnection_SocketXML(String host,int port){
		super(host,port);
		//_sock = new Socket();
	}
	
	public void run(){
		try{
			//BufferedReader in = new BufferedReader( new InputStreamReader(_sock.getInputStream()));
			String strbuf ="";
			byte[] d = new byte[1024*5];
			InputStream is = _sock.getInputStream();
			int len;
			while (true){				
				len = is.read(d);
				if(len < 0 ){
					_sock.close();
					break;
				}
				strbuf+= new String(d,0,len,Charset.forName("UTF-8"));
				int first,last;
				first = strbuf.indexOf(RpcMessage.EnvelopBegin);
				last = strbuf.indexOf(RpcMessage.EnvelopClose);
				if(first == -1){
					if(last == -1){
						strbuf = ""; //无有效消息报文						
					}else{
						strbuf = strbuf.substring(last + RpcMessage.EnvelopClose.length());
					}
					continue;
				}else{
					if( first!=0){
						strbuf = strbuf.substring(first);
					}
					if(last == -1 ){
						if(strbuf.length() > RpcMessage.MAXPACKET_SIZE){ // 包过大
							strbuf = "";
							_sock.close();
							RpcCommunicator.instance().getLogger().error("packet size too large! closesocket()");
						}
						continue;
					}
					// first and last okay,do parse
					String xml = strbuf.substring(0,last+RpcMessage.EnvelopClose.length());
					strbuf = strbuf.substring(last+RpcMessage.EnvelopClose.length());
					if(!decodeMsg(xml) ){
						_sock.close();
						RpcCommunicator.instance().getLogger().error("decode message exception! closesocket()");
					}
				}// end fi				
			} // -- end while()
			
		}catch(Exception e){
			RpcCommunicator.instance().getLogger().error("connection lost! \n detail:" + e.toString());
		}
		RpcCommunicator.instance().getLogger().debug("connection thread exiting...");
		onDisconnected();
	}
	
	@Override
	public boolean connect(){
		InetSocketAddress ep = new InetSocketAddress(_host,_port);
		try{
			_sock = new Socket();
			_sock.connect(ep);
			_workthread = new Thread(this);
			_workthread.start();
			return true;
		}catch(Exception e){
			RpcCommunicator.instance().getLogger().error(e.getMessage());
		}
		_sock = null;
		return false;
	}
	
	@Override
	void onDisconnected(){
		super.onDisconnected();
		_sock = null;
	}
	
	private boolean decodeMsg(String xml){
		RpcMessage m;
		m = RpcMessageXML.unmarshall(xml.getBytes());
		if(m!=null){
			m.conn = this;
			this.dispatchMsg(m);
		}
		return true;
	}

	@Override
	public void close(){
		try{
			if( _sock == null){
				return;
			}
			_sock.close();
			_sock = null;
			if(_workthread !=null){
				_workthread.interrupt();
				_workthread.join();
				_workthread = null;
			}
			
		}catch(Exception e){
			RpcCommunicator.instance().getLogger().error(e.getMessage());
		}
	}
	
	@Override
	boolean  sendDetail(RpcMessage m){
		try{
			if( _sock == null){
				if( !connect() ){
					return false;
				}
			}
			OutputStream os;
			os = _sock.getOutputStream();
			RpcMessageXML xml = (RpcMessageXML) m;
			os.write(xml.marshall());
			
		}catch(Exception e){
			RpcCommunicator.instance().getLogger().error(e.getMessage());
			return false;
		}
		return true;
	}
	
	
	
}
