#include"leptjson.h"
#define EXPEXT(c,ch)  do{assert(*c->json==(ch));c->json++;}while(0)
static void lept_parse_whitespace(lept_context* c)
{
    const char* p=c->json;
    while(*p==' '||*p=='\n'||*p=='\t'||*p=='\r')
    {
        p++;
    }
    c->json=p;
}
static int lept_parse_number(lept_context* c, lept_value* v)
{
    char*end;
    v->n=strtod(c->json,&end);
    if(c->json==end)
    return LEPT_PARSE_INVALID_VALUE;
    c->json=end;
    v->type=LEPT_NUMBER;
    return LEPT_PARSE_OK;
}
static int lept_parse_literal(lept_context* c, lept_value* v, const char* literal, lept_type type) 
{
    EXPEXT(c,literal[0]);
    size_t i;
    for(i=0;literal[i+1];i++)
    {
        if(c->json[i]!=literal[i+1])
        return LEPT_PARSE_INVALID_VALUE;
    }
    c->json+=i;
    v->type=type;
    return LEPT_PARSE_OK;
}
static int lept_parse_value(lept_context* c, lept_value* v) 
{
    switch (*c->json) 
    {
        case 't':  
            return lept_parse_literal(c,v,"true",LEPT_TRUE);
        case 'f':  
            return lept_parse_literal(c,v,"false",LEPT_FALSE);
        case 'n':  
            return lept_parse_literal(c,v,"null",LEPT_NULL);
        default:   
            return lept_parse_number(c,v);
        case '\0': 
            return LEPT_PARSE_EXPECT_VALUE;
    }
}
lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}
double lept_get_number(const lept_value* v)
{
    assert(v!=NULL&&v->type==LEPT_NUMBER);
    return v->n;
}
int lept_parse(lept_value* v, const char* json)
{
    lept_context c;
    c.json=json;
    v->type=LEPT_NULL;
    lept_parse_whitespace(&c);
    int ret;//接受返回值
    if((ret=lept_parse_value(&c,v))==LEPT_PARSE_OK)
    {
        lept_parse_whitespace(&c);
        if(*c.json!='\0')
        return LEPT_PARSE_ROOT_NOT_SINGULAR;
    }
    return ret; 
}