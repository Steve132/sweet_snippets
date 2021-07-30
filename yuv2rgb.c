/* R = (298*Y + 409*Cr + 128)/256,
          G = (298*Y - 100*Cb - 208*Cr + 128)/256,
          B = (298*Y + 516*Cb + 128)/256; */
static inline void yuv2rgb(uint8_t y,uint8_t u,uint8_t v,uint8_t* r,uint8_t* g,uint8_t* b)
{
        //https://www.fourcc.org/fccyvrgb.php fixed point based on 2^16
        //JFIF version instead?
    int32_t yv=(((int32_t)y)-16) << 16;
    int32_t uv=((int32_t)u)-128;
    int32_t vv=((int32_t)v)-128;
    int32_t ov;
    
    ov=(yv+91947*vv)>>16;
    ov = (ov < 0 ? 0 : ov); ov = (ov > 255 ? 255 : ov);
    *r = ov;
    
    ov=(yv-22544*uv-46792*vv)>>16;
    ov = (ov < 0 ? 0 : ov); ov = (ov > 255 ? 255 : ov);
    *g = ov;
    
    ov=(yv+115998*uv)>>16;
    ov = (ov < 0 ? 0 : ov); ov = (ov > 255 ? 255 : ov);
    *b = ov;
}
