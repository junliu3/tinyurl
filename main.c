/**
 * @file    main.c
 * @brief   TinyURL 短地址服务 —— CLI 菜单交互
 *
 * 功能：
 *   1. 生成短地址  — 输入原始 URL，输出 5 位短地址码
 *   2. 解析短地址  — 输入短地址，还原原始 URL
 *   3. 统计信息    — 查看所有映射记录及访问次数
 *   4. 设置有效次数 — 限制某个短地址的最大解析次数
 *   5. 删除短地址  — 删除指定映射
 *   6. 退出程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "short_url.h"
#include "linked_list.h"

/* ==========================================================================
 *                               宏定义
 * ========================================================================== */

#define MAX_URL_LEN   4096       /* URL 最大长度（2k~4k） */
#define MAX_INPUT_LEN 256        /* 通用输入缓冲区      */
#define SHORT_LEN     5          /* 短地址长度          */

/* ==========================================================================
 *                               函数声明
 * ========================================================================== */

static void show_menu(void);
static void do_create(url_list_t* list);
static void do_resolve(url_list_t* list);
static void do_stats(const url_list_t* list);
static void do_set_limit(url_list_t* list);
static void do_delete(url_list_t* list);

/* ==========================================================================
 *                               main 入口
 * ========================================================================== */

int main(void)
{
    /* ---------- 初始化链表 ---------- */
    url_list_t* list = list_create();
    if (list == NULL) {
        fprintf(stderr, "[致命错误] 无法创建链表，程序退出。\n");
        return 1;
    }

    printf("\n");
    printf("╔══════════════════════════════════════════╗\n");
    printf("║        TinyURL - 短地址服务 v1.0        ║\n");
    printf("║        数据结构：单向链表（尾插法）       ║\n");
    printf("╚══════════════════════════════════════════╝\n");

    /* ---------- 主循环 ---------- */
    int running = 1;
    while (running) {
        show_menu();

        char input[MAX_INPUT_LEN];
        printf("请输入：");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        /* 去掉末尾换行 */
        input[strcspn(input, "\n")] = '\0';

        switch (input[0]) {
        case '1':
            do_create(list);
            break;
        case '2':
            do_resolve(list);
            break;
        case '3':
            do_stats(list);
            break;
        case '4':
            do_set_limit(list);
            break;
        case '5':
            do_delete(list);
            break;
        case '6':
            printf("\n感谢使用，再见！\n\n");
            running = 0;
            break;
        default:
            printf("\n[提示] 无效选项，请重新输入。\n\n");
            break;
        }
    }

    /* ---------- 清理 ---------- */
    list_destroy(list);
    return 0;
}

/* ==========================================================================
 *                              菜单显示
 * ========================================================================== */

static void show_menu(void)
{
    printf("┌──────────────────────────────────────────┐\n");
    printf("│          -------- 请选择操作 --------      │\n");
    printf("├──────────────────────────────────────────┤\n");
    printf("│  1. 生成短地址                            │\n");
    printf("│  2. 解析短地址                            │\n");
    printf("│  3. 查看统计信息                          │\n");
    printf("│  4. 设置有效次数                          │\n");
    printf("│  5. 删除短地址                            │\n");
    printf("│  6. 退出程序                              │\n");
    printf("└──────────────────────────────────────────┘\n");
}

/* ==========================================================================
 *                          功能 1：生成短地址
 * ========================================================================== */

static void do_create(url_list_t* list)
{
    char url[MAX_URL_LEN];
    char max_str[MAX_INPUT_LEN];
    int  max_access = -1;  /* 默认无限制 */

    printf("\n--- 生成短地址 ---\n");

    /* 输入原始 URL */
    printf("请输入原始 URL：");
    if (fgets(url, sizeof(url), stdin) == NULL) return;
    url[strcspn(url, "\n")] = '\0';

    if (strlen(url) == 0) {
        printf("[提示] URL 不能为空。\n\n");
        return;
    }

    /* 输入有效次数 */
    printf("请输入有效次数（直接回车 = 无限制）：");
    if (fgets(max_str, sizeof(max_str), stdin) == NULL) return;
    max_str[strcspn(max_str, "\n")] = '\0';

    if (strlen(max_str) > 0) {
        max_access = atoi(max_str);
        if (max_access <= 0) {
            printf("[提示] 有效次数必须为正整数，已设为无限制。\n");
            max_access = -1;
        }
    }

    /* --- 核心算法：全局计数器 → 62 进制 → 短地址码 --- */
    char short_code[6];
    decimal_to_base62(list->counter, short_code);

    /* 插入链表（尾插法） */
    url_node_t* node = list_insert(list, url, short_code, max_access);
    if (node == NULL) {
        printf("[错误] 生成短地址失败。\n\n");
        return;
    }

    /* 计数器递增，为下一次生成做准备 */
    list->counter++;

    printf("\n✓ 短地址生成成功！\n");
    printf("  短地址码：[%s]\n", short_code);
    printf("  原始 URL：%s\n", url);
    printf("  有效次数：%s\n\n", max_access == -1 ? "无限制" : max_str);
}

/* ==========================================================================
 *                          功能 2：解析短地址
 * ========================================================================== */

static void do_resolve(url_list_t* list)
{
    char code[MAX_INPUT_LEN];

    printf("\n--- 解析短地址 ---\n");
    printf("请输入短地址码（5 位）：");

    if (fgets(code, sizeof(code), stdin) == NULL) return;
    code[strcspn(code, "\n")] = '\0';

    /* 格式校验 */
    if (!is_valid_short_code(code)) {
        printf("[错误] 短地址码格式不正确（应为 5 位数字+字母）。\n\n");
        return;
    }

    /* 链表遍历查找 */
    url_node_t* node = list_find_by_short(list, code);
    if (node == NULL) {
        printf("[提示] 未找到该短地址对应的原始 URL。\n\n");
        return;
    }

    /* 检查有效次数 */
    if (node->max_access != -1 && node->access_count >= node->max_access) {
        printf("[提示] 该短地址已达有效次数上限（%d 次），已失效。\n\n",
               node->max_access);
        return;
    }

    /* 解析成功，计数 +1 */
    node->access_count++;

    printf("\n✓ 解析成功！\n");
    printf("  原始 URL：%s\n", node->original_url);
    printf("  已解析 %d 次", node->access_count);
    if (node->max_access != -1) {
        printf(" / 上限 %d 次（剩余 %d 次）",
               node->max_access,
               node->max_access - node->access_count);
    }
    printf("\n\n");
}

/* ==========================================================================
 *                          功能 3：查看统计信息
 * ========================================================================== */

static void do_stats(const url_list_t* list)
{
    list_print_all(list);
    printf("\n");
}

/* ==========================================================================
 *                        功能 4：设置有效次数
 * ========================================================================== */

static void do_set_limit(url_list_t* list)
{
    char code[MAX_INPUT_LEN];
    char limit_str[MAX_INPUT_LEN];

    printf("\n--- 设置有效次数 ---\n");
    printf("请输入短地址码（5 位）：");

    if (fgets(code, sizeof(code), stdin) == NULL) return;
    code[strcspn(code, "\n")] = '\0';

    if (!is_valid_short_code(code)) {
        printf("[错误] 短地址码格式不正确。\n\n");
        return;
    }

    url_node_t* node = list_find_by_short(list, code);
    if (node == NULL) {
        printf("[提示] 未找到该短地址。\n\n");
        return;
    }

    printf("当前有效次数：%s\n",
           node->max_access == -1 ? "无限制" : "");
    if (node->max_access != -1) {
        printf("%d\n", node->max_access);
    }
    printf("请输入新的有效次数（输入 0 或直接回车 = 无限制）：");

    if (fgets(limit_str, sizeof(limit_str), stdin) == NULL) return;
    limit_str[strcspn(limit_str, "\n")] = '\0';

    if (strlen(limit_str) == 0 || atoi(limit_str) == 0) {
        node->max_access = -1;
        printf("✓ 已设为无限制。\n\n");
    } else {
        node->max_access = atoi(limit_str);
        printf("✓ 已更新有效次数为 %d。\n\n", node->max_access);
    }
}

/* ==========================================================================
 *                          功能 5：删除短地址
 * ========================================================================== */

static void do_delete(url_list_t* list)
{
    char code[MAX_INPUT_LEN];

    printf("\n--- 删除短地址 ---\n");
    printf("请输入要删除的短地址码（5 位）：");

    if (fgets(code, sizeof(code), stdin) == NULL) return;
    code[strcspn(code, "\n")] = '\0';

    if (!is_valid_short_code(code)) {
        printf("[错误] 短地址码格式不正确。\n\n");
        return;
    }

    if (list_remove(list, code) == 0) {
        printf("✓ 短地址 [%s] 已删除。\n\n", code);
    } else {
        printf("[提示] 未找到该短地址。\n\n");
    }
}
