/**
 * @file main.c
 * @brief Entry point of the system
 */
//STD headers
#include <stdio.h>
#include <stdlib.h>

//LiB or deps headers
#include <cjson/cJSON.h>


//SAME FOLDER HEADERS,WiTH 


int main(void)
{
    //create a json obj
    cJSON* root = cJSON_CreateObject();
   
   //add samedate
   cJSON_AddStringToObject(root,"name","dario");
   cJSON_AddNumberToObject(root,"age",25);
   cJSON_AddBoolToObject(root,"is_student",0);

   //complex date types
   cJSON* hobbies= cJSON_CreateArray();

   cJSON_addItemToArray(hobbies,cJSON_CreateString("readinf"));
   cJSON_addItemToArray(hobbies,cJSON_CreateString("traning"));
   cJSON_AddObjectToObject(root,"hobbies",hobbies);


   //print obj
   char* json_string = cJSON_Print(root);
   print("%s\n", json_string);

   //cleanup
   cJSON_Delete(root);
   free(json_string);


    return 0;
    
}//cmake .. -DCMAKE_TOOLCHAIN_FILE=./build/Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug

//54:13
//hacer cmake ..