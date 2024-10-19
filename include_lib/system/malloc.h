#ifndef _MEM_HEAP_H_
#define _MEM_HEAP_H_

#ifdef __cplusplus
extern "C" {
#endif

extern void *malloc(unsigned long size);
extern void *zalloc(unsigned int size);
extern void *calloc(unsigned long count, unsigned long size);
extern void *realloc(void *rmem, unsigned long newsize);
extern void  free(void *mem);


extern void *kmalloc(unsigned long size, int flags);
extern void *vmalloc(unsigned long size);
extern void vfree(void *addr);
extern void *kzalloc(unsigned int len, int a);
extern void kfree(void *p);

extern void malloc_stats(void);

extern void malloc_dump();

extern void *my_mem_heap_create(void *base, int size);
void my_free(void *heap, void *rmem);
void *my_malloc(void *heap, unsigned long size);
void my_malloc_stats(void *heap);
void my_malloc_dump(void *heap);


#ifdef __cplusplus
}
#endif

#endif /* _MEM_HEAP_H_ */
