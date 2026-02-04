typedef struct {
  unsigned red : 1 ;
  unsigned internal : 1 ;
  unsigned left : 1 ;
  unsigned root : 1 ;
  unsigned head : 1 ;
} status;

typedef struct rb_node {
  union {
    struct {
      struct rb_node *flink;
      struct rb_node *blink;
    } list;
    struct {
      struct rb_node *left;
      struct rb_node *right;
    } child;
  } c;
  union {
    struct rb_node *parent;
    struct rb_node *root;
  } p;
  status s;
  union {
    unsigned long ukey;
    int ikey;
    char *key;
    struct rb_node *lext;
  } k;
  union {
    char *val;
    struct rb_node *rext;
  } v;
} *Rb_node;

#ifndef __P
#if defined(__STDC__) || defined(__cplusplus)
#define __P(protos)	protos
#else 
#define __P(protos)	()
#endif
#endif

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

EXTERN Rb_node make_rb __P(());
EXTERN Rb_node rb_insert_b __P((Rb_node node, char *key, char *value));

EXTERN Rb_node rb_find_key __P((Rb_node tree, char *key));
EXTERN Rb_node rb_find_ikey __P((Rb_node tree, int ikey));
EXTERN Rb_node rb_find_ukey __P((Rb_node tree, unsigned long ukey));
EXTERN Rb_node rb_find_gkey __P((Rb_node tree, char *key, int (*fxn)()));

EXTERN Rb_node rb_find_key_n __P((Rb_node tree, char *key, int *found));
EXTERN Rb_node rb_find_ikey_n __P((Rb_node tree, int ikey, int *found));
EXTERN Rb_node rb_find_ukey_n __P((Rb_node tree, unsigned long ukey,
    int *found));
EXTERN Rb_node rb_find_gkey_n __P((Rb_node tree, char *key, int (*fxn)(),
    int *found));
EXTERN void rb_delete_node __P((Rb_node node));
EXTERN void rb_free_tree __P((Rb_node node));  /* Deletes and frees an entire tree */
EXTERN char *rb_val __P((Rb_node node));  /* Returns node->v.val
					     (this is to shut lint up */

#define rb_insert_a(n, k, v) rb_insert_b(n->c.list.flink, k, v)
#define rb_insert(t, k, v) rb_insert_b(rb_find_key(t, k), k, v)
#define rb_inserti(t, k, v) rb_insert_b(rb_find_ikey(t, k), (char *) k, v)
#define rb_insertu(t, k, v) rb_insert_b(rb_find_ukey(t, k), (char *) k, v)
#define rb_insertg(t, k, v, f) rb_insert_b(rb_find_gkey(t, k, f), k, v)
#define rb_first(n) (n->c.list.flink)
#define rb_last(n) (n->c.list.blink)
#define rb_next(n) (n->c.list.flink)
#define rb_prev(n) (n->c.list.blink)
#define rb_empty(t) (t->c.list.flink == t)
#ifndef nil
#define nil(t) (t)
#endif

#define rb_traverse(ptr, lst) \
  for(ptr = rb_first(lst); ptr != nil(lst); ptr = rb_next(ptr))

EXTERN void recolor __P(());
EXTERN void single_rotate __P(());
