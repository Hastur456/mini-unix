#include <lib/list.h>

void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}

static inline void __list_add(
    struct list_head *node, 
    struct list_head *prev,
    struct list_head *next
) {
    next->prev = node;
    node->next = next;
    node->prev = prev;
    prev->next = node;
}

static inline void __list_del(
    struct list_head *prev,
    struct list_head *next
) {
    prev->next = next;
    next->prev = prev;
}

void list_add(
    struct list_head *node,
    struct list_head *head
) {
    __list_add(node, head, head->next);
}

void list_add_tail(
    struct list_head *node,
    struct list_head *head
) {
    __list_add(node, head->prev, head);
}

void list_del(struct list_head *entry) {
    __list_del(entry->prev, entry->next);
    entry->prev = NULL;
    entry->next = NULL;
}

void list_del_init(struct list_head *entry) {
    __list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
}

void list_replace(
    struct list_head *old,
    struct list_head *node
) {
    node->next = old->next;
    node->next->prev = node;
    node->prev = old->prev;
    node->prev->next = node;
}

void list_replace_init(struct list_head *old, struct list_head *node) {
    list_replace(old, node);
    INIT_LIST_HEAD(old);
}

void list_move(
    struct list_head *entry,
    struct list_head *head
) {
    __list_del(entry->prev, entry->next);
    list_add(entry, head);
}

void list_move_tail(
    struct list_head *entry,
    struct list_head *head
) {
    __list_del(entry->prev, entry->next);
    list_add_tail(entry, head);
}

static inline void __list_splice(
    const struct list_head *list,
    struct list_head *prev,
    struct list_head *next
) {
    struct list_head *first = list->next;
    struct list_head *last = list->prev;

    first->prev = prev;
    prev->next = first;

    last->next = next;
    next->prev = last;
}

void list_splice(struct list_head *list, struct list_head *head) {
    if (!list_empty(list)) {
        __list_splice(list, head, head->next);
    }
}

void list_splice_tail(struct list_head *list, struct list_head *head) {
    if (!list_empty(list)) {
        __list_splice(list, head->prev, head);
    }
}

void list_splice_init(struct list_head *list, struct list_head *head) {
    if (!list_empty(list)) {
        __list_splice(list, head, head->next);
        INIT_LIST_HEAD(list);
    }
}

int list_empty(const struct list_head *head) {
    return head->next == head;
}

int list_is_singular(const struct list_head *head) {
    return !list_empty(head) && (head->next == head->prev);
}
