package test;

import sns_mobile_xml.*;
import tce.*;
import test.TestServant;

 
public class TestMain extends ts_AsyncCallBack {
	
	@Override
	public void verify(ts_verify_r_Result_t result,RpcProxyBase proxy){
		
		System.out.println(String.format("async call back: code=%d",result.code));
	}	
	
	
	
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		tsProxy prx = tsProxy.createWithXML("localhost", 12001);
		ts_heartbeat_p_User_t user = new ts_heartbeat_p_User_t();
		user.id = "A-12300222-999000";
		RpcCommunicator.instance().init();
		try{

			//reversed callback
			TestServant servant = new TestServant();
			RpcCommAdapter adapter = RpcCommunicator.instance().
					createAdapter("terminal", RpcConsts.MSG_ENCODE_XML);
			adapter.addServant(servant);
			prx.conn.attachAdapter(adapter);
			
			//
			ts_verify_p_User_t p1 = new ts_verify_p_User_t();
			p1.token ="TOKEN-000000001";
			
			prx.heartbeat_oneway(user);
			
			ts_verify_r_Result_t vr;
			vr = prx.verify(p1, 0);
			vr = prx.verify(p1, 4000); //超时调用
			System.out.println(vr.code);
			
			
			 
			//async call
			TestMain asyncer = new TestMain();
			prx.verify_async(p1,asyncer);
			 
			
			
			Thread.sleep(1000*1000);
		}catch(Exception e){
			System.out.println(e.toString());
		}
		
		prx.destroy();
	}

}
