/*-
 * Copyright (c) 1995 Michael B. Durian.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Michael B. Durian.
 * 4. The name of the the Author may be used to endorse or promote 
 *    products derived from this software without specific prior written 
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "sup_os.h"
#include "sup_stynamic.h"
#include "sup_event.h"

void
stynamic_add_byte(sty, byte)
	struct stynamic *sty;
	u_char byte;
{
	int s;
	u_char *new_ptr;

	s = BLOCK_INTR();
	if (sty->len < STYNAMIC_SIZE)
		sty->datas[sty->len++] = byte;
	else {
		if (sty->len < sty->allocated) {
			sty->datad[sty->len++] = byte;
		} else {
			new_ptr = MALLOC_WAIT(sty->allocated + STYNAMIC_ALLOC);
			if (sty->allocated == 0)
				BCOPY(sty->datas, new_ptr, sty->len);
			else {
				BCOPY(sty->datad, new_ptr, sty->len);
				FREE(sty->datad, sty->allocated);
			}
			sty->datad = new_ptr;
			sty->datad[sty->len++] = byte;
			sty->allocated += STYNAMIC_ALLOC;
		}
	}
	UNBLOCK_INTR(s);
}

void
stynamic_add_bytes(sty, bytes, num)
	struct stynamic *sty;
	u_char *bytes;
	int num;
{
	int s, size_inc;
	u_char *new_ptr;

	s = BLOCK_INTR();
	if (sty->len + num <= STYNAMIC_SIZE) {
		BCOPY(bytes, &sty->datas[sty->len], num);
		sty->len += num;
	} else {
		if (sty->len + num <= sty->allocated) {
			BCOPY(bytes, &sty->datad[sty->len], num);
			sty->len += num;
		} else {
			size_inc = (num / STYNAMIC_ALLOC + 1) * STYNAMIC_ALLOC;
			new_ptr = MALLOC_WAIT(sty->allocated + size_inc);
			if (sty->allocated == 0)
				BCOPY(sty->datas, new_ptr, sty->len);
			else {
				BCOPY(sty->datad, new_ptr, sty->len);
				FREE(sty->datad, sty->allocated);
			}
			sty->datad = new_ptr;
			BCOPY(bytes, &sty->datad[sty->len], num);
			sty->allocated += size_inc;
			sty->len += num;
		}
	}
	UNBLOCK_INTR(s);
}

u_char
stynamic_get_byte(sty, index)
	struct stynamic *sty;
	int index;
{

	if (sty->len <= STYNAMIC_SIZE)
		return (sty->datas[index]);
	else
		return (sty->datad[index]);
}

void
stynamic_copy(sty, dest, len)
	struct stynamic *sty;
	void *dest;
	int len;
{
	int s;

	s = BLOCK_INTR();
	if (sty->len <= STYNAMIC_SIZE)
		BCOPY(sty->datas, dest, len);
	else
		BCOPY(sty->datad, dest, len);
	UNBLOCK_INTR(s);
}

void
stynamic_append(dest, src)
	struct stynamic *dest;
	struct stynamic *src;
{

	if (src->len <= STYNAMIC_SIZE)
		stynamic_add_bytes(dest, src->datas, src->len);
	else
		stynamic_add_bytes(dest, src->datad, src->len);
}

#if 0
/* unused function */
void
stynamic_copy_from(sty, index, dest, len)
	struct stynamic *sty;
	int index, len;
	void *dest;
{
	int s;

	s = BLOCK_INTR();
	if (sty->len <= STYNAMIC_SIZE)
		BCOPY(&sty->datas[index], dest, len);
	else
		BCOPY(&sty->datad[index], dest, len);
	UNBLOCK_INTR(s);
}
#endif


void
stynamic_release(sty)
	struct stynamic *sty;
{
	int s;

	s = BLOCK_INTR();
	if (sty->len > STYNAMIC_SIZE)
		FREE(sty->datad, sty->allocated);
	sty->len = 0;
	sty->allocated = 0;
	sty->datad = NULL;
	BZERO(sty->datas, STYNAMIC_SIZE);
	UNBLOCK_INTR(s);
}

void
stynamic_shift(sty, num)
	struct stynamic *sty;
	int num;
{
	int rem, s;
	u_char *ptr;

	if (sty->len <= num) {
		stynamic_release(sty);
		return;
	}
	s = BLOCK_INTR();
	if (sty->len > STYNAMIC_SIZE)
		ptr = &sty->datad[num];
	else
		ptr = &sty->datas[num];
	rem = sty->len - num;
	if (rem > STYNAMIC_SIZE)
		BCOPY(ptr, sty->datad, rem);
	else {
		BCOPY(ptr, sty->datas, rem);
		if (sty->datad != NULL) {
			FREE(sty->datad, sty->allocated);
			sty->datad = NULL;
			sty->allocated = 0;
		}
	}
	sty->len = rem;
	UNBLOCK_INTR(s);
}

void
stynamic_init(sty)
	struct stynamic *sty;
{
	int s;

	s = BLOCK_INTR();
	sty->len = 0;
	sty->allocated = 0;
	sty->datad = NULL;
	BZERO(sty->datas, STYNAMIC_SIZE);
	UNBLOCK_INTR(s);
}

short
stynamic_len(sty)
	struct stynamic *sty;
{

	return (sty->len);
}

#if 0
/* unused function */
void
stynamic_print(sty)
	struct stynamic *sty;
{

	PRINTF("\t\tlen = %d\n", sty->len);
	PRINTF("\t\tallocated = %d\n", sty->allocated);
	PRINTF("\t\tdatad = 0x%x\n", sty->datad);
}
#endif
