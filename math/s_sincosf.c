/*
 * s_sincosf.c - single precision sine and cosine functions
 *
 * Copyright (c) 2009-2015, Arm Limited.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Source: my own head, and Remez-generated polynomial approximations.
 */

#include "rredf.h"
#include <fenv.h>
#include "math_private.h"
#include <math.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef COSINE
#define FUNCNAME ARM__sinf
#define SOFTFP_FUNCNAME __softfp_sinf
#define DO_SIN (!(q & 1))
#define NEGATE_SIN ((q & 2))
#define NEGATE_COS ((q & 2))
#define TRIVIAL_RESULT(x) FLOAT_CHECKDENORM(x)
#define ERR_INF MATHERR_SINF_INF
#else
#define FUNCNAME ARM__cosf
#define SOFTFP_FUNCNAME __softfp_cosf
#define DO_SIN (q & 1)
#define NEGATE_SIN (!(q & 2))
#define NEGATE_COS ((q & 2))
#define TRIVIAL_RESULT(x) 1.0f
#define ERR_INF MATHERR_COSF_INF
#endif

float FUNCNAME(float x)
{
    int q;

    /*
     * Range-reduce x to the range [-pi/4,pi/4].
     */
    {
        /*
         * I enclose the call to ARM__mathlib_rredf in braces so that
         * the address-taken-ness of qq does not propagate
         * throughout the rest of the function, for what that might
         * be worth.
         */
        int qq;
        x = ARM__mathlib_rredf(x, &qq);
        q = qq;
    }
    if (__builtin_expect(q < 0, 0)) { /* this signals tiny, inf, or NaN */
        unsigned k = fai(x) << 1;
        if (k < 0xFF000000)            /* tiny */
            return TRIVIAL_RESULT(x);
        else if (k == 0xFF000000)      /* inf */
            return ERR_INF(x);
        else                           /* NaN */
            return FLOAT_INFNAN(x);
    }

    /*
     * Depending on the quadrant we were in, we may have to compute
     * a sine-like function (f(0)=0) or a cosine-like one (f(0)=1),
     * and we may have to negate it.
     */
    if (DO_SIN) {
        float x2 = x*x;
        /*
         * Coefficients generated by the command

./auxiliary/remez.jl --variable=x2 --suffix=f -- '0' 'atan(BigFloat(1))^2' 2 0 'x==0 ? -1/BigFloat(6) : (x->(sin(x)-x)/x^3)(sqrt(x))' 'sqrt(x^3)'

         */
        x += x * (x2 * (
                      -1.666665066929417292436220415142244613956015227491999719404711781344783392564922e-01f+x2*(8.331978663157089651408875887703995477889496917296385733254577121461421466427772e-03f+x2*(-1.949563623766929906511886482584265500187314705496861877317774185883215997931494e-04f))
                      ));
        if (NEGATE_SIN)
            x = -x;
        return x;
    } else {
        float x2 = x*x;
        /*
         * Coefficients generated by the command

./auxiliary/remez.jl --variable=x2 --suffix=f -- '0' 'atan(BigFloat(1))^2' 2 0 'x==0 ? -1/BigFloat(6) : (x->(cos(x)-1)/x^2)(sqrt(x))' 'x'

         */
        x = 1.0f + x2*(
            -4.999989478137016757327030935768632852012781143541026304540837816323349768666875e-01f+x2*(4.165629457842617238353362092016541041535652603456375154392942188742496860024377e-02f+x2*(-1.35978231111049428748566568960423202948250988565693107969571193763372093404347e-03f))
            );
        if (NEGATE_COS)
            x = -x;
        return x;
    }
}


#ifdef __cplusplus
} /* end of extern "C" */
#endif /* __cplusplus */

/* end of sincosf.c */
