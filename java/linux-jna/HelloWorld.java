import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.ptr.IntByReference;

// 命令行执行  java -Djava.ext.dirs="/usr/xxx/lib/" HelloWorld  
// /usr/xxx/lib包含jna.jar
class NativeUtil
{
  public static abstract interface PCLibrary
    extends Library
  {
    public static final PCLibrary INSTANCE4 = (PCLibrary)Native.loadLibrary("/usr/lib/xxx.so", PCLibrary.class); 
    
    public abstract int genP10(int paramInt1);
  }
}
public class HelloWorld
{
	private static final String PCLibrary = null;

	public static void main(String[] args){
		System.out.println("this is instance:" + NativeUtil.PCLibrary.INSTANCE4);
	}
}





