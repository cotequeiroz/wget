/* Generic support for headers.
   Copyright (C) 1997, 1998 Free Software Foundation, Inc.

This file is part of GNU Wget.

GNU Wget is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

GNU Wget is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wget; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

In addition, as a special exception, the Free Software Foundation
gives permission to link the code of its release of Wget with the
OpenSSL project's "OpenSSL" library (or with modified versions of it
that use the same license as the "OpenSSL" library), and distribute
the linked executables.  You must obey the GNU General Public License
in all respects for all of the code used other than "OpenSSL".  If you
modify this file, you may extend this exception to your version of the
file, but you are not obligated to do so.  If you do not wish to do
so, delete this exception statement from your version.  */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#include "wget.h"
#include "connect.h"
#include "headers.h"

/* This file contains the generic routines for work with headers.
   Currently they are used only by HTTP in http.c, but they can be
   used by anything that cares about RFC822-style headers.

   Header is defined in RFC2068, as quoted below.  Note that this
   definition is not HTTP-specific -- it is virtually
   indistinguishable from the one given in RFC822 or RFC1036.

	message-header = field-name ":" [ field-value ] CRLF

	field-name     = token
	field-value    = *( field-content | LWS )

	field-content  = <the OCTETs making up the field-value
			 and consisting of either *TEXT or combinations
			 of token, tspecials, and quoted-string>

   The public functions are header_get() and header_process(), which
   see.  */


/* Check whether HEADER begins with NAME and, if yes, skip the `:' and
   the whitespace, and call PROCFUN with the arguments of HEADER's
   contents (after the `:' and space) and ARG.  Otherwise, return 0.  */
int
header_process (const char *header, const char *name,
		int (*procfun) (const char *, void *),
		void *arg)
{
  /* Check whether HEADER matches NAME.  */
  while (*name && (TOLOWER (*name) == TOLOWER (*header)))
    ++name, ++header;
  if (*name || *header++ != ':')
    return 0;

  header += skip_lws (header);

  return ((*procfun) (header, arg));
}

/* Helper functions for use with header_process().  */

/* Extract a long integer from HEADER and store it to CLOSURE.  If an
   error is encountered, return 0, else 1.  */
int
header_extract_number (const char *header, void *closure)
{
  const char *p = header;
  long result;

  for (result = 0; ISDIGIT (*p); p++)
    result = 10 * result + (*p - '0');

  /* Failure if no number present. */
  if (p == header)
    return 0;

  /* Skip trailing whitespace. */
  p += skip_lws (p);

  /* Indicate failure if trailing garbage is present. */
  if (*p)
    return 0;

  *(long *)closure = result;
  return 1;
}

/* Strdup HEADER, and place the pointer to CLOSURE.  */
int
header_strdup (const char *header, void *closure)
{
  *(char **)closure = xstrdup (header);
  return 1;
}

/* Write the value 1 into the integer pointed to by CLOSURE.  */
int
header_exists (const char *header, void *closure)
{
  *(int *)closure = 1;
  return 1;
}

/* Skip LWS (linear white space), if present.  Returns number of
   characters to skip.  */
int
skip_lws (const char *string)
{
  const char *p = string;

  while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
    ++p;
  return p - string;
}
