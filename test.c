#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"leptjson.h"
static int main_ret=0;
static int test_count=0;
static int test_pass=0;
#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)


#define EXPECT_EQ_INT(expect,actual) EXPECT_EQ_BASE(((expect)==(actual)),expect,actual,"%d")



#define TESTERROR(error,json)\
    do{\
        lept_value v;\
        v.type=LEPT_FALSE;\
        EXPECT_EQ_INT(error,lept_parse(&v,json));\
        EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));\
    }while(0)

static void test_parse_error()
{
    TESTERROR(LEPT_PARSE_INVALID_VALUE,"nul");
    TESTERROR(LEPT_PARSE_INVALID_VALUE,"?");
    TESTERROR(LEPT_PARSE_EXPECT_VALUE,"");
    TESTERROR(LEPT_PARSE_EXPECT_VALUE," ");
    TESTERROR(LEPT_PARSE_OK,"null       ");
    TESTERROR(LEPT_PARSE_ROOT_NOT_SINGULAR,"null x");
    TESTERROR(LEPT_PARSE_ROOT_NOT_SINGULAR,"null      x");
}
static void test_parse_null() 
{
    lept_value v;
    v.type = LEPT_FALSE;TESTERROR(LEPT_PARSE_ROOT_NOT_SINGULAR,"null    x");
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "null"));
    EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));
}
static void test_parse_true() 
{
    lept_value v;
    v.type = LEPT_FALSE;
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "true"));
    EXPECT_EQ_INT(LEPT_TRUE, lept_get_type(&v));
}

static void test_parse_false() 
{
    lept_value v;
    v.type = LEPT_TRUE;
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "false"));
    EXPECT_EQ_INT(LEPT_FALSE, lept_get_type(&v));
}

static void test_parse()
{
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_error();
}
int main() {
    test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}