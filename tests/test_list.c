#include <lib/assert.h>
#include <stdio.h>

#include <lib/list.h>

struct test_node {
    int value;
    struct list_head list;
};

static void test_init(void)
{
    LIST_HEAD(head);

    ASSERT(head.next == &head);
    ASSERT(head.prev == &head);
    ASSERT(list_empty(&head));
    ASSERT(list_is_singular(&head) == 0);
}

static void test_list_add(void)
{
    LIST_HEAD(head);

    struct test_node a = { .value = 1 };
    struct test_node b = { .value = 2 };

    INIT_LIST_HEAD(&a.list);
    INIT_LIST_HEAD(&b.list);

    list_add(&a.list, &head);
    ASSERT(head.next == &a.list);
    ASSERT(head.prev == &a.list);
    ASSERT(a.list.next == &head);
    ASSERT(a.list.prev == &head);

    list_add(&b.list, &head);

    ASSERT(head.next == &b.list);
    ASSERT(head.prev == &a.list);

    ASSERT(b.list.next == &a.list);
    ASSERT(b.list.prev == &head);

    ASSERT(a.list.next == &head);
    ASSERT(a.list.prev == &b.list);
}

static void test_list_add_tail(void)
{
    LIST_HEAD(head);

    struct test_node a = { .value = 1 };
    struct test_node b = { .value = 2 };

    INIT_LIST_HEAD(&a.list);
    INIT_LIST_HEAD(&b.list);

    list_add_tail(&a.list, &head);
    list_add_tail(&b.list, &head);

    ASSERT(head.next == &a.list);
    ASSERT(head.prev == &b.list);

    ASSERT(a.list.next == &b.list);
    ASSERT(a.list.prev == &head);

    ASSERT(b.list.next == &head);
    ASSERT(b.list.prev == &a.list);
}

static void test_list_del(void)
{
    LIST_HEAD(head);

    struct test_node a = { .value = 1 };
    struct test_node b = { .value = 2 };
    struct test_node c = { .value = 3 };

    INIT_LIST_HEAD(&a.list);
    INIT_LIST_HEAD(&b.list);
    INIT_LIST_HEAD(&c.list);

    list_add_tail(&a.list, &head);
    list_add_tail(&b.list, &head);
    list_add_tail(&c.list, &head);

    list_del(&b.list);

    ASSERT(head.next == &a.list);
    ASSERT(head.prev == &c.list);

    ASSERT(a.list.next == &c.list);
    ASSERT(c.list.prev == &a.list);

    ASSERT(b.list.next == NULL);
    ASSERT(b.list.prev == NULL);
}

static void test_list_del_init(void)
{
    LIST_HEAD(head);

    struct test_node node = { .value = 42 };

    INIT_LIST_HEAD(&node.list);
    list_add(&node.list, &head);

    list_del_init(&node.list);

    ASSERT(list_empty(&head));

    ASSERT(node.list.next == &node.list);
    ASSERT(node.list.prev == &node.list);

    ASSERT(list_empty(&node.list));
}

static void test_list_replace(void)
{
    LIST_HEAD(head);

    struct test_node old = { .value = 1 };
    struct test_node node = { .value = 2 };
    struct test_node tail = { .value = 3 };

    INIT_LIST_HEAD(&old.list);
    INIT_LIST_HEAD(&node.list);
    INIT_LIST_HEAD(&tail.list);

    list_add_tail(&old.list, &head);
    list_add_tail(&tail.list, &head);

    /*
     * Replace the first element:
     *
     * head <-> old <-> tail
     *
     * becomes:
     *
     * head <-> node <-> tail
     */
    list_replace(&old.list, &node.list);

    ASSERT(head.next == &node.list);
    ASSERT(head.prev == &tail.list);

    ASSERT(node.list.prev == &head);
    ASSERT(node.list.next == &tail.list);

    ASSERT(tail.list.prev == &node.list);
    ASSERT(tail.list.next == &head);
}

static void test_list_replace_init(void)
{
    LIST_HEAD(head);

    struct test_node old = { .value = 1 };
    struct test_node node = { .value = 2 };

    INIT_LIST_HEAD(&old.list);
    INIT_LIST_HEAD(&node.list);

    list_add(&old.list, &head);

    list_replace_init(&old.list, &node.list);

    ASSERT(head.next == &node.list);
    ASSERT(head.prev == &node.list);

    ASSERT(node.list.next == &head);
    ASSERT(node.list.prev == &head);

    ASSERT(old.list.next == &old.list);
    ASSERT(old.list.prev == &old.list);
}

static void test_list_move(void)
{
    LIST_HEAD(src);
    LIST_HEAD(dst);

    struct test_node a = { .value = 1 };
    struct test_node b = { .value = 2 };

    INIT_LIST_HEAD(&a.list);
    INIT_LIST_HEAD(&b.list);

    list_add_tail(&a.list, &src);
    list_add_tail(&b.list, &dst);

    list_move(&a.list, &dst);

    ASSERT(list_empty(&src));

    ASSERT(dst.next == &a.list);
    ASSERT(a.list.prev == &dst);
    ASSERT(a.list.next == &b.list);
    ASSERT(b.list.prev == &a.list);
    ASSERT(b.list.next == &dst);
}

static void test_list_move_tail(void)
{
    LIST_HEAD(src);
    LIST_HEAD(dst);

    struct test_node a = { .value = 1 };
    struct test_node b = { .value = 2 };

    INIT_LIST_HEAD(&a.list);
    INIT_LIST_HEAD(&b.list);

    list_add_tail(&a.list, &src);
    list_add_tail(&b.list, &dst);

    list_move_tail(&a.list, &dst);

    ASSERT(list_empty(&src));

    ASSERT(dst.next == &b.list);
    ASSERT(dst.prev == &a.list);

    ASSERT(b.list.next == &a.list);
    ASSERT(b.list.prev == &dst);

    ASSERT(a.list.next == &dst);
    ASSERT(a.list.prev == &b.list);
}

static void test_list_splice(void)
{
    LIST_HEAD(src);
    LIST_HEAD(dst);

    struct test_node a = { .value = 1 };
    struct test_node b = { .value = 2 };
    struct test_node c = { .value = 3 };

    INIT_LIST_HEAD(&a.list);
    INIT_LIST_HEAD(&b.list);
    INIT_LIST_HEAD(&c.list);

    list_add_tail(&a.list, &src);
    list_add_tail(&b.list, &src);
    list_add_tail(&c.list, &dst);

    list_splice(&src, &dst);

    ASSERT(dst.next == &a.list);
    ASSERT(a.list.next == &b.list);
    ASSERT(b.list.next == &c.list);
    ASSERT(c.list.next == &dst);

    ASSERT(dst.prev == &c.list);
    ASSERT(c.list.prev == &b.list);
    ASSERT(b.list.prev == &a.list);
    ASSERT(a.list.prev == &dst);

    /*
     * list_splice() does not reinitialize src.
     * Restore it manually before using it again.
     */
    INIT_LIST_HEAD(&src);
}

static void test_list_splice_tail(void)
{
    LIST_HEAD(src);
    LIST_HEAD(dst);

    struct test_node a = { .value = 1 };
    struct test_node b = { .value = 2 };
    struct test_node c = { .value = 3 };

    INIT_LIST_HEAD(&a.list);
    INIT_LIST_HEAD(&b.list);
    INIT_LIST_HEAD(&c.list);

    list_add_tail(&a.list, &src);
    list_add_tail(&b.list, &src);
    list_add_tail(&c.list, &dst);

    list_splice_tail(&src, &dst);

    ASSERT(dst.next == &c.list);
    ASSERT(dst.prev == &b.list);

    ASSERT(c.list.next == &a.list);
    ASSERT(a.list.next == &b.list);
    ASSERT(b.list.next == &dst);

    ASSERT(b.list.prev == &a.list);
    ASSERT(a.list.prev == &c.list);
    ASSERT(c.list.prev == &dst);

    INIT_LIST_HEAD(&src);
}

static void test_list_splice_init(void)
{
    LIST_HEAD(src);
    LIST_HEAD(dst);

    struct test_node a = { .value = 1 };
    struct test_node b = { .value = 2 };

    INIT_LIST_HEAD(&a.list);
    INIT_LIST_HEAD(&b.list);

    list_add_tail(&a.list, &src);
    list_add_tail(&b.list, &dst);

    list_splice_init(&src, &dst);

    ASSERT(list_empty(&src));

    ASSERT(dst.next == &a.list);
    ASSERT(dst.prev == &b.list);

    ASSERT(a.list.prev == &dst);
    ASSERT(a.list.next == &b.list);

    ASSERT(b.list.prev == &a.list);
    ASSERT(b.list.next == &dst);

    ASSERT(src.next == &src);
    ASSERT(src.prev == &src);
}

static void test_list_empty(void)
{
    LIST_HEAD(head);

    struct test_node node = { .value = 1 };

    INIT_LIST_HEAD(&node.list);

    ASSERT(list_empty(&head));

    list_add(&node.list, &head);

    ASSERT(!list_empty(&head));

    list_del(&node.list);

    ASSERT(list_empty(&head));
}

static void test_list_is_singular(void)
{
    LIST_HEAD(head);

    struct test_node a = { .value = 1 };
    struct test_node b = { .value = 2 };

    INIT_LIST_HEAD(&a.list);
    INIT_LIST_HEAD(&b.list);

    ASSERT(!list_is_singular(&head));

    list_add(&a.list, &head);

    ASSERT(list_is_singular(&head));

    list_add(&b.list, &head);

    ASSERT(!list_is_singular(&head));

    list_del(&b.list);

    ASSERT(list_is_singular(&head));
}

static void test_container_of(void)
{
    struct test_node node = {
        .value = 42
    };

    INIT_LIST_HEAD(&node.list);

    struct test_node *result =
        container_of(&node.list, struct test_node, list);

    ASSERT(result == &node);
    ASSERT(result->value == 42);
}

static void test_list_entry(void)
{
    LIST_HEAD(head);

    struct test_node node = {
        .value = 42
    };

    INIT_LIST_HEAD(&node.list);
    list_add(&node.list, &head);

    struct test_node *result =
        list_entry(head.next, struct test_node, list);

    ASSERT(result == &node);
    ASSERT(result->value == 42);
}

static void test_first_last_entry(void)
{
    LIST_HEAD(head);

    struct test_node a = { .value = 1 };
    struct test_node b = { .value = 2 };
    struct test_node c = { .value = 3 };

    INIT_LIST_HEAD(&a.list);
    INIT_LIST_HEAD(&b.list);
    INIT_LIST_HEAD(&c.list);

    list_add_tail(&a.list, &head);
    list_add_tail(&b.list, &head);
    list_add_tail(&c.list, &head);

    struct test_node *first =
        list_first_entry(&head, struct test_node, list);

    struct test_node *last =
        list_last_entry(&head, struct test_node, list);

    ASSERT(first == &a);
    ASSERT(last == &c);
}

static void test_next_prev_entry(void)
{
    LIST_HEAD(head);

    struct test_node a = { .value = 1 };
    struct test_node b = { .value = 2 };
    struct test_node c = { .value = 3 };

    INIT_LIST_HEAD(&a.list);
    INIT_LIST_HEAD(&b.list);
    INIT_LIST_HEAD(&c.list);

    list_add_tail(&a.list, &head);
    list_add_tail(&b.list, &head);
    list_add_tail(&c.list, &head);

    ASSERT(list_next_entry(&a, list) == &b);
    ASSERT(list_next_entry(&b, list) == &c);

    ASSERT(list_prev_entry(&c, list) == &b);
    ASSERT(list_prev_entry(&b, list) == &a);
}

static void test_list_for_each(void)
{
    LIST_HEAD(head);

    struct test_node a = { .value = 1 };
    struct test_node b = { .value = 2 };
    struct test_node c = { .value = 3 };

    INIT_LIST_HEAD(&a.list);
    INIT_LIST_HEAD(&b.list);
    INIT_LIST_HEAD(&c.list);

    list_add_tail(&a.list, &head);
    list_add_tail(&b.list, &head);
    list_add_tail(&c.list, &head);

    struct list_head *pos;
    int expected = 1;

    list_for_each(pos, &head) {
        struct test_node *node =
            list_entry(pos, struct test_node, list);

        ASSERT(node->value == expected);
        expected++;
    }

    ASSERT(expected == 4);
}

static void test_list_for_each_prev(void)
{
    LIST_HEAD(head);

    struct test_node a = { .value = 1 };
    struct test_node b = { .value = 2 };
    struct test_node c = { .value = 3 };

    INIT_LIST_HEAD(&a.list);
    INIT_LIST_HEAD(&b.list);
    INIT_LIST_HEAD(&c.list);

    list_add_tail(&a.list, &head);
    list_add_tail(&b.list, &head);
    list_add_tail(&c.list, &head);

    struct list_head *pos;
    int expected = 3;

    list_for_each_prev(pos, &head) {
        struct test_node *node =
            list_entry(pos, struct test_node, list);

        ASSERT(node->value == expected);
        expected--;
    }

    ASSERT(expected == 0);
}

static void test_list_for_each_safe(void)
{
    LIST_HEAD(head);

    struct test_node a = { .value = 1 };
    struct test_node b = { .value = 2 };
    struct test_node c = { .value = 3 };

    INIT_LIST_HEAD(&a.list);
    INIT_LIST_HEAD(&b.list);
    INIT_LIST_HEAD(&c.list);

    list_add_tail(&a.list, &head);
    list_add_tail(&b.list, &head);
    list_add_tail(&c.list, &head);

    struct list_head *pos;
    struct list_head *tmp;

    list_for_each_safe(pos, tmp, &head) {
        list_del(pos);
    }

    ASSERT(list_empty(&head));

    ASSERT(a.list.next == NULL);
    ASSERT(a.list.prev == NULL);

    ASSERT(b.list.next == NULL);
    ASSERT(b.list.prev == NULL);

    ASSERT(c.list.next == NULL);
    ASSERT(c.list.prev == NULL);
}

static void test_list_for_each_entry(void)
{
    LIST_HEAD(head);

    struct test_node a = { .value = 10 };
    struct test_node b = { .value = 20 };
    struct test_node c = { .value = 30 };

    INIT_LIST_HEAD(&a.list);
    INIT_LIST_HEAD(&b.list);
    INIT_LIST_HEAD(&c.list);

    list_add_tail(&a.list, &head);
    list_add_tail(&b.list, &head);
    list_add_tail(&c.list, &head);

    struct test_node *pos;
    int expected = 10;

    list_for_each_entry(pos, &head, list) {
        ASSERT(pos->value == expected);
        expected += 10;
    }

    ASSERT(expected == 40);
}

static void test_list_for_each_entry_safe(void)
{
    LIST_HEAD(head);

    struct test_node a = { .value = 1 };
    struct test_node b = { .value = 2 };
    struct test_node c = { .value = 3 };

    INIT_LIST_HEAD(&a.list);
    INIT_LIST_HEAD(&b.list);
    INIT_LIST_HEAD(&c.list);

    list_add_tail(&a.list, &head);
    list_add_tail(&b.list, &head);
    list_add_tail(&c.list, &head);

    struct test_node *pos;
    struct test_node *tmp;

    list_for_each_entry_safe(pos, tmp, &head, list) {
        list_del(&pos->list);
    }

    ASSERT(list_empty(&head));

    ASSERT(a.list.next == NULL);
    ASSERT(a.list.prev == NULL);

    ASSERT(b.list.next == NULL);
    ASSERT(b.list.prev == NULL);

    ASSERT(c.list.next == NULL);
    ASSERT(c.list.prev == NULL);
}

int main(void)
{
    test_init();

    test_list_add();
    test_list_add_tail();

    test_list_del();
    test_list_del_init();

    test_list_replace();
    test_list_replace_init();

    test_list_move();
    test_list_move_tail();

    test_list_splice();
    test_list_splice_tail();
    test_list_splice_init();

    test_list_empty();
    test_list_is_singular();

    test_container_of();
    test_list_entry();

    test_first_last_entry();
    test_next_prev_entry();

    test_list_for_each();
    test_list_for_each_prev();
    test_list_for_each_safe();

    test_list_for_each_entry();
    test_list_for_each_entry_safe();

    int num_failures = get_g_assert_failures();

    if (num_failures) {
        printf("test_list: number of failures: %i\n", num_failures);
    }
    else {
        printf("test_list: ok.\n");
    }

    return 0;
}