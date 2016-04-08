
package test;

import sns_mobile_xml.*;
import tce.*;

public class TestServant extends Terminal {
	public TestServant(){
		super();
	}
	@Override
	public void hello(RpcContext ctx){
		System.out.println("reversed call, hello()");
		
	}	

}
