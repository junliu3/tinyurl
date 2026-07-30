/**
 * @file    linked_list.h
 * @brief   单链表模块 - 存储短地址映射关系
 *
 * 采用尾插法的单向链表，head 指向第一个节点，tail 指向最后一个节点。
 * 链表节点存储：原始URL、短地址码、访问计数、有效次数限制。
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

/* -------------------------------------------------------------------------- */
/*                                 数据结构定义                                  */
/* -------------------------------------------------------------------------- */

/** 短地址节点（链表的一个元素） */
typedef struct url_node {
    char*  original_url;        /* 原始 URL（动态分配）           */
    char   short_code[6];       /* 5 位短地址码 + '\0'           */
    int    access_count;        /* 当前已解析次数                 */
    int    max_access;          /* 最大有效次数（-1 表示无限制）   */
    struct url_node* next;      /* 后继指针                       */
} url_node_t;

/** 链表容器（管理整条链表） */
typedef struct {
    url_node_t*        head;    /* 头指针                         */
    url_node_t*        tail;    /* 尾指针（尾插法优化）            */
    int                size;    /* 当前节点数量                   */
    unsigned long long counter; /* 全局计数器，用于生成短地址 ID   */
} url_list_t;

/* -------------------------------------------------------------------------- */
/*                                 链表操作 API                                */
/* -------------------------------------------------------------------------- */

/**
 * @brief  创建一个空链表
 * @return 新链表指针，失败返回 NULL
 */
url_list_t* list_create(void);

/**
 * @brief  尾插法插入新节点
 * @param  list        目标链表
 * @param  original_url 原始 URL（内部会拷贝一份）
 * @param  short_code  短地址码（5 位 62 进制字符串）
 * @param  max_access  最大有效次数（-1 表示不限）
 * @return 新节点指针，失败返回 NULL
 */
url_node_t* list_insert(url_list_t* list,
                        const char* original_url,
                        const char* short_code,
                        int max_access);

/**
 * @brief  根据短地址码查找节点
 * @param  list       目标链表
 * @param  short_code 要查找的短地址码
 * @return 找到的节点指针，未找到返回 NULL
 */
url_node_t* list_find_by_short(url_list_t* list, const char* short_code);

/**
 * @brief  打印全部映射信息（统计视图）
 * @param  list 目标链表
 */
void list_print_all(const url_list_t* list);

/**
 * @brief  根据短地址码删除节点
 * @param  list       目标链表
 * @param  short_code 要删除的短地址码
 * @return 0 成功，-1 未找到
 */
int list_remove(url_list_t* list, const char* short_code);

/**
 * @brief  销毁整条链表，释放所有内存
 * @param  list 目标链表
 */
void list_destroy(url_list_t* list);

#endif /* LINKED_LIST_H */
