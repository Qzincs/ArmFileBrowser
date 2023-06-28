# ArmFileBrowser
一个简易的网页ARM开发板文件浏览器，可在浏览器中访问ARM开发板上的目录和文件，支持查看文件的大小和类型，预览多种文件，编辑文本文件等功能。

该项目是运行在虚拟ARM平台上的一个C语言CGI程序，测试所用服务器为Boa。

# 主要功能
## 1. 浏览文件夹

![浏览文件夹](https://img1.imgtp.com/2023/06/28/hSSz3UfX.png)

## 2. 查看和编辑文本文件

![编辑文本文件](https://img1.imgtp.com/2023/06/28/R1dwIkhV.png))

## 3. 播放视频、音乐，查看图片和PDF文件

* 查看图片

![查看图片](https://img1.imgtp.com/2023/06/28/BrvpBMnH.png))
* 播放音乐

![播放音乐](https://img1.imgtp.com/2023/06/28/KOwaeDgA.png)
* 播放视频

![播放视频](https://img1.imgtp.com/2023/06/28/1xB6QSpm.png)
* 查看PDF文件

![Alt text](https://img1.imgtp.com/2023/06/28/chYOKkca.png)

# Boa服务器配置参考
```
Port 80
User root
Group root
ErrorLog /dev/console
AccessLog /dev/null
ServerName friendly-arm
DocumentRoot /www
DirectoryIndex index.html
KeepAliveMax 1000
KeepAliveTimeout 10
MimeTypes /etc/mime.types
DefaultType text/plain
CGIPath /bin
AddType application/x-httpd-cgi cgi
```

