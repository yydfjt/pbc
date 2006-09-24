#ifndef PAIRING_H
#define PAIRING_H

#include <stdio.h>
#include "curve.h"
#include "fops.h"

struct pairing_s {
    mpz_t r; //order of G1, G2, GT
    field_t Zr; //the field Z_r
    field_ptr G1, G2, GT;

    void (*phi)(element_ptr out, element_ptr in, struct pairing_s *pairing); //isomorphism G2 --> G1
    void (*map)(element_ptr out, element_ptr in1, element_ptr in2,
	    struct pairing_s *p);
    //is_almost coddh returns 1 given (g, g^x, h, h^x) or (g, g^x, h, h^-x)
    //0 otherwise
    //order is important: a, b are from G1, c, d are from G2
    int (*is_almost_coddh)(element_ptr a, element_ptr b,
	    element_ptr c, element_ptr d,
	    struct pairing_s *p);
    //char *id;
    void (*clear_func)(struct pairing_s *);
    void *data;
};

typedef struct pairing_s pairing_t[1];
typedef struct pairing_s *pairing_ptr;

/*@manual pairing_init
TODO
*/
void pairing_init_inp_generic(pairing_t pairing, fetch_ops_t fops, void *ctx);

/*@manual pairing_init
Read in pairing parameters from array of characters ''buf'' of length ''len''
and use them to initialize ''pairing''.
*/
void pairing_init_inp_buf(pairing_t pairing, const char *buf, size_t len);

/*@manual pairing_init
Read in pairing parameters from ''stream''
and use them to initialize ''pairing''.
*/
void pairing_init_inp_str(pairing_t pairing, FILE *stream);

/*@manual pairing_init
Free the space occupied by ''pairing''. Call
whenever a <type>pairing_t</type> variable is no longer needed.
*/
void pairing_clear(pairing_t pairing);

static inline void bilinear_map(element_t out, element_t in1, element_t in2,
    pairing_t pairing) {
    pairing->map(out, in1, in2, pairing);
}

/*@manual pairing_op
Apply the bilinear map described by ''pairing''. The element ''out''
will be set to the map applied to ''in1'' and ''in2'' (in that order).
''in1'' must be in the group G1,
''in2'' must be in the group G2, and
''out'' must be in the group GT.
*/
static inline void pairing_apply(element_t out, element_t in1, element_t in2,
    pairing_t pairing) {
    pairing->map(out, in1, in2, pairing);
}

/*@manual pairing_op
Returns the length in bytes needed to represent an element of G1.
*/
static inline int pairing_length_in_bytes_G1(pairing_t pairing)
{
    return pairing->G1->fixed_length_in_bytes;
}

/*@manual pairing_op
Returns the length in bytes needed to represent the x-coordinate of
an element of G1.
*/
static inline int pairing_length_in_bytes_x_only_G1(pairing_t pairing)
{
    return pairing->G1->fixed_length_in_bytes / 2;
}

/*@manual pairing_op
Returns the length in bytes needed to represent a compressed form of
an element of G1. There is some overhead in decompressing.
*/
static inline int pairing_length_in_bytes_compressed_G1(pairing_t pairing)
{
    return pairing->G1->fixed_length_in_bytes / 2 + 1;
}

/*@manual pairing_op
Returns the length in bytes needed to represent an element of G2.
*/
static inline int pairing_length_in_bytes_G2(pairing_t pairing)
{
    return pairing->G2->fixed_length_in_bytes;
}

/*@manual pairing_op
Returns the length in bytes needed to represent a compressed form of
an element of G2. There is some overhead in decompressing.
*/
static inline int pairing_length_in_bytes_compressed_G2(pairing_t pairing)
{
    return pairing->G2->fixed_length_in_bytes / 2 + 1;
}

/*@manual pairing_op
Returns the length in bytes needed to represent the x-coordinate of
an element of G2.
*/
static inline int pairing_length_in_bytes_x_only_G2(pairing_t pairing)
{
    return pairing->G2->fixed_length_in_bytes / 2;
}

/*@manual pairing_op
Returns the length in bytes needed to represent an element of GT.
*/
static inline int pairing_length_in_bytes_GT(pairing_t pairing)
{
    return pairing->GT->fixed_length_in_bytes;
}

/*@manual pairing_op
Returns the length in bytes needed to represent an element of Zr.
*/
static inline int pairing_length_in_bytes_Zr(pairing_t pairing)
{
    return pairing->Zr->fixed_length_in_bytes;
}

int generic_is_almost_coddh(element_ptr a, element_ptr b,
	element_ptr c, element_ptr d, pairing_t pairing);

static inline int is_almost_coddh(element_t a, element_t b,
	element_t c, element_t d, pairing_t pairing) {
    return pairing->is_almost_coddh(a, b, c, d, pairing);
}

/*@manual einit.1
*/
static inline void element_init_G1(element_t e, pairing_t pairing)
{
    element_init(e, pairing->G1);
}

/*@manual einit.1
*/
static inline void element_init_G2(element_t e, pairing_t pairing)
{
    element_init(e, pairing->G2);
}

/*@manual einit.1
Initialize ''e'' to be an element of the group G1, G2 or GT of ''pairing''.
*/
static inline void element_init_GT(element_t e, pairing_t pairing)
{
    element_init(e, pairing->GT);
}

/*@manual einit.1
Initialize ''e'' to be an element of the ring Z_r of ''pairing''.
r is the order of the groups G1, G2 and GT that are involved in the pairing.
*/
static inline void element_init_Zr(element_t e, pairing_t pairing)
{
    element_init(e, pairing->Zr);
}
#endif //PAIRING_H
