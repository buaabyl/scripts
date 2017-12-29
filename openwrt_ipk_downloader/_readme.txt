update-ipk.py

    这个脚本可以离线Openwrt的包，通过解析Packages.gz得到列表，然后逐个下载
    如果已有文件的MD5Sum不一致，那么会重新下载，如果一致则跳过。

check-ipk.py

    检查是否有文件不再Packages.gz里，最后生成一个删除的批处理。

