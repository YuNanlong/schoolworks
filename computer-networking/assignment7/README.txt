README

客户端和服务端的开发环境为Ubuntu16.04，运行环境为Linux。

因为客户端的图形界面使用了第三方库GTK+2.0，如果需要编译客户端的源代码需要安装GTK+2.0库，Ubuntu16.04环境中安装GTK+2.0的方法如下：
apt-get install build-essential
apt-get install gnome-core-devel
apt-get install pkg-config
apt-get install devhelp
apt-get install libglib2.0-doc libgtk2.0-doc
apt-get install glade libglade2-dev
apt-get install libgtk2.0*
编译命令为：
gcc client.c -o client -lpthread `pkg-config --cflags --libs gtk+-2.0`

服务端源代码的编译命令为：
gcc server.c -o server -lpthread