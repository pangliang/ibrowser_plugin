int base64encode(const char *in_str, int in_len, char *out_str)
{
    static unsigned char base64[] ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int curr_out_len = 0;
    int i = 0;
    unsigned char a, b, c;
    out_str[0] = '\0';
    if (in_len > 0)
    {
        while (i < in_len)
        {
            a = in_str[i];
            b = (i + 1 >= in_len) ? 0 : in_str[i + 1];
            c = (i + 2 >= in_len) ? 0 : in_str[i + 2];
            if (i + 2 < in_len)
            {
                out_str[curr_out_len++] = (base64[(a >> 2) & 0x3F]);
                out_str[curr_out_len++] = (base64[((a << 4) & 0x30) + ((b >> 4) & 0xf)]);
                out_str[curr_out_len++] = (base64[((b << 2) & 0x3c) + ((c >> 6) & 0x3)]);
                out_str[curr_out_len++] = (base64[c & 0x3F]);
            }
            else if (i + 1 < in_len)
            {
                out_str[curr_out_len++] = (base64[(a >> 2) & 0x3F]);
                out_str[curr_out_len++] = (base64[((a << 4) & 0x30) + ((b >> 4) & 0xf)]);
                out_str[curr_out_len++] = (base64[((b << 2) & 0x3c) + ((c >> 6) & 0x3)]);
                out_str[curr_out_len++] = '=';
            }
            else
            {
                out_str[curr_out_len++] = (base64[(a >> 2) & 0x3F]);
                out_str[curr_out_len++] = (base64[((a << 4) & 0x30) + ((b >> 4) & 0xf)]);
                out_str[curr_out_len++] = '=';
                out_str[curr_out_len++] = '=';
            }
            i += 3;
        }
        out_str[curr_out_len] = '\0';
    }
    return curr_out_len;
}