/* A packrat parser generated by PackCC 1.6.0 */

#ifndef PCC_INCLUDED__HOME_SSZCZYRB_REPOS_CPPTEST_SRC_CALC_H
#define PCC_INCLUDED__HOME_SSZCZYRB_REPOS_CPPTEST_SRC_CALC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct calc_context_tag calc_context_t;

calc_context_t *calc_create(void *auxil);
int calc_parse(calc_context_t *ctx, int *ret);
void calc_destroy(calc_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* !PCC_INCLUDED__HOME_SSZCZYRB_REPOS_CPPTEST_SRC_CALC_H */
