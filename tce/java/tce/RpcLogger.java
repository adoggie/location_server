package tce;

public class RpcLogger {
	public static final int DEBUG =1 ;
	public static final int INFO = 2;
	public static final int ERROR = 3;
	
	public RpcLogger(){
		
	}
	
	public void debug(String msg){
		print(msg,DEBUG);
	}
	
	public void info(String msg){
		print(msg,INFO);
	}
	
	public void error(String msg){
		print(msg,ERROR);
	}
	
	void print(String msg ,int level){
		System.out.println(msg);
	}
}
