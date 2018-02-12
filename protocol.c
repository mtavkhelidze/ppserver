/**
 * Copyright (c) 2018 Misha Tavkhelidze <misha.tavkhelidze@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <string.h>
#include <errno.h>

#include "protocol.h"
#include "config.h"

const char *proto_response(const char *req, size_t *rlen)
{
    if (strncmp(req, PP_CLIENT_REQ, (size_t) rlen) == 0) {
        *rlen = strlen(PP_SERVER_RES);
        return PP_SERVER_RES;
    }
    errno = EINVAL;
    return NULL;
}

const char *proto_hup(size_t *rlen)
{
    *rlen = strlen(PP_SERVER_HUP);
    return PP_SERVER_HUP;
}

const char *proto_request(size_t *rlen)
{
    *rlen = strlen(PP_CLIENT_REQ);
    return PP_CLIENT_REQ;
}
