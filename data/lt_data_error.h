#ifndef LT_DATA_ERROR_H
#define LT_DATA_ERROR_H

//LOG_PRIORITY_ERROR 1100 ~ 1199
typedef enum
{
    LT_DATA_ERROR_TYPE_OK = 0,
    LT_DATA_ERROR_TYPE__STRING = 1100,
    LT_DATA_ERROR_TYPE_INT,
    LT_DATA_ERROR_TYPE_LLONG,
    LT_DATA_ERROR_TYPE_BOOL,
    LT_DATA_ERROR_TYPE_DIC ,
    LT_DATA_ERROR_TYPE_ARRARY ,
    LT_DATA_ERROR_UNKNOWN = 1199
} LT_DATA_ERROR_T;

#endif //LT_DATA_ERROR_H
