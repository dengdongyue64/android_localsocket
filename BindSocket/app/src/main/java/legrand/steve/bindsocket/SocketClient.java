package legrand.steve.bindsocket;

import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class SocketClient {
    private static  final String TAG = "MySocketClient";
    private static final  boolean LOCAL_DEBUG = true;

    private InputStream mIn;
    private OutputStream mOut;
    private LocalSocket mSocket;
    private byte buf[] = new byte[1024];

    public SocketClient(){
    }

    public synchronized  String transact(String cmd) {
        String result = null;
        if (LOCAL_DEBUG) {
            Log.d(TAG, "send " + cmd + "");
        }
        if (connet() && writeCommand(cmd)) {
            final  int replyLenth = readReply();
            if(replyLenth > 0){
                result = new String(buf,0,replyLenth);
                if (LOCAL_DEBUG){
                    Log.d(TAG,"recv: " + result + "");
                }
            }
        } else {
            if (LOCAL_DEBUG) {
                Log.d(TAG, "connect or write command failed !");
            }
        }
        return result;
    }

    public String execute(String cmd){
        return transact(cmd);
    }
    private boolean connet(){
        if(mSocket != null){
            return true;
        }
        Log.d(TAG,"conecting  .......");
        try {
            mSocket = new LocalSocket();
            LocalSocketAddress address = new LocalSocketAddress("mysocket",LocalSocketAddress.Namespace.RESERVED);
            mSocket.connect(address);
            mIn = mSocket.getInputStream();
            mOut = mSocket.getOutputStream();
        }catch (IOException ex){
            disconnect();
            return  false;
        }
        return true;
    }

    public void  disconnect(){
        Log.d(TAG,"disconnecting .....");
        try {
            if (mSocket!=null)
                mSocket.close();
        }catch (IOException ex){
        }

        try {
            if(mIn != null)
                mIn.close();
        }catch (IOException ex){
        }

        try {
            if(mOut != null)
                mOut.close();
        }catch (IOException ex){
        }

        mSocket = null;
        mIn = null;
        mOut = null;
    }

    private boolean readFully(byte[] buffer, int len) {
        int off = 0, count;
        if (len < 0)
            return false;
        while (off != len) {
            try {
                count = mIn.read(buffer, off, len - off);
                if (count <= 0) {
                    Log.d(TAG, "read error " + count);
                    break;
                }
                off += count;
            } catch (IOException ex) {
                Log.d(TAG, "read exception");
                break;
            }
        }
        Log.d(TAG, "read " + len + " bytes");
        if (off == len)
            return true;
        disconnect();
        return false;
    }

    private int readReply() {
        if (!readFully(buf, 2)) {
            return -1;
        }

        final int len = (((int) buf[0]) & 0xff) | ((((int) buf[1]) & 0xff) << 8);

        if ((len < 1)) {
            Log.d(TAG, "invalid reply length (" + len + ")");
            disconnect();
            return -1;
        }
        if(len > buf.length){
            buf = new byte[len];  //重新扩容
        }

        if (!readFully(buf, len)) {
            return -1;
        }

        return len;
    }

    private boolean writeCommand(String cmdString) {
        final byte[] cmd = cmdString.getBytes();
        final int len = cmd.length;
        if ((len < 1) || (len > buf.length)) {
            return false;
        }

        buf[0] = (byte) (len & 0xff);
        buf[1] = (byte) ((len >> 8) & 0xff);
        try {
            mOut.write(buf, 0, 2);
            mOut.write(cmd, 0, len);
        } catch (IOException ex) {
            Log.d(TAG, "write error");
            disconnect();
            return false;
        }
        return true;
    }
}
