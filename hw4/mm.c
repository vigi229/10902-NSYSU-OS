#include "mm.h"

#define NALLOC 1024 /* minimum units to request */

union header{  /* block header */
	struct{
		union header *next_ptr; /* next block if on free list */
		size_t size; /* size of this block */
	} s;
	long x; /* force alignment of blocks, never used*/
};

typedef union header Header;

static Header base; /* empty list to get started */
static Header *free_p = NULL; /* start of free list */

/* morecore: ask system for more memory */
static Header *morecore(size_t new_units)
{
	char *cur_p,*sbrk(int);		/* cur_p : current pointer */
	Header *up_ptr;
	
	if (new_units < NALLOC)		/* At least NALLOC units */
		new_units = NALLOC;
	cur_p = sbrk(new_units * sizeof(Header));	/* sbrk returns a pointer to n  more bytes of storage. */
	
	/* no space at all */
	if (cur_p == (char *) -1) 	/* -1 if sbrk can not find space. */
		return NULL;
	
	/* make up equal to cp and set size */
	up_ptr = (Header *)cur_p;
	up_ptr->s.size = new_units;
	
	/* put up into free list */
	myfree((void *)(up_ptr+1));
	
	return free_p;
}

/* mymalloc: general-purpose storage allocator */
void *mymalloc(size_t size)
{
	Header *p, *pre_p;	/* point to free list */
	Header *morecore(size_t);
	size_t new_units; /* a number of Header we need to get */
	
	new_units = (size + sizeof(Header)-1) / sizeof(Header) + 1; 
	
	if ((pre_p = free_p) == NULL)	/* no free list yet .One block of size zero,point to itself. */
	{ 
		base.s.next_ptr = free_p = pre_p = &base;
		base.s.size = 0;
	}
	for (p = pre_p->s.next_ptr ; ; pre_p = p, p = p->s.next_ptr)
	{
		if(p->s.size >= new_units)	/* big enough */
		{
			if (p->s.size == new_units) /* exactly */
				pre_p->s.next_ptr = p->s.next_ptr;
			else   /* allocate tail end */
			{
				p->s.size -= new_units;
				p += p->s.size;
				p->s.size = new_units;
			}
			free_p = pre_p;
			return (void *)(p+1);
		}
		if (p == free_p)	/* wrapped around free list */
			if ((p = morecore(new_units)) == NULL) /* none left */
				return NULL;
	}
}
/* free: put block ap(ptr) in free list */
void myfree(void *ptr)
{
	Header *bp, *p;

	bp = (Header *)ptr - 1;            /* point to block header */
	for (p = free_p ; !(bp > p && bp < p->s.next_ptr) ; p = p->s.next_ptr)
	{
		/* freed block at start or end of arena */
		if (p >= p->s.next_ptr && (bp > p || bp < p->s.next_ptr))
			break; 
	}
	/* join to upper neighbor */
	if (bp + bp->s.size == p->s.next_ptr)
	{ 
		bp->s.size += p->s.next_ptr->s.size;
		bp->s.next_ptr = p->s.next_ptr->s.next_ptr;
	}
	else
		bp->s.next_ptr = p->s.next_ptr;
	
	/* join to lower neighbor */
	if (p + p->s.size == bp)
	{
		p->s.size += bp->s.size;
		p->s.next_ptr = bp->s.next_ptr;
	}
	else
		p->s.next_ptr = bp;
	
	free_p = p;
}
void *myrealloc(void *ptr, size_t size)
{
	Header *bp;
	size_t new_units;
	
	if(ptr == NULL)                /* if the ptr points to null, not realloc, malloc new one */
		return mymalloc(size);
	bp = (Header *)ptr - 1;
	new_units = (size + sizeof(Header) - 1) / sizeof(Header) + 1;

	if(bp->s.size >= new_units)
		return ptr;

	void *np = mymalloc(size);

	if(np == NULL)
		return NULL;
	memcpy(np, ptr, (bp->s.size - 1) * sizeof(Header));
	
	myfree(ptr);

	return np;
} 
void *mycalloc(size_t nmemb, size_t size)
{
	size_t all = nmemb * size;
	void *np = mymalloc(all);
	if(np == NULL)
		return NULL;
	memset(np,0,all);
	return np;
}
