#include <stdio.h>
#include <mysql/mysql.h>
#include <string.h>
#include <stdlib.h>

int hexToStr(unsigned char *hex, unsigned char *ch);
int hexCharToValue(const unsigned char ch);

int main()
{
  MYSQL mysql;    //mysql连接
  MYSQL_RES *res; //这个结构代表返回行的一个查询结果集
  MYSQL_ROW row; //一个行数据的类型安全(type-safe)的表示
  char *query;  //查询语句    
  int t,r,i;
  mysql_init(&mysql);
  unsigned char *mail;
  unsigned char pubkey[33];
  unsigned char encsec[49];
  unsigned char encmas[49];
  unsigned char salt[9];
  unsigned char *s_method;
  unsigned char *s_rounds;
  unsigned char *hex_pubkey;
  unsigned char *hex_encsec;
  unsigned char *hex_encmas;
  unsigned char *hex_salt;


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
  printf("2222\n");
  row = mysql_fetch_row(res);
  printf("3333\n");
  asprintf(&mail,"%s", row[1]);
  asprintf(&hex_pubkey,"%s", row[2]);
  asprintf(&hex_encsec,"%s", row[3]);
  asprintf(&hex_encmas,"%s", row[4]);
  asprintf(&hex_salt,"%s", row[5]);
  asprintf(&s_method,"%s", row[6]);
  asprintf(&s_rounds,"%s", row[7]);
  printf("4444\n");
  mysql_free_result(res);//释放结果集使用的内存。
  mysql_close(&mysql);
  printf("5555\n");
  printf("mail is %s\n", mail);
  printf("pubkey is %s\n", hex_pubkey);
  printf("encsec is %s\n", hex_encsec);
  printf("encmas is %s\n", hex_encmas);
  printf("salt is %s\n", hex_salt);
  printf("method is %s\n", s_method);
  printf("rounds is %s\n", s_rounds);

  hexToStr(hex_pubkey, pubkey);
  hexToStr(hex_encsec, encsec);
  hexToStr(hex_encmas, encmas);
  hexToStr(hex_salt, salt);
  
  printf("pubkey is %s\n", pubkey);
  printf("encsec is %s\n", encsec);
  printf("encmas is %s\n", encmas);
  printf("salt is %s\n", salt);

  for(i = 0; i < 48; i++)
  {
    if(i % 8 == 0)
      printf("\n");
    printf("0x%02x ", encsec[i]);
  }
  printf("\n"); 

  free(hex_pubkey);
  free(hex_encsec);
  free(hex_encmas);
  free(hex_salt);

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
