package tce;
import tce.RpcMessage;
import tce.RpcCommunicator;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import java.io.*;
import java.util.*;


// msgcls.msg 用.分割 成 msgcls和msg， msgcls作为接口名称 , msg 作为接口内的函数名称

public class RpcMessageXML  extends RpcMessage{
	public String xml =""; //
	
	public String msg;	//消息类型，就是接口函数名称
	public String msgcls; //对应interface名称    , 需要xiake添加 消息大类 msgcls 字段
	public Element parentNode;	//父xml节点
	
	public RpcMessageXML(int calltype_){
		super(calltype_);
	}
	
	public static RpcMessage unmarshall(byte[] d){
		try{
			RpcMessageXML m;
			DocumentBuilderFactory factory =DocumentBuilderFactory.newInstance();
	        DocumentBuilder builder =factory.newDocumentBuilder();
	        ByteArrayInputStream is = new ByteArrayInputStream(d);  
	        Document dom = builder.parse(is);
	        Element root = dom.getDocumentElement();
	        if( root.getTagName() !="NaviMSG"){
	        	return null;
	        }
	        String cls_msg  = root.getAttribute("type"); //包含接口和方法名称 , 以dot分割
	       
	        String[] pair;
	        pair = cls_msg.split("\\.");
	        if(pair.length<2){
	        	cls_msg = "Terminal."+cls_msg; // 自己加上默认的本地Terminal接口类别
	        }
	        pair = cls_msg.split("\\.");
	       /* if( pair.length!=2){
	        	RpcCommunicator.instance().getLogger().error(String.format("invalid msg cls: %s",cls_msg));
	        	return null;
	        }*/
	        int calltype;
	        calltype = Integer.valueOf( root.getAttribute("calltype").trim()).intValue();
	        int seq;
	        seq = Integer.valueOf(root.getAttribute("sequence").trim()).intValue();
	        m = new RpcMessageXML(calltype);
	        m.sequence = seq;
	        m.msg = pair[1];
	        m.msgcls = pair[0];
	        m.parentNode = root;
	        return m;
		}catch(Exception e){
			RpcCommunicator.instance().getLogger().error( e.getMessage());
		}
		return null;
	}
	
	@Override
	public byte[] marshall(){
		String packet;
		packet = String.format("<NaviMSG msgcls=\"%s\" msg=\"%s\" type=\"%s\" calltype=\"%d\" sequence=\"%d\">",
					msgcls,msg,msg,this.calltype,this.sequence);
		packet+= xml;
		packet+= "</NaviMSG>";
		
		return packet.getBytes();
	}
}