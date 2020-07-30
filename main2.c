#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define STAGE_NUM_ONE 1                            /* stage numbers */
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4

#define STAGE_HEADER_F "=========================" /* stage header frame
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
    int daily_lim, transaction_lim;
    date_t last_tran;
    int daily_tran_amount;
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
/* Linked-list codes declarations (modified) */
/* credit: Alistair Moffat's program "listop.c" */

/* contains information about data in the node and the pointer to the next
   node*/
typedef struct node node_t;

struct node {
    transaction_t tran_data;
    node_t *next;
};

/* contains information about the pointers to the head and foot of the
   linked list */
typedef struct {
    node_t *head;
    node_t *foot;
} list_t;

/* listops.c function prototypes (modified) */
list_t *make_empty_list(void);

void free_list(list_t *list);

list_t *insert_at_foot(list_t *list, transaction_t tran_data);
/*--------------------------------------------------------------------------*/

/* function prototypes */
void print_stage_header(int stage_num);

void stage_one(credit_card_t *one_credit_card);

void stage_two(credit_card_t credit_card_list[], int *num_credit_cards);

void stage_three(list_t *tran_list);

void stage_four(list_t *tran_list, credit_card_t credit_card_list[],
                 int num_credit_cards);

int read_one_record(credit_card_t *credit_card);

double avg_daily_limit(credit_card_t credit_card_list[],
                        int *num_credit_cards);

char* largest_tran_lim(credit_card_t credit_card_list[],
                        int *num_credit_cards);

int compare_id(credit_card_t credit_card_list[], int id_index_1,
                int id_index_2);

transaction_t *read_one_tran(void);

void record_and_check(list_t *tran_list, credit_card_t credit_card_list[],
                       int num_credit_cards);

int compare_card_id(const void *card_id1, const void *card_id2);

void check_limit(credit_card_t *card_id, node_t *tran);

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
    
    /* create an empty linked-list to store transaction informations */
    tran_list = make_empty_list();
    assert(tran_list);
    
    /* stage 3: reading the transactions using linked list */
    stage_three(tran_list);
    
    /* stage 4: checking for fraudulent transactions */
    stage_four(tran_list, credit_cards_list, num_credit_cards);
    
    /* free the memory allocated for the transaction list */
    free_list(tran_list);
    tran_list = NULL;
    
    return 0;
}

/* print stage header given stage number */
void
print_stage_header(int stage_num) {
    printf(STAGE_HEADER_F"Stage %d"STAGE_HEADER_F"\n", stage_num);
}

/* read an input of credit card record into one_credit_card and return TRUE if
    successful, FALSE if otherwise */
int
read_one_record(credit_card_t *one_credit_card) {
    int i = 0, c;
    /* holds a target id to be checked */
    char *target_id = (char *) malloc((CARD_ID_CHAR + 1) * sizeof(char));
    assert(target_id != NULL);
    
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
    
    free(target_id);
    
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
    /* compare and return the id with a smaller ASCII value */
    if (strcmp(credit_card_list[id_index_1].card_id,
         credit_card_list[id_index_2].card_id) > 0) {
        return id_index_2;
    }
    
    return id_index_1;
}

/* search for the card id with the largest transaction limit */
char
*largest_tran_lim(credit_card_t credit_card_list[], int *num_credit_cards) {
    int i, largest_tran = 0, card_index = 0;
    
    for (i = 0; i < *num_credit_cards; i++) {
    
        if (credit_card_list[i].transaction_lim >= largest_tran) {
            /* found a tie between two card ids */
            if (credit_card_list[i].transaction_lim == largest_tran) {
                /* choose the one with the smallest id */
                card_index = compare_id(credit_card_list, i, card_index);
                
            } else {
                /* otherwise record the largest transaction limit and
                   the card's index in the list*/
                largest_tran = credit_card_list[i].transaction_lim;
                card_index = i;
            }
        }
    }
    
    return credit_card_list[card_index].card_id;
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
    while (*num_credit_cards <= MAX_NUM_CARDS &&
            read_one_record(&credit_card_list[*num_credit_cards])) {
        
        *num_credit_cards += 1;
    }
    
    /* print a summary information about the list of credit cards recorded */
    printf("Number of credit cards: %d\n", *num_credit_cards);
    printf("Average daily limit: %.2f\n", avg_daily_limit(credit_card_list,
            num_credit_cards));
    printf("Card with the largest transaction limit: %s\n",
            largest_tran_lim(credit_card_list, num_credit_cards));
    
    printf("\n");
}

/* read one transaction information */
transaction_t
*read_one_tran(void) {
    int i = 0, c;
    /* allocate memory to record one transaction information */
    transaction_t *one_tran = (transaction_t *) malloc(sizeof(transaction_t));
    assert(one_tran != NULL);
        
    while ((c = getchar()) != EOF) {
        /* record transaction id, record the first character followed by
           characters until info separator (space) is reached */
        one_tran->tran_id[i++] = c;
        while ((c = getchar()) != INFO_SEPARATOR) {
            one_tran->tran_id[i++] = c;
        }
        one_tran->tran_id[i] = '\0';

        /* record credit card id associated with the transaction */
        i = 0;
        while ((c = getchar()) != INFO_SEPARATOR) {
            one_tran->card_id[i++] = c;
        }
        one_tran->card_id[i] = '\0';
    
        /* record the date and time of the transaction */
        scanf("%d:%d:%d:%d:%d:%d", &one_tran->date.yyyy,
               &one_tran->date.mm, &one_tran->date.dd,
                &one_tran->time.hr, &one_tran->time.min,
                 &one_tran->time.sec);
    
        /* record the amount of the transaction */
        scanf(" %d", &one_tran->amount);
        
        /* done reading */
        if ((c = getchar()) == '\n' || c == EOF) {
            return one_tran;
        }
    }
    
    /* no record found */
    return NULL;
}

/* stage 3: reading the transactions */
void 
stage_three(list_t *tran_list) {
    /* pointer to one transaction information */
    transaction_t *one_tran;
    /* pointer to the transaction record in the transaction list */
    node_t *tran_record;
    
    /* print stage header */
    print_stage_header(STAGE_NUM_THREE);
    
    /* read transactions information into the transaction list */
    while ((one_tran = read_one_tran()) != NULL) {
        insert_at_foot(tran_list, *one_tran);
    }
    
    /* traverse the transactions linked-list and print the transaction
       ids of each transaction */
    tran_record = tran_list->head;
    while (tran_record != NULL) {
        printf("%s\n", tran_record->tran_data.tran_id);
        tran_record = tran_record->next;
    }
    
    printf("\n");
}

/* check if each transaction exceeds the transaction limit or the daily
   limit of the corresponding credit card */
void
record_and_check(list_t *tran_list, credit_card_t credit_card_list[],
                  int num_credit_cards) {
    /* pointer to the transaction record in the transaction list */
    node_t *tran_record;
    /* a pointer to the card id */
    credit_card_t *ptr_id;
    
    /* traverse the transaction record linked-list */
    tran_record = tran_list->head;
    while (tran_record != NULL) {
        /* find the corresponding card id of the transaction */
        ptr_id = (credit_card_t *) bsearch(tran_record->tran_data.card_id,
                   credit_card_list, num_credit_cards, sizeof(credit_card_t),
                    compare_card_id);
        
        /* if the transaction is done on the same day as the last transaction
           of the credit card, increment the transaction amount of the last
           transaction by the amount of this transaction */
        if (ptr_id->last_tran.yyyy == tran_record->tran_data.date.yyyy &&
             ptr_id->last_tran.mm == tran_record->tran_data.date.mm &&
              ptr_id->last_tran.dd == tran_record->tran_data.date.dd) {
            
            ptr_id->daily_tran_amount += tran_record->tran_data.amount;
            
        } else {
            /* otherwise replace the date and amount of last transaction of 
               the credit card with the date and amount of this transaction */
            ptr_id->last_tran = tran_record->tran_data.date;
            ptr_id->daily_tran_amount = tran_record->tran_data.amount;
        }
        
        /* now check if each transaction exceeds the transaction limit or the
           daily limit of the corresponding credit card */
        check_limit(ptr_id, tran_record);
        
        tran_record = tran_record->next;
    }
}

/* checks if a transaction exceeds the transaction limit or the daily limit or
   both of the corresponding credit card */
void
check_limit(credit_card_t *card_id, node_t *tran) {

    /* first check for both limits then check one by one */
    if (card_id->daily_tran_amount > card_id->daily_lim &&
         tran->tran_data.amount > card_id->transaction_lim) {
        printf("%s             OVER_BOTH_LIMITS\n", tran->tran_data.tran_id);
        
    } else if (tran->tran_data.amount > card_id->transaction_lim) {
        printf("%s             OVER_TRANS_LIMIT\n", tran->tran_data.tran_id);
        
    } else if (card_id->daily_tran_amount > card_id->daily_lim) {
        printf("%s             OVER_DAILY_LIMIT\n", tran->tran_data.tran_id);
        
    } else {
        printf("%s             IN_BOTH_LIMITS\n", tran->tran_data.tran_id);
    }
    
}

/* stage 4: checking for fraudulent transactions */
void
stage_four(list_t *tran_list, credit_card_t credit_card_list[],
            int num_credit_cards) {
    
    /* print stage header */
    print_stage_header(STAGE_NUM_FOUR);
    
    /* check if each transaction exceeds the transaction limit or the daily
       limit of the corresponding credit card */
    record_and_check(tran_list, credit_card_list, num_credit_cards);

}

/* comparison function for bsearch() to find the card_id */
int
compare_card_id(const void *card_id1, const void *card_id2) {
    return strcmp(((credit_card_t *) card_id1)->card_id,
                    ((credit_card_t *) card_id2)->card_id);
}

/*--------------------------------------------------------------------------*/
/* listops.c linked-list skeleton codes from lecture slide (modified) */

/* create an empty linked-list */
list_t
*make_empty_list(void) {
    list_t *list;
    list = (list_t *) malloc(sizeof(*list));
    assert(list != NULL);
    
    list->head = list->foot = NULL;
    
    return list;
}

/* free the memory allocated for the linked-list after use */
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

/* insert a node at the end of a linked-list */
list_t
*insert_at_foot(list_t *list, transaction_t tran_data) {
    node_t *new;
    new = (node_t *) malloc(sizeof(*new));
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

/*--------------------------------------------------------------------------*/

/* In stage 4, the function record_and_check traverse the transaction
   list containing n transactions and for each transaction binary search is
   used to search for the credit card id (from m credit cards in the record)
   corresponding to the transaction. Since the average time complexity of
   binary search is O(log m), therefore combining the two operations
   the time complexity in this stage will be O(nlogm). */


