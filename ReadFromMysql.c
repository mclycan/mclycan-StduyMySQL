#include <stdio.h>
#include <mysql/mysql.h>
#include <string.h>

int main()
{
  MYSQL mysql;    //mysql连接
  MYSQL_RES *res; //这个结构代表返回行的一个查询结果集
  MYSQL_ROW row; //一个行数据的类型安全(type-safe)的表示
  char *query;  //查询语句    
  int t,r;
  mysql_init(&mysql);
  unsigned char mail[45];
  unsigned char pubkey[33];

  if (!mysql_real_connect(&mysql, "localhost", "root", "123456", "walletinfo", 0, NULL, 0))
  {
    printf( "Error connecting to database: %s", mysql_error(&mysql));
  }
  else
  {
    printf("Connected...\n");
  }
  query = "select * from info where id=2;";
  t = mysql_real_query(&mysql, query, (unsigned int)strlen(query));//执行指定为计数字符串的SQL查询。
  if(t)
  {
    printf("ERROR query: %s", mysql_error(&mysql));
  }
  printf("1111\n");
  res = mysql_store_result(&mysql);//检索完整的结果集至客户端。
  row = mysql_fetch_row(res);
  printf("2222\n");          
  sprintf(mail,"%s", row[1]);
  sprintf(pubkey,"%s", row[2]);
  printf("3333\n");
  printf("mail and pubkey is %s, %s .\n", mail, pubkey);
 
  mysql_free_result(res);//释放结果集使用的内存。
  mysql_close(&mysql);
  
  return 0;
}