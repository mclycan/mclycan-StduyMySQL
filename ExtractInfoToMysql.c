/*
Bruteforce a wallet file.

Copyright 2014-2015 Guillaume LE VAILLANT

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <ctype.h>
#include <db.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mysql/mysql.h>

/*
 * Database
 */

unsigned char pkey[33], encrypted_seckey[48], encrypted_masterkey[48], salt[8];
unsigned int pubkey_len, encrypted_seckey_len, encrypted_masterkey_len, method, rounds;
unsigned char hex_pkey[34], hex_encrypted_seckey[49], hex_encrypted_masterkey[49], hex_salt[9];

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
  //printf("11111111\n");
  memset(&db_key, 0, sizeof(db_key));
  memset(&db_data, 0, sizeof(db_data));
  while((ret = db_cursor->get(db_cursor, &db_key, &db_data, DB_NEXT)) == 0)
    {
      /* Find the encrypted master key */
      if(!mkey && (db_key.size > 7) && (memcmp(db_key.data + 1, "mkey", 4) == 0))
        {
          mkey = 1;
          encrypted_masterkey_len = ((unsigned char *) db_data.data)[0];
          //encrypted_masterkey = (unsigned char *) malloc(encrypted_masterkey_len);
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
          //pkey = (unsigned char *) malloc(pubkey_len);
          encrypted_seckey_len = ((unsigned char *) db_data.data)[0];
          //encrypted_seckey = (unsigned char *) malloc(encrypted_seckey_len);
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
              //cipher = EVP_aes_256_cbc();
              //digest = EVP_sha512();
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

  filename = argv[optind];

  char *mail = "machao@163.com";
  
  res = get_wallet_info(filename);
  if(res == 0)
    {
      fprintf(stderr, "Error: couldn't find required info in wallet.\n\n");
      exit(EXIT_FAILURE);
    }

    sprintf(hex_pkey
    ,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
    , pkey[0],  pkey[1],  pkey[2],  pkey[3],  pkey[4],  pkey[5],  pkey[6],  pkey[7],  pkey[8],  pkey[9],  pkey[10],
      pkey[11], pkey[12], pkey[13], pkey[14], pkey[15], pkey[16], pkey[17], pkey[18], pkey[19], pkey[20], pkey[21],
      pkey[22], pkey[23], pkey[24], pkey[25], pkey[26], pkey[27], pkey[28], pkey[29], pkey[30], pkey[31], pkey[32]);
   
    sprintf(hex_encrypted_seckey
    ,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
    , encrypted_seckey[0],   encrypted_seckey[1],  encrypted_seckey[2],  encrypted_seckey[3],  encrypted_seckey[4],  encrypted_seckey[5],
      encrypted_seckey[6],   encrypted_seckey[7],  encrypted_seckey[8],  encrypted_seckey[9],  encrypted_seckey[10], encrypted_seckey[11],
      encrypted_seckey[12],  encrypted_seckey[13], encrypted_seckey[14], encrypted_seckey[15], encrypted_seckey[16], encrypted_seckey[17],
      encrypted_seckey[18],  encrypted_seckey[19], encrypted_seckey[20], encrypted_seckey[21], encrypted_seckey[22], encrypted_seckey[23],
      encrypted_seckey[24],  encrypted_seckey[25], encrypted_seckey[26], encrypted_seckey[27], encrypted_seckey[28], encrypted_seckey[29],
      encrypted_seckey[30],  encrypted_seckey[31], encrypted_seckey[32], encrypted_seckey[33], encrypted_seckey[34], encrypted_seckey[35],
      encrypted_seckey[36],  encrypted_seckey[37], encrypted_seckey[38], encrypted_seckey[39], encrypted_seckey[40], encrypted_seckey[41],
      encrypted_seckey[42],  encrypted_seckey[43], encrypted_seckey[44], encrypted_seckey[45], encrypted_seckey[46], encrypted_seckey[47]);
    //printf("%s\n",hex_encrypted_seckey);
      
    sprintf(hex_encrypted_masterkey
    ,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
    , encrypted_masterkey[0],   encrypted_masterkey[1],  encrypted_masterkey[2],  encrypted_masterkey[3],  encrypted_masterkey[4],  encrypted_masterkey[5],
      encrypted_masterkey[6],   encrypted_masterkey[7],  encrypted_masterkey[8],  encrypted_masterkey[9],  encrypted_masterkey[10], encrypted_masterkey[11],
      encrypted_masterkey[12],  encrypted_masterkey[13], encrypted_masterkey[14], encrypted_masterkey[15], encrypted_masterkey[16], encrypted_masterkey[17],
      encrypted_masterkey[18],  encrypted_masterkey[19], encrypted_masterkey[20], encrypted_masterkey[21], encrypted_masterkey[22], encrypted_masterkey[23],
      encrypted_masterkey[24],  encrypted_masterkey[25], encrypted_masterkey[26], encrypted_masterkey[27], encrypted_masterkey[28], encrypted_masterkey[29],
      encrypted_masterkey[30],  encrypted_masterkey[31], encrypted_masterkey[32], encrypted_masterkey[33], encrypted_masterkey[34], encrypted_masterkey[35],
      encrypted_masterkey[36],  encrypted_masterkey[37], encrypted_masterkey[38], encrypted_masterkey[39], encrypted_masterkey[40], encrypted_masterkey[41],
      encrypted_masterkey[42],  encrypted_masterkey[43], encrypted_masterkey[44], encrypted_masterkey[45], encrypted_masterkey[46], encrypted_masterkey[47]);

    sprintf(hex_salt
    ,"%02x%02x%02x%02x%02x%02x%02x%02x"
    , salt[0], salt[1], salt[2], salt[3], salt[4], salt[5], salt[6], salt[7]);
    
    printf("%s\n",hex_encrypted_seckey);
    printf("%s\n",hex_encrypted_masterkey);
    printf("%s\n",hex_pkey);
    printf("%s\n",hex_salt);
	 
    mysql_init(&my_connection);
    //"localhost", "root", "123456", "mysql" : ip, user, passwd, database;
    if (mysql_real_connect(&my_connection, "localhost", "root", "123456", "walletinfo", 0, NULL, 0)) 
    {
        printf("Connection success\n");
        printf("%s\n",hex_encrypted_seckey);
        printf("%s\n",hex_encrypted_masterkey);
        sprintf(sql_insert
        ,"INSERT INTO info(mail, pubkey, encsec, encmas, salt, method, rounds) VALUES('%s', '%s', '%s', '%s', '%s', '%d', '%d');"
        ,mail
        ,hex_pkey
        ,hex_encrypted_seckey
        ,hex_encrypted_masterkey
        ,hex_salt
        ,method
        ,rounds);
        printf("%s\n",hex_encrypted_seckey);
        printf("%s\n",hex_encrypted_masterkey);

        res = mysql_query(&my_connection, sql_insert);

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
