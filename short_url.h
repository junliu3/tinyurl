/**
 * @file    short_url.h
 * @brief   短地址模块 - 62 进制编解码 & 短地址生成
 *
 * 62 进制字符集：[0-9 a-z A-Z]，共 62 个字符。
 * 5 位 62 进制数可表示 62^5 ≈ 9.16 亿个短地址，足够使用。
 */

#ifndef SHORT_URL_H
#define SHORT_URL_H

/* -------------------------------------------------------------------------- */
/*                               62 进制编解码                                 */
/* -------------------------------------------------------------------------- */

/**
 * @brief  十进制数 → 5 位 62 进制字符串
 * @param  num  十进制数值（通常来自全局计数器）
 * @param  buf  输出缓冲区（至少 6 字节：5 字符 + '\0'）
 * @return buf（方便链式调用）
 *
 * @note   不足 5 位时左侧补 '0'
 */
char* decimal_to_base62(unsigned long long num, char* buf);

/**
 * @brief  5 位 62 进制字符串 → 十进制数
 * @param  code 62 进制字符串（5 位）
 * @return 十进制数值，格式错误返回 0
 */
unsigned long long base62_to_decimal(const char* code);

/**
 * @brief  验证短地址码格式是否正确（5 位 62 进制字符）
 * @param  code 待验证字符串
 * @return 1=合法，0=非法
 */
int is_valid_short_code(const char* code);

#endif /* SHORT_URL_H */
