package legrand.steve.bindsocket;

import android.app.Activity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity {
    private TextView tv_result;
    private Button bt_lsmod;
    private Button bt_iptable;
    private String cmd=null;
    private SocketClient client;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tv_result= findViewById(R.id.textview);
        bt_lsmod=findViewById(R.id.bt_lsmod);
        bt_iptable=findViewById(R.id.bt_iptables);
        tv_result.setMovementMethod(ScrollingMovementMethod.getInstance());
        client=new SocketClient();
        bt_lsmod.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                cmd=bt_lsmod.getText().toString();
                final String result=client.execute(cmd);
                if(result!=null)
                    tv_result.setText(result);
                else
                    tv_result.setText("null");

                if(client!=null)
                    client.disconnect();
            }
        });

        bt_iptable.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                cmd=bt_iptable.getText().toString();
                final String result=client.execute(cmd);
                if(result!=null)
                    tv_result.setText(result);
                else
                    tv_result.setText("null");

                if(client!=null)
                    client.disconnect();
            }
        });
    }
    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub
        super.onDestroy();
        if(client!=null)
            client.disconnect();
    }
}