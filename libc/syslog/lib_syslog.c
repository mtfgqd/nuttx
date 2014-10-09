/****************************************************************************
 * libc/syslog/lib_syslog.c
 *
 *   Copyright (C) 2007-2009, 2011-2014 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdio.h>
#include <syslog.h>

#include <nuttx/streams.h>

#include "syslog/syslog.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Type Declarations
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Global Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Global Constant Data
 ****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/

/****************************************************************************
 * Private Constant Data
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: vsyslog_internal
 *
 * Description:
 *   This is the internal implementation of vsyslog (see the description of
 *   syslog and vsyslog below)
 *
 ****************************************************************************/

static inline int vsyslog_internal(FAR const char *fmt, va_list ap)
{
#if defined(CONFIG_SYSLOG)

  struct lib_outstream_s stream;

  /* Wrap the low-level output in a stream object and let lib_vsprintf
   * do the work.
   */

  lib_syslogstream((FAR struct lib_outstream_s *)&stream);
  return lib_vsprintf((FAR struct lib_outstream_s *)&stream, fmt, ap);

#elif CONFIG_NFILE_DESCRIPTORS > 0

  struct lib_rawoutstream_s rawoutstream;

  /* Wrap the stdout in a stream object and let lib_vsprintf
   * do the work.
   */

  lib_rawoutstream(&rawoutstream, 1);
  return lib_vsprintf(&rawoutstream.public, fmt, ap);

#elif defined(CONFIG_ARCH_LOWPUTC)

  struct lib_outstream_s stream;

  /* Wrap the low-level output in a stream object and let lib_vsprintf
   * do the work.
   */

  lib_lowoutstream((FAR struct lib_outstream_s *)&stream);
  return lib_vsprintf((FAR struct lib_outstream_s *)&stream, fmt, ap);

#else
  return 0;
#endif
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: vsyslog
 *
 * Description:
 *   The function vsyslog() performs the same task as syslog() with the
 *   difference that it takes a set of arguments which have been obtained
 *   using the stdarg variable argument list macros.
 *
 ****************************************************************************/

int vsyslog(int priority, FAR const char *fmt, va_list ap)
{
  int ret = 0;

  /* Check if this priority is enabled */

  if ((g_syslog_mask & LOG_MASK(priority)) != 0)
    {
      /* Yes.. let vsylog_internal to the deed */

      ret = vsyslog_internal(fmt, ap);
    }

  return ret;
}

/****************************************************************************
 * Name: syslog
 *
 * Description:
 *   syslog() generates a log message. The priority argument is formed by
 *   ORing the facility and the level values (see include/syslog.h). The
 *   remaining arguments are a format, as in printf and any arguments to the
 *   format.
 *
 *   The NuttX implementation does not support any special formatting
 *   characters beyond those supported by printf.
 *
 ****************************************************************************/

int syslog(int priority, FAR const char *fmt, ...)
{
  va_list ap;
  int ret;

  /* Let vsyslog do the work */

  va_start(ap, fmt);
  ret = vsyslog(priority, fmt, ap);
  va_end(ap);

  return ret;
}
