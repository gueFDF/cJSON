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
//枚举出来返回码
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
    LEPT_PARSE_INVALID_UNICODE_SURROGATE,
    LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
    LEPT_PARSE_MISS_KEY,
    LEPT_PARSE_MISS_COLON,
    LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET,
    LEPT_STRINGIFY_OK,
    LEPT_KEY_NOT_EXIST
};
#define STRING_ERROR(ret) do { c->top = head; return ret; } while(0)
typedef struct lept_value lept_value;
typedef struct lept_member lept_member;

struct lept_value {
    union {
        struct { lept_member* m; size_t size,capacity;}o;
        struct { lept_value* e; size_t size,capacity;}a;
        struct { char* s; size_t len; }s;
        double n;
    }u;
    lept_type type;
};

struct lept_member {
    char* k; size_t klen;   /* member key string, key string length */
    lept_value v;           /* member value */
};
int lept_parse(lept_value* v, const char* json);   //解析json
lept_type lept_get_type(const lept_value* v); //获取访问结果
double lept_get_number(const lept_value* v);//获取数字
static int lept_parse_number(lept_context* c, lept_value* v);
static int lept_parse_literal(lept_context* c, lept_value* v, const char* literal, lept_type type) ;
void lept_set_string(lept_value* v, const char* s, size_t len);
void lept_free(lept_value* v);
static int lept_parse_value(lept_context* c, lept_value* v);
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
size_t lept_get_array_size(const lept_value*v);
lept_value* lept_get_array_element(const lept_value*v,size_t index);
static int lept_parse_array(lept_context*c,lept_value*v);


static void*lept_context_pop(lept_context*c,size_t size);
static void*lept_context_push(lept_context*c,size_t size);

static int lept_parse_string_raw(lept_context* c, char** str, size_t* len) ;
static int lept_stringify_value(lept_context* c, const lept_value* v);
int lept_stringify(const lept_value* v, char** json, size_t* length);
static int lept_parse_object(lept_context* c, lept_value* v);
static void lept_stringify_string(lept_context* c, const char* s, size_t len);

size_t lept_get_object_size(const lept_value* v);
const char* lept_get_object_key(const lept_value* v, size_t index);
size_t lept_get_object_key_length(const lept_value* v, size_t index);
lept_value* lept_get_object_value(lept_value* v, size_t index);
int lept_is_equal(const lept_value* lhs, const lept_value* rhs);

void lept_copy(lept_value* dst, const lept_value* src);
void lept_move(lept_value* dst, lept_value* src);
void lept_swap(lept_value* lhs, lept_value* rhs);


lept_value* lept_insert_array_element(lept_value* v, size_t index);
void lept_erase_array_element(lept_value* v, size_t index, size_t count);
void lept_clear_array(lept_value* v);
void lept_set_array(lept_value* v, size_t capacity);
void lept_reserve_array(lept_value* v, size_t capacity);
void lept_shrink_array(lept_value* v);
lept_value* lept_pushback_array_element(lept_value* v);
void lept_popback_array_element(lept_value* v);
size_t lept_get_array_capacity(const lept_value* v);


void lept_set_object(lept_value* v, size_t capacity);
size_t lept_get_object_capacity(const lept_value* v);
void lept_reserve_object(lept_value* v, size_t capacity);
void lept_shrink_object(lept_value* v);
void lept_clear_object(lept_value* v);
lept_value* lept_set_object_value(lept_value* v, const char* key, size_t klen);
void lept_remove_object_value(lept_value* v, size_t index);
#endif 
