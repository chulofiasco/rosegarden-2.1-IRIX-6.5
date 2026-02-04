/*-
 * Copyright (c) 1995, 1996 Michael B. Durian.  All rights reserved.
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
#ifndef SUP_STYNAMIC_H
#define SUP_STYNAMIC_H

/*
 * most events are short, so we use the static array,
 * but occationally we get a long event (sysex) and
 * we dynamically allocate that
 */
#define STYNAMIC_SIZE 4
#define STYNAMIC_ALLOC 256

struct stynamic {
	short	allocated;
	short	len;
	u_char	datas[4];
	u_char	*datad;
};

extern void stynamic_init __P((struct stynamic *sty));
extern void stynamic_add_byte __P((struct stynamic *, int /* u_char */));
extern void stynamic_add_bytes __P((struct stynamic *, u_char *, int));
extern u_char stynamic_get_byte __P((struct stynamic *, int));
extern void stynamic_copy __P((struct stynamic *, void *, int));
extern void stynamic_append __P((struct stynamic *, struct stynamic *));
extern void stynamic_release __P((struct stynamic *));
extern void stynamic_shift __P((struct stynamic *, int));
extern short stynamic_len __P((struct stynamic *));

#endif
