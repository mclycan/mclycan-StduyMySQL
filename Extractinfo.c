#include <ctype.h>
#include <db.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include <openssl/evp.h>
#include <openssl/x509.h>


unsigned char *pkey, *encrypted_seckey, *encrypted_masterkey, *salt;
unsigned char *base64_pkey, *base64_encrypted_seckey, *base64_encrypted_masterkey, *base64_salt;
unsigned int pubkey_len, encrypted_seckey_len, encrypted_masterkey_len, method, rounds;

void base64_Encode(unsigned char *input, int inl, unsigned char *output)  
{  
    EVP_ENCODE_CTX ctx;    
    int outl;  
  
    EVP_EncodeInit(&ctx);  
 
    EVP_EncodeUpdate(&ctx, output, &outl, input, inl);  
  
    EVP_EncodeFinal(&ctx, output, &outl);  

    return; 
  
}  

void base64_Decode(unsigned char *input, int inl, unsigned char *output)  
{  
    EVP_ENCODE_CTX ctx;  
    int outl;  
  
    EVP_DecodeInit(&ctx);//Base64 解码初始化  
   
    EVP_DecodeUpdate(&ctx, output, &outl, input, inl);  
    
    EVP_DecodeFinal(&ctx, output, &outl);  
  
    return;  
}  


int get_wallet_info(char *filename)
{
  DB *db;
  DBC *db_cursor;
  DBT db_key, db_data;
  int ret, mkey = 0, ckey = 0;

  /* Open the BerkeleyDB database file */
  ret = db_create(&db, NULL, 0);
  if(ret != 0)
    {
      fprintf(stderr, "Error: db_create: %s.\n\n", db_strerror(ret));
      exit(EXIT_FAILURE);
    }

  ret = db->open(db, NULL, filename, "main", DB_UNKNOWN, DB_RDONLY, 0);
  if(ret != 0)
    {
      db->err(db, ret, "Error: %s.\n\n", filename);
      db->close(db, 0);
      exit(EXIT_FAILURE);
    }

  ret = db->cursor(db, NULL, &db_cursor, 0);
  if(ret != 0)
    {
      db->err(db, ret, "Error: %s.\n\n", filename);
      db->close(db, 0);
      exit(EXIT_FAILURE);
    }
  
  memset(&db_key, 0, sizeof(db_key));
  memset(&db_data, 0, sizeof(db_data));
  while((ret = db_cursor->get(db_cursor, &db_key, &db_data, DB_NEXT)) == 0)
    {
      /* Find the encrypted master key */
      if(!mkey && (db_key.size > 7) && (memcmp(db_key.data + 1, "mkey", 4) == 0))
        {
          mkey = 1;
          encrypted_masterkey_len = ((unsigned char *) db_data.data)[0];
          encrypted_masterkey = (unsigned char *) malloc(encrypted_masterkey_len);
          salt = (unsigned char *) malloc(8);
          if(encrypted_masterkey == NULL)
            {
              fprintf(stderr, "Error: memory allocation failed.\n\n");
              exit(EXIT_FAILURE);
            }

          memcpy(encrypted_masterkey, db_data.data + 1, encrypted_masterkey_len);
          memcpy(salt, db_data.data + 1 + encrypted_masterkey_len + 1, 8);
          method = *((unsigned int *) (db_data.data + 1 + encrypted_masterkey_len + 1 + 8));
          rounds = *((unsigned int *) (db_data.data + 1 + encrypted_masterkey_len + 1 + 8 + 4));
        }

      /* Find an encrypted secret key */
      if(!ckey && (db_key.size > 7) && (memcmp(db_key.data + 1, "ckey", 4) == 0))
        {
          ckey = 1;
          pubkey_len = ((unsigned char *) db_key.data)[5];
          pkey = (unsigned char *) malloc(pubkey_len);
          encrypted_seckey_len = ((unsigned char *) db_data.data)[0];
          encrypted_seckey = (unsigned char *) malloc(encrypted_seckey_len);
          if((pkey == NULL) || (encrypted_seckey == NULL))
            {
              fprintf(stderr, "Error: memory allocation failed.\n\n");
              exit(EXIT_FAILURE);
            }

          memcpy(pkey, db_key.data + 6, pubkey_len);
          memcpy(encrypted_seckey, db_data.data + 1, encrypted_seckey_len);
        }

      if(mkey && ckey)
        {
          if(method == 0)
            {
             
            }
          else
            {
              fprintf(stderr, "Error: encryption method not supported: %u.\n\n", method);
              exit(EXIT_FAILURE);
            }

          db_cursor->close(db_cursor);
          db->close(db, 0);
          return(1);
        }
    }

  db_cursor->close(db_cursor);
  db->close(db, 0);
  return(0);
}


int main(int argc, char **argv)
{
  char *filename;
  MYSQL my_connection;
  int res, i;
  unsigned char sql_insert[520];
 
  if(optind >= argc)
    {
      
      fprintf(stderr, "Error: missing filename.\n\n");
      exit(EXIT_FAILURE);
    }

  OpenSSL_add_all_algorithms();

  filename = argv[optind];

  char *mail = "machao@163.com";
  
  res = get_wallet_info(filename);
  if(res == 0)
    {
      fprintf(stderr, "Error: couldn't find required info in wallet.\n\n");
      exit(EXIT_FAILURE);
    }

  //pubkey_len, encrypted_seckey_len, encrypted_masterkey_len
  //*pkey, *encrypted_seckey, *encrypted_masterkey, *salt;

  base64_Encode(pkey, pubkey_len, base64_pkey);
  base64_Encode(encrypted_seckey, encrypted_seckey_len, base64_encrypted_seckey);
  base64_Encode(encrypted_masterkey, encrypted_masterkey_len, base64_encrypted_masterkey);
  base64_Encode(salt, 8, base64_salt);

  mysql_init(&my_connection);
    
    if (mysql_real_connect(&my_connection, "localhost", "root", "123456", "walletinfo", 0, NULL, 0)) 
    {
        printf("Connection success...\n");

        sprintf(sql_insert
        ,"INSERT INTO info(mail, pubkey, encsec, encmas, salt, method, rounds) VALUES('%s', '%s', '%s', '%s', '%s', '%d', '%d');"
        ,mail
        ,base64_pkey
        ,base64_encrypted_seckey
        ,base64_encrypted_masterkey
        ,base64_salt
        ,method
        ,rounds);

        res = mysql_query(&my_connection, sql_insert);

        if (!res) 
        {
            printf("Insert %lu rows success...\n", (unsigned long)mysql_affected_rows(&my_connection));
            printf("You can goto your database to check result...\n");
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
