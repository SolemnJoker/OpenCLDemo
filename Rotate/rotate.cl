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