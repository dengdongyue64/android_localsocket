需要在init.rc  加入 启动服务，并创建 mysocket 用于进程间通信
service mysocket /system/bin/mysocket
   class main
   socket mysocket stream 0666 root system
   oneshot

BindSocket  :         java app 
bindsocket_C_client : c demo client
mysocketservice     : c demo server
