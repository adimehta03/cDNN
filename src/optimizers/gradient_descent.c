#include "gradient_descent.h"
#include "../model/model.h"

extern __Model__ * m;

void GD(double lr){
  Computation_Graph * temp = m->graph;
  dARRAY * layer_weights, *layer_biases, *grad_W, *grad_b;
  
  while(temp!=NULL){
    m->current_layer = temp;
    if(temp->type!=INPUT){
      layer_weights = temp->DENSE->weights;
      layer_biases = temp->DENSE->bias;

      double mul_val = m->lambda / m->Y_train->shape[1];
      
      dARRAY * temp_reg_weight = NULL;
      temp_reg_weight = mulScalar(layer_weights,mul_val);

      grad_W = temp->DENSE->dW;
      temp->DENSE->dW = add(grad_W,temp_reg_weight); 
      
      free2d(grad_W);
      free2d(temp_reg_weight);
      
      grad_W = NULL;
      temp_reg_weight = NULL;
      
      grad_W = temp->DENSE->dW;
      grad_b = temp->DENSE->db;

      dARRAY * mul_lr_W = mulScalar(grad_W,lr);

      if(m->lambda==0.0)
        temp->DENSE->weights = subtract(layer_weights,mul_lr_W);
      else{

        dARRAY * reg_weight_decay = NULL;
        double mul_value = m->lambda * m->learning_rate / m->Y_train->shape[1];
        reg_weight_decay = mulScalar(layer_weights,mul_value);
        
        dARRAY * temp_weight_update = subtract(layer_weights,reg_weight_decay);
        
        free2d(reg_weight_decay);
        reg_weight_decay = NULL;

        temp->DENSE->weights = subtract(temp_weight_update,mul_lr_W);

        free2d(temp_weight_update);
        temp_weight_update = NULL;
      }
      
      free2d(layer_weights);
      free2d(mul_lr_W);
      
      dARRAY * mul_lr_b = mulScalar(grad_W,lr);
      temp->DENSE->bias = subtract(layer_biases,mul_lr_b);
      
      free2d(layer_biases);
      free2d(mul_lr_b);
      free2d(grad_W);
      free2d(grad_b);

      if(temp->DENSE->dropout_mask!=NULL)
        free2d(temp->DENSE->dropout_mask);
      if(temp->DENSE->A!=NULL)
        free2d(temp->DENSE->A);
      if(temp->DENSE->cache!=NULL)
        free2d(temp->DENSE->cache);
      if(temp->DENSE->dA!=NULL)
        free2d(temp->DENSE->dA);
      grad_W = grad_b = layer_weights = layer_biases = mul_lr_W = mul_lr_b = temp->DENSE->dA\
      =temp->DENSE->cache = temp->DENSE->A = temp->DENSE->dropout_mask = NULL;
    }
    temp = temp->next_layer;
  }
}