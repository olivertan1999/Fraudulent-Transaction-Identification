/*--------------------------------------------------------------------------*/
/* listops.c declarations (modified) */

typedef struct node node_t;

struct node {
    transaction_t tran_data;
    node_t *next;
};

typedef struct {
    node_t *head;
    node_t *foot;
} list_t;

/* listops.c function prototypes (modified) */
list_t *make_empty_list(void);
int is_empty_list(list_t *list);
void free_list(list_t *list);
list_t *insert_at_head(list_t *list, transaction_t tran_data);
list_t *insert_at_foot(list_t *list, transaction_t tran_data);
transaction_t get_head(list_t *list);
list_t *get_tail(list_t *list);

//---------------------------------------------------------------------------//
/* listops skeleton codes */

list_t
*make_empty_list(void) {
    list_t *list;
    list = (list_t*) malloc(sizeof(*list));
    assert(list != NULL);
    list->head = list->foot = NULL;
    return list;
}

int
is_empty_list(list_t *list) {
    assert(list != NULL);
    return list->head == NULL;
}

void
free_list(list_t *list) {
    node_t *curr, *prev;
    assert(list != NULL);
    curr = list->head;
    while (curr) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }
    free(list);
}

list_t
*insert_at_head(list_t *list, transaction_t tran_data) {
    node_t *new;
    new = (node_t*) malloc(sizeof(*new));
    assert(list != NULL && new != NULL);
    new->tran_data = tran_data;
    new->next = list->head;
    list->head = new;
    if (list->foot == NULL) {
        /* this is the first insertion into the list */
        list->foot = new;
    }
    return list;
}

list_t
*insert_at_foot(list_t *list, transaction_t tran_data) {
    node_t *new;
    new = (node_t*) malloc(sizeof(*new));
    assert(list != NULL && new != NULL);
    new->tran_data = tran_data;
    new->next = NULL;
    if (list->foot == NULL) {
        /* this is the first insertion into the list */
        list->head = list->foot = new;
    } else {
        list->foot->next = new;
        list->foot = new;
    }
    return list;
}

transaction_t
get_head(list_t *list) {
    assert(list != NULL && list->head != NULL);
    return list->head->tran_data;
}

list_t
*get_tail(list_t *list) {
    node_t *oldhead;
    assert(list != NULL && list->head != NULL);
    oldhead = list->head;
    list->head = list->head->next;
    if (list->head == NULL) {
        /* the only list node just got deleted */
        list->foot = NULL;
    }
    free(oldhead);
    return list;
}
