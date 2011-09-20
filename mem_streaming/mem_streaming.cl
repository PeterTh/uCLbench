
__kernel __attribute__((reqd_work_group_size (localRange,1,1))) void local_mem(
                    __global float* output
#ifdef LARG
					,__local dtype* s1, __local dtype* s2)	
{
#else
    ) {
    __local dtype s1[ELEMENTS];
    __local dtype s2[2*ELEMENTS];
#endif

     int id = get_local_id(0);
     int gid = get_global_id(0);
     int step = get_local_size(0);

    for(int idx = id; idx < ELEMENTS; idx += step)
    {
#if VEC == 1
        s2[idx].s0 = 0.5f;
        s2[idx].s1 = 0.5f;
        s1[idx].s0 = 0.0f;
        s1[idx].s1 = 0.0f;
#elif VEC == 2
        s2[idx].s0 = 0.5f;
        s2[idx].s1 = 0.5f;
        s2[idx].s2 = 0.5f;
        s2[idx].s3 = 0.5f;
        s1[idx].s0 = 0.0f;
        s1[idx].s1 = 0.0f;
        s1[idx].s2 = 0.0f;
        s1[idx].s3 = 0.0f;
#elif VEC == 3
        s2[idx].s0 = 0.5f;
        s2[idx].s1 = 0.5f;
        s2[idx].s2 = 0.5f;
        s2[idx].s3 = 0.5f;
        s2[idx].s4 = 0.5f;
        s2[idx].s5 = 0.5f;
        s2[idx].s6 = 0.5f;
        s2[idx].s7 = 0.5f;
        s1[idx].s0 = 0.0f;
        s1[idx].s1 = 0.0f;
        s1[idx].s2 = 0.0f;
        s1[idx].s3 = 0.0f;
        s1[idx].s4 = 0.0f;
        s1[idx].s5 = 0.0f;
        s1[idx].s6 = 0.0f;
        s1[idx].s7 = 0.0f;
#elif VEC == 4
        s2[idx].s0 = 0.5f;
        s2[idx].s1 = 0.5f;
        s2[idx].s2 = 0.5f;
        s2[idx].s3 = 0.5f;
        s2[idx].s4 = 0.5f;
        s2[idx].s5 = 0.5f;
        s2[idx].s6 = 0.5f;
        s2[idx].s7 = 0.5f;
        s2[idx].s8 = 0.5f;
        s2[idx].s9 = 0.5f;
        s2[idx].sa = 0.5f;
        s2[idx].sb = 0.5f;
        s2[idx].sc = 0.5f;
        s2[idx].sd = 0.5f;
        s2[idx].se = 0.5f;
        s2[idx].sf = 0.5f;
        s1[idx].s0 = 0.0f;
        s1[idx].s1 = 0.0f;
        s1[idx].s2 = 0.0f;
        s1[idx].s3 = 0.0f;
        s1[idx].s4 = 0.0f;
        s1[idx].s5 = 0.0f;
        s1[idx].s6 = 0.0f;
        s1[idx].s7 = 0.0f;
        s1[idx].s8 = 0.0f;
        s1[idx].s9 = 0.0f;
        s1[idx].sa = 0.0f;
        s1[idx].sb = 0.0f;
        s1[idx].sc = 0.0f;
        s1[idx].sd = 0.0f;
        s1[idx].se = 0.0f;
        s1[idx].sf = 0.0f;
#else
        s2[idx] = 0.5f;
        s1[idx] = 0.0f;
#endif
    }
    
    for(int idx = id+ELEMENTS; idx < ELEMENTS*2; idx += step)
    {
#if VEC == 1
        s2[idx].s0 = 0.5f;
        s2[idx].s1 = 0.5f;
#elif VEC == 2
        s2[idx].s0 = 0.5f;
        s2[idx].s1 = 0.5f;
        s2[idx].s2 = 0.5f;
        s2[idx].s3 = 0.5f;
#elif VEC == 3
        s2[idx].s0 = 0.5f;
        s2[idx].s1 = 0.5f;
        s2[idx].s2 = 0.5f;
        s2[idx].s3 = 0.5f;
        s2[idx].s4 = 0.5f;
        s2[idx].s5 = 0.5f;
        s2[idx].s6 = 0.5f;
        s2[idx].s7 = 0.5f;
#elif VEC == 4
        s2[idx].s0 = 0.5f;
        s2[idx].s1 = 0.5f;
        s2[idx].s2 = 0.5f;
        s2[idx].s3 = 0.5f;
        s2[idx].s4 = 0.5f;
        s2[idx].s5 = 0.5f;
        s2[idx].s6 = 0.5f;
        s2[idx].s7 = 0.5f;
        s2[idx].s8 = 0.5f;
        s2[idx].s9 = 0.5f;
        s2[idx].sa = 0.5f;
        s2[idx].sb = 0.5f;
        s2[idx].sc = 0.5f;
        s2[idx].sd = 0.5f;
        s2[idx].se = 0.5f;
        s2[idx].sf = 0.5f;
#else
        s2[idx] = 0.5f;
#endif
    }

    barrier(CLK_LOCAL_MEM_FENCE);


    for(int idx = id; idx < ELEMENTS; idx += step)
    {
        for(int i = 0; i < 100; ++i)
        {
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
            s1[idx] = s1[idx] + s2[idx] + s2[2*idx];
        }
    }

    for( int idx = id; idx < ELEMENTS; idx += step)
#if VEC == 1
    output[gid] += s1[idx].s0 + s1[idx].s1;
#elif VEC == 2
    output[gid] += s1[idx].s0 + s1[idx].s1 + s1[idx].s2 + s1[idx].s3;
#elif VEC == 3
    output[gid] += s1[idx].s0 + s1[idx].s1 + s1[idx].s2 + s1[idx].s3 + 
                   s1[idx].s4 + s1[idx].s5 + s1[idx].s6 + s1[idx].s7;
#elif VEC == 4
    output[gid] += s1[idx].s0 + s1[idx].s1 + s1[idx].s2 + s1[idx].s3 + 
                   s1[idx].s4 + s1[idx].s5 + s1[idx].s6 + s1[idx].s7 +
                   s1[idx].s8 + s1[idx].s9 + s1[idx].sa + s1[idx].sb + 
                   s1[idx].sc + s1[idx].sd + s1[idx].se + s1[idx].sf;
#else
    output[gid] += s1[idx];
#endif
}

__kernel __attribute__((reqd_work_group_size (localRange,1,1))) void global_mem(
                    __global float* output,
					__global dtype* g1,
					__global dtype* g2)	
{
     int id = get_local_id(0);
     int gid = get_global_id(0);
     int step = get_local_size(0);

    for(int idx = id; idx < ELEMENTS; idx += step)
    {
#if VEC == 1
        g2[idx].s0 = 0.5f;
        g2[idx].s1 = 0.5f;
        g1[idx].s0 = 0.0f;
        g1[idx].s1 = 0.0f;
#elif VEC == 2
        g2[idx].s0 = 0.5f;
        g2[idx].s1 = 0.5f;
        g2[idx].s2 = 0.5f;
        g2[idx].s3 = 0.5f;
        g1[idx].s0 = 0.0f;
        g1[idx].s1 = 0.0f;
        g1[idx].s2 = 0.0f;
        g1[idx].s3 = 0.0f;
#elif VEC == 3
        g2[idx].s0 = 0.5f;
        g2[idx].s1 = 0.5f;
        g2[idx].s2 = 0.5f;
        g2[idx].s3 = 0.5f;
        g2[idx].s4 = 0.5f;
        g2[idx].s5 = 0.5f;
        g2[idx].s6 = 0.5f;
        g2[idx].s7 = 0.5f;
        g1[idx].s0 = 0.0f;
        g1[idx].s1 = 0.0f;
        g1[idx].s2 = 0.0f;
        g1[idx].s3 = 0.0f;
        g1[idx].s4 = 0.0f;
        g1[idx].s5 = 0.0f;
        g1[idx].s6 = 0.0f;
        g1[idx].s7 = 0.0f;
#elif VEC == 4
        g2[idx].s0 = 0.5f;
        g2[idx].s1 = 0.5f;
        g2[idx].s2 = 0.5f;
        g2[idx].s3 = 0.5f;
        g2[idx].s4 = 0.5f;
        g2[idx].s5 = 0.5f;
        g2[idx].s6 = 0.5f;
        g2[idx].s7 = 0.5f;
        g2[idx].s8 = 0.5f;
        g2[idx].s9 = 0.5f;
        g2[idx].sa = 0.5f;
        g2[idx].sb = 0.5f;
        g2[idx].sc = 0.5f;
        g2[idx].sd = 0.5f;
        g2[idx].se = 0.5f;
        g2[idx].sf = 0.5f;
        g1[idx].s0 = 0.0f;
        g1[idx].s1 = 0.0f;
        g1[idx].s2 = 0.0f;
        g1[idx].s3 = 0.0f;
        g1[idx].s4 = 0.0f;
        g1[idx].s5 = 0.0f;
        g1[idx].s6 = 0.0f;
        g1[idx].s7 = 0.0f;
        g1[idx].s8 = 0.0f;
        g1[idx].s9 = 0.0f;
        g1[idx].sa = 0.0f;
        g1[idx].sb = 0.0f;
        g1[idx].sc = 0.0f;
        g1[idx].sd = 0.0f;
        g1[idx].se = 0.0f;
        g1[idx].sf = 0.0f;
#else
        g2[idx] = 0.5f;
        g1[idx] = 0.0f;
#endif
    }
    
    for(int idx = id+ELEMENTS; idx < ELEMENTS*2; idx += step)
    {
#if VEC == 1
        g2[idx].s0 = 0.5f;
        g2[idx].s1 = 0.5f;
#elif VEC == 2
        g2[idx].s0 = 0.5f;
        g2[idx].s1 = 0.5f;
        g2[idx].s2 = 0.5f;
        g2[idx].s3 = 0.5f;
#elif VEC == 3
        g2[idx].s0 = 0.5f;
        g2[idx].s1 = 0.5f;
        g2[idx].s2 = 0.5f;
        g2[idx].s3 = 0.5f;
        g2[idx].s4 = 0.5f;
        g2[idx].s5 = 0.5f;
        g2[idx].s6 = 0.5f;
        g2[idx].s7 = 0.5f;
#elif VEC == 4
        g2[idx].s0 = 0.5f;
        g2[idx].s1 = 0.5f;
        g2[idx].s2 = 0.5f;
        g2[idx].s3 = 0.5f;
        g2[idx].s4 = 0.5f;
        g2[idx].s5 = 0.5f;
        g2[idx].s6 = 0.5f;
        g2[idx].s7 = 0.5f;
        g2[idx].s8 = 0.5f;
        g2[idx].s9 = 0.5f;
        g2[idx].sa = 0.5f;
        g2[idx].sb = 0.5f;
        g2[idx].sc = 0.5f;
        g2[idx].sd = 0.5f;
        g2[idx].se = 0.5f;
        g2[idx].sf = 0.5f;
#else
        g2[idx] = 0.5f;
#endif
    }

    barrier(CLK_LOCAL_MEM_FENCE);


    for(int idx = id; idx < ELEMENTS; idx += step)
    {
        for(int i = 0; i < 100; ++i)
        {
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
            g1[idx] = g1[idx] + g2[idx] + g2[2*idx];
        }
    }

    for( int idx = id; idx < ELEMENTS; idx += step)
#if VEC == 1
    output[gid] += g1[idx].s0 + g1[idx].s1;
#elif VEC == 2
    output[gid] += g1[idx].s0 + g1[idx].s1 + g1[idx].s2 + g1[idx].s3;
#elif VEC == 3
    output[gid] += g1[idx].s0 + g1[idx].s1 + g1[idx].s2 + g1[idx].s3 + 
                   g1[idx].s4 + g1[idx].s5 + g1[idx].s6 + g1[idx].s7;
#elif VEC == 4
    output[gid] += g1[idx].s0 + g1[idx].s1 + g1[idx].s2 + g1[idx].s3 + 
                   g1[idx].s4 + g1[idx].s5 + g1[idx].s6 + g1[idx].s7 +
                   g1[idx].s8 + g1[idx].s9 + g1[idx].sa + g1[idx].sb + 
                   g1[idx].sc + g1[idx].sd + g1[idx].se + g1[idx].sf;
#else
    output[gid] += g1[idx];
#endif
}


__kernel __attribute__((reqd_work_group_size (localRange,1,1))) void private_mem(
                    __global float* output)	
{
    int id = get_local_id(0);
    int gid = get_global_id(0);
//    int num = NUM;
    __private dtype r1[ELEMENTS];
    __private dtype r2[2*ELEMENTS];

    for(int idx = 0; idx < ELEMENTS; ++idx)
    {
#if VEC == 1
        r2[idx].s0 = 0.5f;
        r2[idx].s1 = 0.5f;
        r1[idx].s0 = 0.0f;
        r1[idx].s1 = 0.0f;
#elif VEC == 2
        r2[idx].s0 = 0.5f;
        r2[idx].s1 = 0.5f;
        r2[idx].s2 = 0.5f;
        r2[idx].s3 = 0.5f;
        r1[idx].s0 = 0.0f;
        r1[idx].s1 = 0.0f;
        r1[idx].s2 = 0.0f;
        r1[idx].s3 = 0.0f;
#elif VEC == 3
        r2[idx].s0 = 0.5f;
        r2[idx].s1 = 0.5f;
        r2[idx].s2 = 0.5f;
        r2[idx].s3 = 0.5f;
        r2[idx].s4 = 0.5f;
        r2[idx].s5 = 0.5f;
        r2[idx].s6 = 0.5f;
        r2[idx].s7 = 0.5f;
        r1[idx].s0 = 0.0f;
        r1[idx].s1 = 0.0f;
        r1[idx].s2 = 0.0f;
        r1[idx].s3 = 0.0f;
        r1[idx].s4 = 0.0f;
        r1[idx].s5 = 0.0f;
        r1[idx].s6 = 0.0f;
        r1[idx].s7 = 0.0f;
#elif VEC == 4
        r2[idx].s0 = 0.5f;
        r2[idx].s1 = 0.5f;
        r2[idx].s2 = 0.5f;
        r2[idx].s3 = 0.5f;
        r2[idx].s4 = 0.5f;
        r2[idx].s5 = 0.5f;
        r2[idx].s6 = 0.5f;
        r2[idx].s7 = 0.5f;
        r2[idx].s8 = 0.5f;
        r2[idx].s9 = 0.5f;
        r2[idx].sa = 0.5f;
        r2[idx].sb = 0.5f;
        r2[idx].sc = 0.5f;
        r2[idx].sd = 0.5f;
        r2[idx].se = 0.5f;
        r2[idx].sf = 0.5f;
        r1[idx].s0 = 0.0f;
        r1[idx].s1 = 0.0f;
        r1[idx].s2 = 0.0f;
        r1[idx].s3 = 0.0f;
        r1[idx].s4 = 0.0f;
        r1[idx].s5 = 0.0f;
        r1[idx].s6 = 0.0f;
        r1[idx].s7 = 0.0f;
        r1[idx].s8 = 0.0f;
        r1[idx].s9 = 0.0f;
        r1[idx].sa = 0.0f;
        r1[idx].sb = 0.0f;
        r1[idx].sc = 0.0f;
        r1[idx].sd = 0.0f;
        r1[idx].se = 0.0f;
        r1[idx].sf = 0.0f;
#else
        r2[idx] = 0.5f;
        r1[idx] = 0.0f;
#endif
    }
    
    for(int idx = ELEMENTS; idx < ELEMENTS*2; ++idx)
    {
#if VEC == 1
        r2[idx].s0 = 0.5f;
        r2[idx].s1 = 0.5f;
#elif VEC == 2
        r2[idx].s0 = 0.5f;
        r2[idx].s1 = 0.5f;
        r2[idx].s2 = 0.5f;
        r2[idx].s3 = 0.5f;
#elif VEC == 3
        r2[idx].s0 = 0.5f;
        r2[idx].s1 = 0.5f;
        r2[idx].s2 = 0.5f;
        r2[idx].s3 = 0.5f;
        r2[idx].s4 = 0.5f;
        r2[idx].s5 = 0.5f;
        r2[idx].s6 = 0.5f;
        r2[idx].s7 = 0.5f;
#elif VEC == 4
        r2[idx].s0 = 0.5f;
        r2[idx].s1 = 0.5f;
        r2[idx].s2 = 0.5f;
        r2[idx].s3 = 0.5f;
        r2[idx].s4 = 0.5f;
        r2[idx].s5 = 0.5f;
        r2[idx].s6 = 0.5f;
        r2[idx].s7 = 0.5f;
        r2[idx].s8 = 0.5f;
        r2[idx].s9 = 0.5f;
        r2[idx].sa = 0.5f;
        r2[idx].sb = 0.5f;
        r2[idx].sc = 0.5f;
        r2[idx].sd = 0.5f;
        r2[idx].se = 0.5f;
        r2[idx].sf = 0.5f;
#else
        r2[idx] = 0.5f;
#endif
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    for(int idx = 0; idx < ELEMENTS; ++idx)
    {
        for(int i = 0; i < 100; ++i)
        {
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
            r1[idx] = r1[idx] + r2[idx] + r2[2*idx];
        }
    }
	
    for( int idx = 0; idx < ELEMENTS; ++idx)
#if VEC == 1
    output[gid] += r1[idx].s0 + r1[idx].s1;
#elif VEC == 2
    output[gid] += r1[idx].s0 + r1[idx].s1 + r1[idx].s2 + r1[idx].s3;
#elif VEC == 3
    output[gid] += r1[idx].s0 + r1[idx].s1 + r1[idx].s2 + r1[idx].s3 + 
                   r1[idx].s4 + r1[idx].s5 + r1[idx].s6 + r1[idx].s7;
#elif VEC == 4
    output[gid] += r1[idx].s0 + r1[idx].s1 + r1[idx].s2 + r1[idx].s3 + 
                   r1[idx].s4 + r1[idx].s5 + r1[idx].s6 + r1[idx].s7 +
                   r1[idx].s8 + r1[idx].s9 + r1[idx].sa + r1[idx].sb + 
                   r1[idx].sc + r1[idx].sd + r1[idx].se + r1[idx].sf;
#else
    output[gid] += r1[idx];
#endif
}
