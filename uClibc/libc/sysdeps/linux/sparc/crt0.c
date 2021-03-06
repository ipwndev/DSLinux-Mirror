/* vi: set sw=4 ts=4: */
/* uClibc/sysdeps/linux/i386/crt0.S
 * Pull stuff off the stack and get uClibc moving.
 *
 * Copyright (C) 2000,2001 by Erik Andersen <andersen@uclibc.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* Stick in a dummy reference to main(), so that if an application
 * is linking when the main() function is in a static library (.a)
 * we can be sure that main() actually gets linked in */
extern void main(int argc,void *argv,void *envp);
void (*mainp)(int argc,void *argv,void *envp) = main;

extern void __uClibc_main(int argc,void *argv,void *envp);


void _start(void)
{
	unsigned int argc;
	char **argv, **envp;
	unsigned long *stack;

	stack = ((unsigned long*)__builtin_frame_address(0))+16;
	argc = *stack;
	argv = (char **)stack + 1;
	envp = (char **)stack + argc + 2;

	__uClibc_main(argc, argv, envp);
}

