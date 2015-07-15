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
  unsigned char encsec[48];
  unsigned char encmas[48];
  unsigned char salt[8];
  unsigned char  method[2];
  unsigned char  rounds[10];

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
  row = mysql_fetch_row(res);
    
  sprintf(mail,"%s", row[1]);
  sprintf(pubkey,"%s", row[2]);
  sprintf(encsec,"%s", row[3]);
  sprintf(encmas,"%s", row[4]);
  sprintf(salt,"%s", row[5]);
  sprintf(method,"%s", row[6]);
  sprintf(rounds,"%s", row[7]);
  printf("11111\n");
  mysql_free_result(res);//释放结果集使用的内存。
  printf("22222\n");
  mysql_close(&mysql);
  printf("33333\n");  
  printf("mail is %s\n", mail);
  printf("pubkey is %s\n", pubkey);
  printf("encsec is %s\n", encsec);
  printf("encmas is %s\n", encmas);
  printf("salt is %s\n", salt);
  printf("method is %s\n", method);
  printf("rounds is %s\n", rounds);
 
  mysql_free_result(res);//释放结果集使用的内存。
  mysql_close(&mysql);
  
  return 0;
}