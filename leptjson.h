#ifndef LEPTJSON_H_
#define LEPTJSON_H_
#include<stdio.h>
#include<assert.h>
#include<string.h>
#include <stddef.h>
#include<stdlib.h>
#include<errno.h>
#include<math.h>
typedef struct 
{
    const char* json;
    char*stack;
    size_t size,top;
}lept_context;
//枚举出来6中数据类型
typedef enum { LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT } lept_type;
//枚举出来返回值
enum {
    LEPT_PARSE_OK = 0,     //正确
    LEPT_PARSE_EXPECT_VALUE,   //若一个 JSON 只含有空白
    LEPT_PARSE_INVALID_VALUE,   //若值不是那三种字面值
    LEPT_PARSE_ROOT_NOT_SINGULAR,  //若一个值之后，在空白之后还有其他字符
    LEPT_PARSE_NUMBER_TOO_BIG,    //太大
    LEPT_PARSE_MISS_QUOTATION_MARK,
    LEPT_PARSE_INVALID_STRING_ESCAPE,
    LEPT_PARSE_INVALID_STRING_CHAR,
    LEPT_PARSE_INVALID_UNICODE_HEX,
    LEPT_PARSE_INVALID_UNICODE_SURROGATE
};
#define STRING_ERROR(ret) do { c->top = head; return ret; } while(0)
typedef struct
{
    lept_type type;  //类型
    union 
    {
        double n;  //数字(type == LEPT_NUMBER)
        struct
        {
            char*s;
            size_t len;
        }s;
    }u;
}lept_value; 
int lept_parse(lept_value* v, const char* json);   //解析json
lept_type lept_get_type(const lept_value* v); //获取访问结果
double lept_get_number(const lept_value* v);//获取数字
static int lept_parse_number(lept_context* c, lept_value* v);
static int lept_parse_literal(lept_context* c, lept_value* v, const char* literal, lept_type type) ;
void lept_set_string(lept_value* v, const char* s, size_t len);
void lept_free(lept_value* v);

#define lept_set_null(v) lept_free(v)
#define lept_init(v)  do{(v)->type=LEPT_NULL;}while(0)
int lept_get_boolean(const lept_value* v);
void lept_set_boolean(lept_value* v, int b);

double lept_get_number(const lept_value* v);
void lept_set_number(lept_value* v, double n);

static int lept_parse_string(lept_context*c,lept_value*v);
const char* lept_get_string(const lept_value* v);
size_t lept_get_string_length(const lept_value* v);
void lept_set_string(lept_value* v, const char* s, size_t len);
static const char* lept_parse_hex4(const char* p, unsigned* u);
static void lept_encode_utf8(lept_context* c, unsigned u);
#endif 
