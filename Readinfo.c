#include <stdio.h>
#include <mysql/mysql.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/evp.h>

unsigned char *base64_pkey, *base64_encrypted_seckey, *base64_encrypted_masterkey, *base64_salt;
unsigned char *pkey, *encrypted_seckey, *encrypted_masterkey, *salt;
unsigned char *method;
unsigned char *rounds;
unsigned char *mail;

int EncodeBase64(const unsigned char *pFrom, unsigned char *pTo, int nFromLen)
{
  int nEncodeLen = EVP_EncodeBlock(pTo, pFrom, nFromLen);
  return nEncodeLen;
}

int DecodeBase64(const unsigned char *pFrom, unsigned char *pTo, int nFromLen)
{
  int nDecodeLen = EVP_DecodeBlock(pTo, pFrom, nFromLen);
  return nDecodeLen;
}

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
  query = "select * from info where id = 7;";
  t = mysql_real_query(&mysql, query, (unsigned int)strlen(query));//执行指定为计数字符串的SQL查询。
  if(t)
  {
    printf("ERROR query: %s", mysql_error(&mysql));
  }

  res = mysql_store_result(&mysql);//检索完整的结果集至客户端。

  row = mysql_fetch_row(res);



  asprintf(&mail,"%s", row[1]);
  asprintf(&base64_pkey,"%s", row[2]);
  asprintf(&base64_encrypted_seckey,"%s", row[3]);
  asprintf(&base64_encrypted_masterkey,"%s", row[4]);
  asprintf(&base64_salt,"%s", row[5]);
  asprintf(&method,"%s", row[6]);
  asprintf(&rounds,"%s", row[7]);

  mysql_free_result(res);
  mysql_close(&mysql); 
  
  //decode
  
  pkey = malloc(33);
  encrypted_seckey = malloc(48);
  encrypted_masterkey = malloc(48);
  salt = malloc(8);

  DecodeBase64(base64_pkey, pkey, 44);
  DecodeBase64(base64_encrypted_seckey, encrypted_seckey, 64);
  DecodeBase64(base64_encrypted_masterkey, encrypted_masterkey, 64);
  DecodeBase64(base64_salt, salt, 12);


  printf("mail is %s\n", mail);
  printf("method is %s\n", method);
  printf("rounds is %s\n", rounds);

  printf("%s\n", pkey);
  printf("%s\n", encrypted_seckey);
  printf("%s\n", encrypted_masterkey);
  printf("%s\n", salt);


  
  
  return 0;
}