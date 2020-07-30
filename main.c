/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*  Foundation of Algorithm Assignment 2                                    */
/*  (Fraudulent Transaction Identification)                                 */
/*  Developed by: Oliver Ming Hui Tan                                       */
/*  Student id: 1087948                                                     */
/*  Algorithms are fun!                                                     */
/*                                                                          */
/*  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define STAGE_NUM_ONE 1                            /* stage numbers */
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define STAGE_HEADER_L "=========================" /* stage header format
                                                      string */

#define INFO_SEPARATOR ' '                         /* information separator
                                                      (space) */
#define END_OF_RECORD "%%%%%%%%%%"                 /* end of credit card
                                                      records indicator  */

#define MAX_NUM_CARDS 100                          /* maximum number of card */

#define CARD_ID_CHAR 8                             /* number of characters in
                                                      card id */
#define TRAN_ID_CHAR 12                            /* number of characters in
                                                      transaction id */

#define TRUE 1                                     /* true */
#define FALSE 0                                    /* false */

/* holds a date in yyyy/mm/dd format */
typedef struct {
    int yyyy, mm, dd;
} date_t;

/* contains information about a credit card including date of the last
   transaction and the total transaction amount of that day */
typedef struct {
    char card_id[CARD_ID_CHAR + 1];
    int daily_lim, transaction_lim, daily_tran_amount;
    date_t last_tran;
} credit_card_t;

/* holds a time in hr:min:sec format */
typedef struct {
    int hr, min, sec;
} time_info_t;

/* contains information about a transaction */
typedef struct {
    char tran_id[TRAN_ID_CHAR + 1], card_id[CARD_ID_CHAR + 1];
    date_t date;
    time_info_t time;
    int amount;
} transaction_t;

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
/*--------------------------------------------------------------------------*/

/* function prototypes */
void print_stage_header(int stage_num);
void stage_one(credit_card_t *one_credit_card);
void stage_two(credit_card_t credit_card_list[], int *num_credit_cards);
void stage_three(list_t *list);
void stage_four(list_t *list, credit_card_t credit_card_list[],
                int num_credit_cards);

int read_one_record(credit_card_t *credit_card);
double avg_daily_limit(credit_card_t credit_card_list[],
                       int *num_credit_cards);
char* largest_tran_lim(credit_card_t credit_card_list[],
                       int *num_credit_cards);
int compare_id(credit_card_t credit_card_list[], int id_index_1,
               int id_index_2);
transaction_t *read_one_transaction(void);
void record_and_check(list_t *list, credit_card_t credit_card_list[], int                               num_credit_cards);
int compare_card_id(const void *card_id1, const void *card_id2);
/*--------------------------------------------------------------------------*/

int
main(int argc, char *argv[]) {
    /* to hold all input credit cards */
    credit_card_t credit_cards_list[MAX_NUM_CARDS];
    /* to hold the number of input credit cards */
    int num_credit_cards = 0;
    /* to hold all input transaction */
    list_t *tran_list;
    
    /* stage 1: reading one credit card record */
    stage_one(&credit_cards_list[num_credit_cards]);
    num_credit_cards++;

    /* stage 2: reading all credit card records*/
    stage_two(credit_cards_list, &num_credit_cards);
    
    /* stage 3: reading the transactions using linked list */
    /* create an empty linked-list to store transaction informations */
    tran_list = make_empty_list();
    stage_three(tran_list);
    
    /* stage 4: checking for fraudulent transactions */
    stage_four(tran_list, credit_cards_list, num_credit_cards);
    
    free(tran_list);

    return 0;
}

/* print stage header given stage number */
void
print_stage_header(int stage_num) {
    printf(STAGE_HEADER_L"Stage %d"STAGE_HEADER_L"\n", stage_num);
}

/* read an input of credit card record into one_credit_card and return TRUE if successful, FALSE if otherwise */
int
read_one_record(credit_card_t *one_credit_card) {
    int i = 0, c;
    /* holds a target id to be checked */
    char target_id[CARD_ID_CHAR + 1];
    
    /* read the card id and check if the end of credit card record is
       reached */
    while ((c = getchar()) != INFO_SEPARATOR && c != '\n') {
        target_id[i++] = c;
    }
    target_id[i] = '\0';
    
    /* if end of credit card record is reached, return FALSE to stop reading */
    if (strcmp(target_id, END_OF_RECORD) == 0) {
        return FALSE;
    } else {
        /* else record it to one_credit_card */
        strcpy(one_credit_card->card_id, target_id);
    }
    
    /* record the daily limit and transaction limit of the credit card */
    scanf("%d %d", &one_credit_card->daily_lim,
          &one_credit_card->transaction_lim);
    
    /* check if transaction limit exceeds daily limit */
    if ((c = getchar()) == EOF || c == '\n') {
        if (one_credit_card->transaction_lim > one_credit_card->daily_lim) {
            printf("Error: Transaction limit exceeded daily limit!\n");
            exit(EXIT_FAILURE);
        }
    }
    
    /* initialize the last transaction date and transaction amount on that day
       to zero */
    one_credit_card->daily_tran_amount = 0;
    one_credit_card->last_tran.yyyy = 0;
    one_credit_card->last_tran.mm = 0;
    one_credit_card->last_tran.dd = 0;
    
    return TRUE;
}

/* calculate the average daily limit of all credit cards in the list */
double
avg_daily_limit(credit_card_t credit_card_list[], int *num_credit_cards) {
    int i;
    double sum = 0;
    
    for (i = 0; i < *num_credit_cards; i++) {
        sum += credit_card_list[i].daily_lim;
    }
    
    return sum / *num_credit_cards;
}

/* compare two card ids and return the one with the smallest id */
int
compare_id(credit_card_t credit_card_list[], int id_index_1, int id_index_2) {
    
    if (strcmp(credit_card_list[id_index_1].card_id, credit_card_list[id_index_2].card_id) > 0) {
        return id_index_2;
    }
    return id_index_1;
}

/* search for the card id with the largest transaction limit */
char
*largest_tran_lim(credit_card_t credit_card_list[], int *num_credit_cards) {
    int i, largest_tran = 0, index = 0;
    
    for (i = 0; i < *num_credit_cards; i++) {
        
        if (credit_card_list[i].transaction_lim >= largest_tran) {
            /* found a tie between two card ids */
            if (credit_card_list[i].transaction_lim == largest_tran) {
                /* choose the one with the smallest id */
                index = compare_id(credit_card_list, i, index);
                
            } else {
                /* otherwise record the largest transaction limit and
                   the card's index in the list*/
                largest_tran = credit_card_list[i].transaction_lim;
                index = i;
            }
        }
    }
    return credit_card_list[index].card_id;
}

/* stage 1: reading one credit card record */
void
stage_one(credit_card_t *one_credit_card) {
    
    /* print stage header */
    print_stage_header(STAGE_NUM_ONE);
    
    /* read the first credit card record */
    read_one_record(one_credit_card);
    
    /* print information about the credit card */
    printf("Card ID: %s\n", one_credit_card->card_id);
    printf("Daily limit: %d\n", one_credit_card->daily_lim);
    printf("Transaction limit: %d\n", one_credit_card->transaction_lim);

    printf("\n");
}

/* stage 2: reading all credit card records */
void
stage_two(credit_card_t credit_card_list[], int *num_credit_cards) {
    
    /* print stage header */
    print_stage_header(STAGE_NUM_TWO);
    
    /* read the subsequent credit cards and keep track of the number of
       credit cards recorded */
    while (*num_credit_cards <= MAX_NUM_CARDS && read_one_record(&credit_card_list[*num_credit_cards])) {
        *num_credit_cards += 1;
    }
    
    /* print a summary information about the list of credit cards recorded */
    printf("Number of credit cards: %d\n", *num_credit_cards);
    printf("Average daily limit: %.2f\n", avg_daily_limit(credit_card_list, num_credit_cards));
    printf("Card with the largest transaction limit: %s\n", largest_tran_lim(credit_card_list, num_credit_cards));
    
    printf("\n");
}

/* read one transaction information */
transaction_t
*read_one_transaction(void){
    int i = 0, c;
    /* allocate memory to record one transaction information */
    transaction_t *one_transaction = malloc(sizeof(transaction_t));
        
    while ((c = getchar()) != EOF) {
        /* record transaction id */
        one_transaction->tran_id[i++] = c;
        while ((c = getchar()) != INFO_SEPARATOR) {
        one_transaction->tran_id[i++] = c;
        }
        one_transaction->tran_id[i] = '\0';

        /* record credit card id associated with the transaction */
        i = 0;
        while ((c = getchar()) != INFO_SEPARATOR) {
            one_transaction->card_id[i++] = c;
        }
        one_transaction->card_id[i] = '\0';
    
        /* record the date and time of the transaction */
        scanf("%d:%d:%d:%d:%d:%d", &one_transaction->date.yyyy, &one_transaction->date.mm,
        &one_transaction->date.dd, &one_transaction->time.hr, &one_transaction->time.min,
          &one_transaction->time.sec);
    
        /* record the amount of the transaction */
        scanf(" %d", &one_transaction->amount);
        
        /* done reading */
        if ((c = getchar()) == '\n' || c == EOF) {
            return one_transaction;
        }
    }
    return NULL;
}

/* stage 3: reading the transactions */
void stage_three(list_t *list) {
    /* holds one transaction information */
    transaction_t *one_transaction;
    /* holds the transaction records */
    node_t *transaction_records;
    
    /* print stage header */
    print_stage_header(STAGE_NUM_THREE);
    
    /* read transactions information into the transaction records
       linked list */
    while((one_transaction = read_one_transaction()) != NULL) {
        insert_at_foot(list, *one_transaction);
    }
    
    /* traverse the transaction records linked-list and print the transaction
       ids of each transaction */
    transaction_records = list->head;
    while(transaction_records) {
        printf("%s\n", transaction_records->tran_data.tran_id);
        transaction_records = transaction_records->next;
    }
    
    printf("\n");
}

/* check if each transaction exceeds the transaction limit or the daily
   limit of the corresponding credit card */
void
record_and_check(list_t *list, credit_card_t credit_card_list[],
                 int num_credit_cards) {
    node_t *A;
    /* a pointer to the card id */
    credit_card_t *ptr_id;
    
    /* traverse the transaction record linked-list */
    A = list->head;
    while(A) {
        /* find the corresponding card id of the transaction */
        ptr_id = (credit_card_t *)bsearch(A->tran_data.card_id,
                    credit_card_list, num_credit_cards, sizeof(credit_card_t),
                         (int(*)(const void*,const void*)) compare_card_id);
        
        /* if the transaction is done on the same day as the last transaction
           of the credit card, increment the transaction amount of the last
           transaction by the amount of this transaction */
        if(ptr_id->last_tran.yyyy == A->tran_data.date.yyyy &&
            ptr_id->last_tran.mm == A->tran_data.date.mm &&
            ptr_id->last_tran.dd == A->tran_data.date.dd) {
            ptr_id->daily_tran_amount += A->tran_data.amount;
            
        } else {
            /* otherwise replace the date and amount of last transaction of the credit card with the date and amount of this transaction */
            ptr_id->last_tran = A->tran_data.date;
            ptr_id->daily_tran_amount = A->tran_data.amount;
        }
        
        /* now check if each transaction exceeds the transaction limit or the
           daily limit of the corresponding credit card */
        if (ptr_id->daily_tran_amount > ptr_id->daily_lim &&
            A->tran_data.amount > ptr_id->transaction_lim) {
            printf("%s             OVER_BOTH_LIMITS\n", A->tran_data.tran_id);
        } else if (A->tran_data.amount > ptr_id->transaction_lim) {
            printf("%s             OVER_TRANS_LIMIT\n", A->tran_data.tran_id);
        } else if (ptr_id->daily_tran_amount > ptr_id->daily_lim) {
            printf("%s             OVER_DAILY_LIMIT\n", A->tran_data.tran_id);
        } else {
            printf("%s             IN_BOTH_LIMITS\n", A->tran_data.tran_id);
        }
        
        A = A->next;
    }
}

/* stage 4: checking for fraudulent transactions */
void
stage_four(list_t *list, credit_card_t credit_card_list[],
           int num_credit_cards) {
    
    /* print stage header */
    print_stage_header(STAGE_NUM_FOUR);
    
    /* check if each transaction exceeds the transaction limit or the daily
       limit of the corresponding credit card */
    record_and_check(list, credit_card_list, num_credit_cards);
    
}

/* comparison function for bsearch() to find the card_id */
int
compare_card_id(const void *card_id1, const void *card_id2) {
    return strncmp(card_id1, ((const credit_card_t *)card_id2)->card_id,
                   sizeof(((credit_card_t *)0)->card_id));
}




//---------------------------------------------------------------------------//
/* listops skeleton codes from lecture slide */

list_t
*make_empty_list(void) {
    list_t *list;
    list = (list_t*)malloc(sizeof(*list));
    assert(list!=NULL);
    list->head = list->foot = NULL;
    return list;
}

int
is_empty_list(list_t *list) {
    assert(list!=NULL);
    return list->head==NULL;
}

void
free_list(list_t *list) {
    node_t *curr, *prev;
    assert(list!=NULL);
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
    new = (node_t*)malloc(sizeof(*new));
    assert(list!=NULL && new!=NULL);
    new->tran_data = tran_data;
    new->next = list->head;
    list->head = new;
    if (list->foot==NULL) {
        /* this is the first insertion into the list */
        list->foot = new;
    }
    return list;
}

list_t
*insert_at_foot(list_t *list, transaction_t tran_data) {
    node_t *new;
    new = (node_t*)malloc(sizeof(*new));
    assert(list!=NULL && new!=NULL);
    new->tran_data = tran_data;
    new->next = NULL;
    if (list->foot==NULL) {
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
    assert(list!=NULL && list->head!=NULL);
    return list->head->tran_data;
}

list_t
*get_tail(list_t *list) {
    node_t *oldhead;
    assert(list!=NULL && list->head!=NULL);
    oldhead = list->head;
    list->head = list->head->next;
    if (list->head==NULL) {
        /* the only list node just got deleted */
        list->foot = NULL;
    }
    free(oldhead);
    return list;
}
