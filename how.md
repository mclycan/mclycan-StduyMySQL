# 钱包密码恢复之MySQL存储和提取信息 #
环境：Ubuntu 14.04 64位
### C语言操作mysql的demo： ###



1. **linux安装mysql环境**： 

		sudo apt-get install mysql-server mysql-client
		sudo apt-get install libmysqlclient15-dev




1. **示例代码**：
		
		#include <stdlib.h>
		#include <stdio.h>
		#include <mysql/mysql.h>

		int main(int argc, char *argv[]) 
		{
    		MYSQL my_connection;
    		int res;
    		mysql_init(&my_connection);
     		//"localhost", "root", "123456", "mysql" : ip, user, passwd, database;
    		if (mysql_real_connect(&my_connection, "localhost", "root", "123456", "mysql", 0, NULL, 0)) 
    		{
        		printf("Connection success\n");
        		res = mysql_query(&my_connection, "INSERT INTO wallet(fname, age) VALUES('david', 8)");
        
        		if (!res) 
        		{
           			printf("Inserted %lu rows\n", (unsigned long)mysql_affected_rows(&my_connection));
        		} 
        		else 
        		{
           			fprintf(stderr, "Insert error %d: %s\n", mysql_errno(&my_connection),
            		mysql_error(&my_connection));
        		}
        
        		mysql_close(&my_connection);
    		} 
    		else 
    		{
        		fprintf(stderr, "Connection failed\n");
        		if (mysql_errno(&my_connection)) 
        		{
           			fprintf(stderr, "Connection error %d: %s\n",
            		mysql_errno(&my_connection), mysql_error(&my_connection));
        		}
    		}
        
    		return EXIT_SUCCESS;
		}		
	


1. **编译**：

		gcc -I/usr/include/mysql test.c -L/usr/lib/mysql -lmysqlclient -o app

	

----------
### 钱包信息提取及存储： ###


1. **下载并安装Berkeley**： http://download.oracle.com/berkeley-db/db-4.8.30.NC.tar.gz
		
		tar xvfz db-4.8.30.NC.tar.gz
		cd db-4.8.30.NC/build_unix
		../dist/configure
		sudo make
		sudo make install
	缺省安装Berkeley会被安装到/usr/local目录中，你可以选择你自己的安装路径。比如 /opt/Berkeley，后面链接头文件和库目录时也要相应改变。

1. **代码**：
		
		详见 ExtractInfoToMysql.c

	数据库信息可以按要求修改

1. **编译**：
	
		gcc -I/usr/include/mysql -I/usr/local/BerkeleyDB.4.8/include ExtractInfoToMysql.c -L/usr/lib/mysql -lmysqlclient -L/usr/local/BerkeleyDB.4.8/lib -ldb-4.8 -o ExtractInfoToMysql

		gcc -I/usr/include/mysql -I/usr/local/BerkeleyDB.4.8/include -I/usr/local/include Extractinfo.c -L/usr/local/lib -lssl -lcrypto -L/usr/lib/mysql -lmysqlclient -L/usr/local/BerkeleyDB.4.8/lib -ldb-4.8 -o Extractinfo

1. **数据库设置**：

		sudo mysql -p123456
		> create database walletinfo;
		> show databases;
		> use walletinfo;
	
	创建表及其结构：
		
		create table info(
		id INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
		mail VARCHAR(50) NOT NULL,
		pubkey VARCHAR(100) NOT NULL, 
		encsec VARCHAR(200) NOT NULL, 
		encmas VARCHAR(200) NOT NULL, 
		salt VARCHAR(20) NOT NULL, 
		method INT(2), 
		rounds INT(8) 
		)AUTO_INCREMENT=1;
	注意输入法标点的半角和全角


1. **执行**：
		
		./ExtractInfoToMysql mc.dat



1. **结果**：

	![](http://i3.tietuku.com/5a914f9a43faa38b.png)
	其中id是自增长，方便取数据

----------
### 从数据库提取数据： ###



1. **代码**：

		详见 ReadFromMysql.c 或者 readmysql-example.c



1. **编译**：
	
		gcc -I/usr/include/mysql ReadFromMysql.c -L/usr/lib/mysql -lmysqlclient -o ReadFromMysql





安装openssl
sudo apt-get install openssl 
sudo apt-get install libssl-dev
