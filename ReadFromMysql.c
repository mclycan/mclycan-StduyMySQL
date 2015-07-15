#include <stdio.h>
#include <mysql/mysql.h>
#include <string.h>

int hexToStr(unsigned char *hex, unsigned char *ch);
int hexCharToValue(const unsigned char ch);

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
  unsigned char encsec[49];
  unsigned char encmas[49];
  unsigned char salt[8];
  unsigned char string_method[2];
  unsigned char string_rounds[10];
  unsigned char hex_mail[45];
  unsigned char hex_pubkey[33];
  unsigned char hex_encsec[49];
  unsigned char hex_encmas[49];
  unsigned char hex_salt[8];


  if (!mysql_real_connect(&mysql, "localhost", "root", "123456", "walletinfo", 0, NULL, 0))
  {
    printf( "Error connecting to database: %s", mysql_error(&mysql));
  }
  else
  {
    //printf("Connected...\n");
  }
  query = "select * from info where id=2;";
  t = mysql_real_query(&mysql, query, (unsigned int)strlen(query));//执行指定为计数字符串的SQL查询。
  if(t)
  {
    printf("ERROR query: %s", mysql_error(&mysql));
  }

  res = mysql_store_result(&mysql);//检索完整的结果集至客户端。
  mysql_close(&mysql);
  row = mysql_fetch_row(res);
  
  sprintf(hex_mail,"%s", row[1]);
  sprintf(hex_pubkey,"%s", row[2]);
  sprintf(hex_encsec,"%s", row[3]);
  sprintf(hex_encmas,"%s", row[4]);
  sprintf(hex_salt,"%s", row[5]);
  sprintf(string_method,"%s", row[6]);
  sprintf(string_rounds,"%s", row[7]);

  mysql_free_result(res);//释放结果集使用的内存。

  printf("mail is %s\n", hex_mail);
  printf("pubkey is %s\n", hex_pubkey);
  printf("encsec is %s\n", hex_encsec);
  printf("encmas is %s\n", hex_encmas);
  printf("salt is %s\n", hex_salt);
  printf("method is %s\n", string_method);
  printf("rounds is %s\n", string_rounds);


  hexToStr(hex_mail, mail);
  hexToStr(hex_pubkey, pubkey);
  hexToStr(hex_encsec, encsec);
  hexToStr(hex_encmas, encmas);
  hexToStr(hex_salt, salt);

  printf("mail is %s\n", mail);
  printf("pubkey is %s\n", pubkey);
  printf("encsec is %s\n", encsec);
  printf("encmas is %s\n", encmas);
  printf("salt is %s\n", salt);

  return 0;
}

int hexToStr(unsigned char *hex, unsigned char *ch)
{
  int high,low;
  int tmp = 0;
  if(hex == NULL || ch == NULL){
    return -1;
  }

  while(*hex){
    high = hexCharToValue(*hex);
    if(high < 0){
      *ch = '\0';
      return -3;
    }
    hex++; //指针移动到下一个字符上
    low = hexCharToValue(*hex);
    if(low < 0){
      *ch = '\0';
      return -3;
    }
    tmp = (high << 4) + low;
    *ch++ = (unsigned char)tmp;
    hex++;
  }
  *ch = '\0';
  return 0;
}

int hexCharToValue(const unsigned char ch){
  int result = 0;
  //获取16进制的高字节位数据
  if(ch >= '0' && ch <= '9'){
    result = (int)(ch - '0');
  }
  else if(ch >= 'a' && ch <= 'z'){
    result = (int)(ch - 'a') + 10;
  }
  else if(ch >= 'A' && ch <= 'Z'){
    result = (int)(ch - 'A') + 10;
  }
  else{
    result = -1;
  }
  return result;
}
