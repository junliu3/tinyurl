/**
 * @file    short_url.c
 * @brief   短地址模块实现 - 62 进制编解码
 *
 * 字符集：[0-9 a-z A-Z]，每个字符对应 0~61 的数值。
 */

#include "short_url.h"
#include <string.h>

/* --------------------------------------------------------------------------
 *  62 进制字符集
 * -------------------------------------------------------------------------- */

static const char BASE62_CHARS[] =
    "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/* --------------------------------------------------------------------------
 *  decimal_to_base62 —— 十进制 → 5 位 62 进制
 * -------------------------------------------------------------------------- */

char* decimal_to_base62(unsigned long long num, char* buf)
{
    /* 从右往左填充 */
    for (int i = 4; i >= 0; i--) {
        buf[i] = BASE62_CHARS[num % 62];
        num /= 62;
    }
    buf[5] = '\0';
    return buf;
}

/* --------------------------------------------------------------------------
 *  base62_to_decimal —— 62 进制 → 十进制
 * -------------------------------------------------------------------------- */

unsigned long long base62_to_decimal(const char* code)
{
    if (code == NULL || strlen(code) != 5) {
        return 0;
    }

    unsigned long long result = 0;

    for (int i = 0; i < 5; i++) {
        int value;

        if (code[i] >= '0' && code[i] <= '9') {
            value = code[i] - '0';
        } else if (code[i] >= 'a' && code[i] <= 'z') {
            value = code[i] - 'a' + 10;
        } else if (code[i] >= 'A' && code[i] <= 'Z') {
            value = code[i] - 'A' + 36;
        } else {
            return 0;  /* 非法字符 */
        }

        result = result * 62 + value;
    }

    return result;
}

/* --------------------------------------------------------------------------
 *  is_valid_short_code —— 格式校验
 * -------------------------------------------------------------------------- */

int is_valid_short_code(const char* code)
{
    if (code == NULL || strlen(code) != 5) {
        return 0;
    }

    for (int i = 0; i < 5; i++) {
        char c = code[i];
        int valid = (c >= '0' && c <= '9') ||
                    (c >= 'a' && c <= 'z') ||
                    (c >= 'A' && c <= 'Z');
        if (!valid) {
            return 0;
        }
    }

    return 1;
}
