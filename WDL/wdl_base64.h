#ifndef _WDL_BASE64_H_
#define _WDL_BASE64_H_

#ifndef WDL_BASE64_FUNCDECL
#define WDL_BASE64_FUNCDECL static
#endif

#ifdef OutputDebugString
#undef OutputDebugString
#define OutputDebugString OutputDebugStringA
#endif

static void DBGMSG2(const char* format, ...)
{
  char buf[4096], * p = buf;
  va_list args;
  int     n;

  va_start(args, format);
  n = _vsnprintf(p, sizeof buf - 3, format, args); // buf-3 is room for CR/LF/NUL
  va_end(args);

  p += (n < 0) ? sizeof buf - 3 : n;

  while (p > buf && isspace(p[-1]))
    *--p = '\0';

  *p++ = '\r';
  *p++ = '\n';
  *p = '\0';

  OutputDebugString(buf);
}


/*
typedef unsigned char BYTE;

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";


static inline bool is_base64(BYTE c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

WDL_BASE64_FUNCDECL void wdl_base64encode(const unsigned char* buf, char* out, int bufLen)
{
  DBGMSG2("in %s\n", buf);
  std::string ret;
  int i = 0;
  int j = 0;
  BYTE char_array_3[3];
  BYTE char_array_4[4];

  while (bufLen--) {
    char_array_3[i++] = *(buf++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (i = 0; (i < 4); i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for (j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while ((i++ < 3))
      ret += '=';
  }
  memcpy(out, &ret, sizeof ret);
  for (size_t n = 0; n < sizeof out; ++n)
    putchar(out[n]);
  DBGMSG2("ret %s; out %s\n", ret, out);
}

WDL_BASE64_FUNCDECL int wdl_base64decode(const char* src, unsigned char* dest, int destsize)
{
  DBGMSG2("destsize %d\n", destsize);
  if (destsize <= 0) return 0;
  if (!src || !dest)
    return 0;

  while (destsize >= 0)
  {
    dest[destsize] = src[destsize];
    destsize -= 1;
  }

  static const char *tab = // poolable string
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x3e\xff\xff\xff\x3f"
    "\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\xff\xff\xff\xff\xff\xff"
    "\xff\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e"
    "\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\xff\xff\xff\xff\xff"
    "\xff\x1a\x1b\x1c\x1d\x1e\x1f\x20\x21\x22\x23\x24\x25\x26\x27\x28"
    "\x29\x2a\x2b\x2c\x2d\x2e\x2f\x30\x31\x32\x33\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff";


  int accum=0, nbits=0, wpos=0;

  if (destsize <= 0) return 0;
  if (!src || !dest) return 0;

  for (;;)
  {
    const int v=(int)tab[*(unsigned char *)src++];
    if (v<0) return wpos;

    accum += v;
    nbits += 6;

    if (nbits >= 8)
    {
      nbits-=8;
      dest[wpos] = (accum>>nbits)&0xff;
      if (++wpos >= destsize) return wpos;
    }
    accum <<= 6;
  }
  DBGMSG2("dest %s\n", dest);
}
*/

static const char wdl_base64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
WDL_BASE64_FUNCDECL void wdl_base64encode(const unsigned char *in, char *out, int len)
{
  DBGMSG2("len %d; in %s; size in %d; size out %d\n", len, in, sizeof in, sizeof out);
  if (!in || !out) {
    DBGMSG2("no in or out");
    return;
  }

  while (len >= 6)
  {
    const int accum = (in[0] << 16) + (in[1] << 8) + in[2];
    const int accum2 = (in[3] << 16) + (in[4] << 8) + in[5];
    out[0] = wdl_base64_alphabet[(accum >> 18) & 0x3F];
    out[1] = wdl_base64_alphabet[(accum >> 12) & 0x3F];
    out[2] = wdl_base64_alphabet[(accum >> 6) & 0x3F];
    out[3] = wdl_base64_alphabet[accum & 0x3F];
    out[4] = wdl_base64_alphabet[(accum2 >> 18) & 0x3F];
    out[5] = wdl_base64_alphabet[(accum2 >> 12) & 0x3F];
    out[6] = wdl_base64_alphabet[(accum2 >> 6) & 0x3F];
    out[7] = wdl_base64_alphabet[accum2 & 0x3F];
    out+=8;
    in+=6;
    len-=6;
  }

  if (len >= 3)
  {
    const int accum = (in[0]<<16)|(in[1]<<8)|in[2];
    out[0] = wdl_base64_alphabet[(accum >> 18) & 0x3F];
    out[1] = wdl_base64_alphabet[(accum >> 12) & 0x3F];
    out[2] = wdl_base64_alphabet[(accum >> 6) & 0x3F];
    out[3] = wdl_base64_alphabet[accum & 0x3F];    
    in+=3;
    len-=3;
    out+=4;
  }

  if (len>0)
  {
    if (len == 2)
    {
      const int accum = (in[0] << 8) | in[1];
      out[0] = wdl_base64_alphabet[(accum >> 10) & 0x3F];
      out[1] = wdl_base64_alphabet[(accum >> 4) & 0x3F];
      out[2] = wdl_base64_alphabet[(accum & 0xF)<<2];
    }
    else
    {
      const int accum = in[0];
      out[0] = wdl_base64_alphabet[(accum >> 2) & 0x3F];
      out[1] = wdl_base64_alphabet[(accum & 0x3)<<4];
      out[2]='=';  
    }
    out[3]='=';  
    out+=4;
  }

  //out[0]=0;
  DBGMSG2("done! out %s\n", out);
}

WDL_BASE64_FUNCDECL int wdl_base64decode(const char *src, unsigned char *dest, int destsize)
{
  DBGMSG2("src %s; destsize %d\n", src, destsize);
  if (destsize <= 0) return 0;
  if (!src || !dest)
    return 0;

  while (destsize >= 0)
  {
    dest[destsize] = src[destsize];
    destsize -= 1;
  }

  static const char *tab = // poolable string
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x3e\xff\xff\xff\x3f"
    "\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\xff\xff\xff\xff\xff\xff"
    "\xff\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e"
    "\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\xff\xff\xff\xff\xff"
    "\xff\x1a\x1b\x1c\x1d\x1e\x1f\x20\x21\x22\x23\x24\x25\x26\x27\x28"
    "\x29\x2a\x2b\x2c\x2d\x2e\x2f\x30\x31\x32\x33\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"
    "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff";


  int accum=0, nbits=0, wpos=0;

  if (destsize <= 0) return 0;
  if (!src || !dest) return 0;

  for (;;)
  {
    const int v=(int)tab[*(unsigned char *)src++];
    if (v<0) return wpos;

    accum += v;
    nbits += 6;   

    if (nbits >= 8)
    {
      nbits-=8;
      dest[wpos] = (accum>>nbits)&0xff;
      if (++wpos >= destsize) return wpos;
    }
    accum <<= 6;
  }

  DBGMSG2("dest %s\n", dest);
}

#endif
