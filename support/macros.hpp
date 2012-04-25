/*
 * Copyright (c) 2010, Erik Lindroos <gliptic@gmail.com>
 * This software is released under the The BSD-2-Clause License:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UUID_DFBC6E7713BD45652D22AB8D4A57781B
#define UUID_DFBC6E7713BD45652D22AB8D4A57781B

#define FOREACH(t_, i_, c_) for(t_::iterator i_ = (c_).begin(); i_ != (c_).end(); ++i_)
#define CONST_FOREACH(t_, i_, c_) for(t_::const_iterator i_ = (c_).begin(); i_ != (c_).end(); ++i_)
#define REVERSE_FOREACH(t_, i_, c_) for(t_::reverse_iterator i_ = (c_).rbegin(); i_ != (c_).rend(); ++i_)
#define FOREACH_DELETE(t_, v_, c_) for(t_::iterator v_ = (c_).begin(), n_; v_ != (c_).end() && (n_ = v_, ++n_, true); v_ = n_)

// Borrowed from boost
#define GVL_CONCAT(a, b) GVL_CONCAT_I(a, b)
#define GVL_CONCAT_I(a, b) GVL_CONCAT_II(a ## b)
#define GVL_CONCAT_II(res) res

#endif // UUID_DFBC6E7713BD45652D22AB8D4A57781B
