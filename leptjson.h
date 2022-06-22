#ifndef LEPTJSON_H_
#define LEPTJSON_H_
#include<stdio.h>
#include<assert.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<math.h>
typedef struct 
{
    const char* json;
}lept_context;
//枚举出来6中数据类型
typedef enum { LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT } lept_type;
//枚举出来返回值
enum {
    LEPT_PARSE_OK = 0,     //正确
    LEPT_PARSE_EXPECT_VALUE,   //若一个 JSON 只含有空白
    LEPT_PARSE_INVALID_VALUE,   //若值不是那三种字面值
    LEPT_PARSE_ROOT_NOT_SINGULAR,  //若一个值之后，在空白之后还有其他字符
     LEPT_PARSE_NUMBER_TOO_BIG    //太大
};
typedef struct
{
    lept_type type;  //类型
    double n;  //数字(type == LEPT_NUMBER)

}lept_value; 
int lept_parse(lept_value* v, const char* json);   //解析json
lept_type lept_get_type(const lept_value* v); //获取访问结果
double lept_get_number(const lept_value* v);//获取数字
static int lept_parse_number(lept_context* c, lept_value* v);
static int lept_parse_literal(lept_context* c, lept_value* v, const char* literal, lept_type type) ;
#endif 