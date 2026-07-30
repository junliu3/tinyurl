/**
 * @file    linked_list.c
 * @brief   单链表模块实现 - 尾插法的单向链表
 */

#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ==========================================================================
 *                              链表核心操作
 * ========================================================================== */

url_list_t* list_create(void)
{
    url_list_t* list = (url_list_t*)malloc(sizeof(url_list_t));
    if (list == NULL) {
        fprintf(stderr, "[错误] 内存分配失败：无法创建链表\n");
        return NULL;
    }

    list->head    = NULL;
    list->tail    = NULL;
    list->size    = 0;
    list->counter = 1;          /* ID 从 1 开始 */
    return list;
}

/* -------------------------------------------------------------------------- */

url_node_t* list_insert(url_list_t* list,
                        const char* original_url,
                        const char* short_code,
                        int max_access)
{
    if (list == NULL || original_url == NULL || short_code == NULL) {
        return NULL;
    }

    /* --- 1. 分配新节点 --- */
    url_node_t* node = (url_node_t*)malloc(sizeof(url_node_t));
    if (node == NULL) {
        fprintf(stderr, "[错误] 内存分配失败：无法创建新节点\n");
        return NULL;
    }

    /* --- 2. 拷贝原始 URL --- */
    size_t url_len = strlen(original_url);
    node->original_url = (char*)malloc(url_len + 1);
    if (node->original_url == NULL) {
        fprintf(stderr, "[错误] 内存分配失败：无法存储 URL\n");
        free(node);
        return NULL;
    }
    strcpy(node->original_url, original_url);

    /* --- 3. 填充数据域 --- */
    strncpy(node->short_code, short_code, 5);
    node->short_code[5]   = '\0';
    node->access_count    = 0;
    node->max_access      = max_access;
    node->next            = NULL;

    /* --- 4. 尾插法插入链表 --- */
    if (list->head == NULL) {
        /* 空链表：新节点既是头也是尾 */
        list->head = node;
        list->tail = node;
    } else {
        /* 非空：追加到尾部 */
        list->tail->next = node;
        list->tail       = node;
    }

    list->size++;
    return node;
}

/* -------------------------------------------------------------------------- */

url_node_t* list_find_by_short(url_list_t* list, const char* short_code)
{
    if (list == NULL || short_code == NULL) {
        return NULL;
    }

    url_node_t* curr = list->head;
    while (curr != NULL) {
        if (strcmp(curr->short_code, short_code) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL; /* 未找到 */
}

/* -------------------------------------------------------------------------- */

void list_print_all(const url_list_t* list)
{
    if (list == NULL) {
        return;
    }

    printf("\n");
    printf("┌──────────────────────────────────────────────────────────────────┐\n");
    printf("│                        短地址统计信息                            │\n");
    printf("├──────┬────────────────────────────────────────┬────────┬────────┤\n");
    printf("│ 序号 │ 短地址    │ 原始 URL                     │ 解析次数│ 有效次数│\n");
    printf("├──────┼───────────┼──────────────────────────────┼────────┼────────┤\n");

    if (list->head == NULL) {
        printf("│                           （暂无数据）                           │\n");
    } else {
        int idx = 1;
        url_node_t* curr = list->head;
        while (curr != NULL) {
            /* 截断太长的 URL */
            char url_display[31];
            if (strlen(curr->original_url) > 28) {
                strncpy(url_display, curr->original_url, 27);
                url_display[27] = '.';
                url_display[28] = '.';
                url_display[29] = '.';
                url_display[30] = '\0';
            } else {
                strcpy(url_display, curr->original_url);
            }

            /* 有效次数显示 */
            char max_str[16];
            if (curr->max_access == -1) {
                strcpy(max_str, "无限制");
            } else {
                sprintf(max_str, "%d", curr->max_access);
            }

            printf("│ %4d │ %-9s │ %-28s │ %6d │ %-6s │\n",
                   idx++,
                   curr->short_code,
                   url_display,
                   curr->access_count,
                   max_str);

            curr = curr->next;
        }
    }

    printf("├──────┴───────────┴──────────────────────────────┴────────┴────────┤\n");
    printf("│  总计：%d 条记录                                                   │\n", list->size);
    printf("└──────────────────────────────────────────────────────────────────┘\n");
}

/* -------------------------------------------------------------------------- */

int list_remove(url_list_t* list, const char* short_code)
{
    if (list == NULL || short_code == NULL || list->head == NULL) {
        return -1;
    }

    url_node_t* prev = NULL;
    url_node_t* curr = list->head;

    while (curr != NULL) {
        if (strcmp(curr->short_code, short_code) == 0) {
            /* 找到了，开始删除 */

            if (prev == NULL) {
                /* 删除的是头节点 */
                list->head = curr->next;
                if (list->head == NULL) {
                    list->tail = NULL;  /* 链表变空 */
                }
            } else {
                /* 删除的是中间或尾部节点 */
                prev->next = curr->next;
                if (curr == list->tail) {
                    list->tail = prev;  /* 更新尾指针 */
                }
            }

            free(curr->original_url);
            free(curr);
            list->size--;
            return 0;
        }

        prev = curr;
        curr = curr->next;
    }

    return -1; /* 未找到 */
}

/* -------------------------------------------------------------------------- */

void list_destroy(url_list_t* list)
{
    if (list == NULL) {
        return;
    }

    url_node_t* curr = list->head;
    while (curr != NULL) {
        url_node_t* next = curr->next;
        free(curr->original_url);
        free(curr);
        curr = next;
    }

    free(list);
}
