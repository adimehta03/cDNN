#include "./rmsprop.h"
#include "../model/model.h"

extern __Model__ * m;
//cache = cache * decay_rate + (1-decay_rate) * dx^2
void RMSProp(){
  Computation_Graph * temp = m->graph->next_layer;
  int layer = 0;
  while(temp!=NULL){
    dARRAY * scaled_dW = power(temp->DENSE->dW,2);
    dARRAY * scaled_db = power(temp->DENSE->db,2);

    double mul_factor = 1-m->beta;
    dARRAY * wavg_term2_dW = mulScalar(scaled_dW,mul_factor);
    dARRAY * wavg_term2_db = mulScalar(scaled_db,mul_factor);

    free2d(scaled_dW);
    free2d(scaled_db);
    scaled_dW = scaled_db = NULL;

    mul_factor = m->beta;
    dARRAY * ptr_cache_dW = m->cache_dW[layer];
    dARRAY * ptr_cache_db = m->cache_db[layer];

    dARRAY * wavg_term1_dW = mulScalar(ptr_cache_dW,mul_factor);
    dARRAY * wavg_term1_db = mulScalar(ptr_cache_db,mul_factor);

    free2d(ptr_cache_dW);
    free2d(ptr_cache_db);
    ptr_cache_dW = ptr_cache_db = NULL;

    m->cache_dW[layer] = add(wavg_term1_dW,wavg_term2_dW);
    m->cache_db[layer] = add(wavg_term1_db,wavg_term2_db);

    free2d(wavg_term1_dW);
    free2d(wavg_term1_db);
    free2d(wavg_term2_dW);
    free2d(wavg_term2_db);
    wavg_term1_dW = wavg_term1_db = wavg_term2_dW = wavg_term2_db = NULL;

    dARRAY * div_factor_temp_dW = power(m->cache_dW[layer],0.5);
    dARRAY * div_factor_temp_db = power(m->cache_db[layer],0.5);

    dARRAY * div_factor_dW = addScalar(div_factor_temp_dW,m->epsilon);
    dARRAY * div_factor_db = addScalar(div_factor_temp_db,m->epsilon);

    free2d(div_factor_temp_dW);
    free2d(div_factor_temp_db);
    div_factor_temp_dW = div_factor_temp_db = NULL;

    dARRAY * mul_lr_w = mulScalar(temp->DENSE->dW,m->learning_rate);
    dARRAY * mul_lr_b = mulScalar(temp->DENSE->db,m->learning_rate);

    dARRAY * update_term_dW = divison(mul_lr_w,div_factor_dW);
    dARRAY * update_term_db = divison(mul_lr_b,div_factor_db);

    free2d(div_factor_dW);
    free2d(div_factor_db);
    free2d(mul_lr_w);
    free2d(mul_lr_b);
    div_factor_dW = div_factor_db = mul_lr_w = mul_lr_b = NULL;

    dARRAY * grad_W = temp->DENSE->weights;
    dARRAY * grad_b = temp->DENSE->bias;

    temp->DENSE->weights = subtract(grad_W,update_term_dW);
    temp->DENSE->bias = subtract(grad_b,update_term_db);

    free2d(grad_W);
    free2d(grad_b);
    free2d(update_term_dW);
    free2d(update_term_db);
    grad_W = grad_b = update_term_dW = update_term_db = NULL;

    if(temp->DENSE->dropout_mask!=NULL)
      free2d(temp->DENSE->dropout_mask);
    if(temp->DENSE->A!=NULL)
      free2d(temp->DENSE->A);
    if(temp->DENSE->cache!=NULL)
      free2d(temp->DENSE->cache);
    if(temp->DENSE->dA!=NULL)
      free2d(temp->DENSE->dA);
    if(temp->DENSE->dZ!=NULL)
      free2d(temp->DENSE->dZ);
    temp->DENSE->dA=temp->DENSE->cache = temp->DENSE->A = temp->DENSE->dropout_mask = temp->DENSE->dZ = NULL;
    
    layer++;
    temp = temp->next_layer;
  }
}