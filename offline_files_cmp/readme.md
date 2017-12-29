# Readme

flst --sha1
    计算文件的SHA1值，如果现有的check-*.json数据库里已经有文件的sha1值，
    并且文件的大小和修改时间没有变动的话，会跳过。

flst --sha1 --force
    强制计算所有文件的SHA1值

