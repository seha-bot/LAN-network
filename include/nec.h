typedef struct {
    long long size;
} nec_array;

#define nec(a) ((nec_array*)(a) - 1)
#define nec_size(a) ((a) ? nec(a)->size : 0)
#define nec_realloc(a, s)\
({\
    void* __nec_b;\
    int __nec_s = (s);\
    (a) ?\
    ((a) = realloc(nec(a), __nec_s * sizeof(*(a)) + sizeof(nec_array)), __nec_b = (nec_array*)(a) + 1, (a) = __nec_b):\
    ((a) = realloc((a), __nec_s * sizeof(*(a)) + sizeof(nec_array)), __nec_b = (nec_array*)(a) + 1, (a) = __nec_b);\
    nec(a)->size = __nec_s;\
})
#define nec_free(a) ({ void* __nec_b = nec(a); free(__nec_b); a = 0; })
#define nec_push(ar, v)\
({\
    nec_realloc(ar, nec_size(ar) + 1);\
    (ar)[nec_size(ar) - 1] = (v);\
})

#define nec_remove_at(t, a, i)\
({\
    if(i >= 0 && i < nec_size(a))\
    {\
        if(nec_size(a) == 1)\
        {\
            nec_free(a);\
            (a) = 0;\
        }\
        else if(i == nec_size(a) - 1) nec_realloc(a, nec_size(a) - 1);\
        else\
        {\
            void* ___nec_b = (a);\
            (a) = 0;\
            nec_realloc(a, nec_size(___nec_b) - 1);\
            int off = 0;\
            for(int it = 0; it < nec_size(___nec_b); it++)\
            {\
                if(it != i) (a)[it - off] = ((t*)(___nec_b))[it];\
                else off++;\
            }\
            nec_free(___nec_b);\
        }\
    }\
})

#define nec_remove(t, a, v)\
({\
    for(int i = 0; i < nec_size(a); i++)\
    {\
        if((a)[i] == v)\
        {\
            nec_remove_at(t, a, i);\
            break;\
        }\
    }\
})

#define nec_contains(__nec_a, __nec_v)\
({\
    char __nec_c = 0;\
    for(int __nec_i = 0; __nec_i < nec_size(__nec_a); __nec_i++)\
    {\
        if(__nec_v == __nec_a[__nec_i])\
        {\
            __nec_c = 1;\
            break;\
        }\
    }\
    __nec_c;\
})

/*
TODO:
add nec_push_norealloc
rename all one letter parameters into something similar to __nec_b
*/