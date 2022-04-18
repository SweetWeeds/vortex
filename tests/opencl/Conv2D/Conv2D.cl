__kernel void Conv2D_3x3(
    __global int *out_fmap,
    __global int *in_fmap,
    __global int *weight,
    int in_ch, int in_wp, int in_hp, int P
) {
    const int out_ch = get_global_size(0);
    const int out_h  = get_global_size(1);
    const int out_w  = get_global_size(2);

    const int o_ch   = get_global_id(0);
    const int o_h    = get_global_id(1);
    const int o_w    = get_global_id(2);

    int i_h, i_w;

    int sum = 0;
    for (int i_ch = 0; i_ch < in_ch; i_ch++) {
        for (int k_r = 0; k_r < 3; k_r++) {
            i_h = o_h + k_r;
            for (int k_c = 0; k_c < 3; k_c++) {
                i_w = o_w + k_c;
                sum += in_fmap[i_ch*in_hp*in_wp + i_h*in_wp + i_w] * weight[o_ch*in_ch*3*3 + i_ch*3*3 + k_r*3 + k_c];
            }
        }
    }
    out_fmap[o_ch*out_h*out_w + o_h*out_w + o_w] = sum;
}