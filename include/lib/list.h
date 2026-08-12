#ifndef LIST_H
#define LIST_H

#include <stddef.h>

/* Doubly-linked circular list */

struct list_head {
    struct list_head *next;
    struct list_head *prev;
};

/* Initialization */

#define LIST_HEAD_INIT(name) \
    { &(name), &(name) }

#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

void INIT_LIST_HEAD(struct list_head *list); 

/* Basic operations */

void list_add(struct list_head *node,
              struct list_head *head);

void list_add_tail(struct list_head *node,
                   struct list_head *head);

void list_del(struct list_head *entry);

void list_del_init(struct list_head *entry);

void list_replace(struct list_head *old,
                  struct list_head *node);

void list_replace_init(struct list_head *old,
                       struct list_head *node);

void list_move(struct list_head *entry,
               struct list_head *head);

void list_move_tail(struct list_head *entry,
                    struct list_head *head);

void list_splice(struct list_head *list,
                 struct list_head *head);

void list_splice_tail(struct list_head *list,
                      struct list_head *head);

void list_splice_init(struct list_head *list,
                      struct list_head *head);

/* Queries */

int list_empty(const struct list_head *head);

int list_is_singular(const struct list_head *head);

/* container_of */

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

/* Entry helpers */

#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

#define list_last_entry(ptr, type, member) \
    list_entry((ptr)->prev, type, member)

#define list_next_entry(pos, member) \
    list_entry((pos)->member.next, __typeof__(*pos), member)

#define list_prev_entry(pos, member) \
    list_entry((pos)->member.prev, __typeof__(*pos), member)

/* Iterators */

#define list_for_each(pos, head) \
    for ((pos) = (head)->next; (pos) != (head); (pos) = (pos)->next)

#define list_for_each_prev(pos, head) \
    for ((pos) = (head)->prev; (pos) != (head); (pos) = (pos)->prev)

#define list_for_each_safe(pos, tmp, head)                  \
    for ((pos) = (head)->next, (tmp) = (pos)->next;         \
         (pos) != (head);                                   \
         (pos) = (tmp), (tmp) = (pos)->next)

#define list_for_each_entry(pos, head, member)                          \
    for ((pos) = list_first_entry(head, __typeof__(*pos), member);      \
         &(pos)->member != (head);                                      \
         (pos) = list_next_entry(pos, member))

#define list_for_each_entry_safe(pos, tmp, head, member)                \
    for ((pos) = list_first_entry(head, __typeof__(*pos), member),      \
         (tmp) = list_next_entry(pos, member);                          \
         &(pos)->member != (head);                                      \
         (pos) = (tmp),                                                 \
         (tmp) = list_next_entry(tmp, member))

#endif
