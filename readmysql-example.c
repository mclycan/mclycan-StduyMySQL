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

  res = mysql_store_result(&mysql);//检索完整的结果集至客户端。

  while(row = mysql_fetch_row(res))
  {                           
    for(t = 0; t < mysql_num_fields(res); t++)
    {
      printf("%s\t", row[t]);
    }
    printf("\n");
  }
  mysql_free_result(res);//释放结果集使用的内存。
  mysql_close(&mysql); //
  
  return 0;
}