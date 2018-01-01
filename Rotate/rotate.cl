__kernel void rotate_kernel(__global char* src_data,__global char* dest_data, int W, int H, float sinTheta, float cosTheta)
{
    const int ix = get_global_id(0);
    const int iy = get_global_id(1);
    const int ic = get_global_id(2);
    int xc = W/2;
    int yc = H/2;
    int xpos =  ( ix-xc)*cosTheta - (iy-yc)*sinTheta+xc;
    int ypos =  (ix-xc)*sinTheta + ( iy-yc)*cosTheta+yc;
    if ((xpos>=0) && (xpos< W)    && (ypos>=0) && (ypos< H))    //边界检测
    {
        dest_data[(ypos*W+xpos)*3 + ic]= src_data[(iy*W+ix)*3 + ic];
    }
}

__kernel void scale_kernel(__global char* src_data,__global char* dst_data, int srcW, int srcH,int dstW,int dstH, float scale)
{
    const int ix = get_global_id(0);
    const int iy = get_global_id(1);
    const int ic = get_global_id(2);
    const float srcX = ix/scale;
    const float srcY = iy/scale;
    int x0 = srcX;
    int x1 = srcX + 1;
    int y0 = srcY ;
    int y1 = srcY + 1;
    if(x0 >0 && x1 <srcW && y0 > 0 && y1 < srcH)
    {
        float ax = srcX - x1;
        float ay = srcY - y1;
        int line0 = y0*srcW;
        int line1 = line0 + srcW;
        dst_data[(ix+iy*dstW)*3 + ic] = ((ax + ay)*src_data[(line0 + x0) * 3 + ic] + (1-ax + ay)*src_data[(line0 + x1)*3+ic] 
        + (ax + 1- ay)*src_data[(line1 + x0)*3 + ic] + (1-ax + 1- ay)*src_data[(line1 + x1)*3 + ic])/4;
    }
}

