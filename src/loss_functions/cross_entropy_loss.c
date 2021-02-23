#include "../model/model.h"
#include "./cross_entropy_loss.h"

extern __Model__ * m;
cross_entropy_loss_layer * loss_layer = NULL;

void forward_pass_L2_LOSS(){
  //Store the number of training examples in a variable
  int number_of_examples = m->Y_train->shape[1];
  dARRAY * Y = loss_layer->gnd_truth;
  int act_dims[] = {m->output->shape[0],m->output->shape[1]};
  dARRAY * loss = NULL;

  if(!strcasecmp(m->current_layer->prev_layer->DENSE->activation,"softmax")){
    dARRAY * log_y_hat= NULL;
    log_y_hat = (dARRAY *)malloc(sizeof(dARRAY));
    log_y_hat->matrix = (float*)calloc(act_dims[0]*act_dims[1],sizeof(float));
    //calculate log(y_pred)
    #pragma omp parallel for num_threads(8)
    for(int i=0;i<act_dims[0]*act_dims[1];i++){
      log_y_hat->matrix[i] = log(m->output->matrix[i]);
    }
    log_y_hat->shape[0] = act_dims[0];
    log_y_hat->shape[1] = act_dims[1];

    // printf("calculating y*log(y_hat)\n");
    dARRAY * loss_term_temp = multiply(Y,log_y_hat);
    
    free2d(log_y_hat);
    log_y_hat = NULL;

    loss = sum(loss_term_temp,0);

    free2d(loss_term_temp);
    loss_term_temp = NULL;
  }
  else{
    dARRAY * log_y_hat= NULL;
    log_y_hat = (dARRAY *)malloc(sizeof(dARRAY));
    log_y_hat->matrix = (float*)calloc(act_dims[0]*act_dims[1],sizeof(float));
    //calculate log(y_pred)
    #pragma omp parallel for num_threads(8)
    for(int i=0;i<act_dims[0]*act_dims[1];i++){
      log_y_hat->matrix[i] = log(m->output->matrix[i]);
    }
    log_y_hat->shape[0] = act_dims[0];
    log_y_hat->shape[1] = act_dims[1];

    //calculating (1-y_pred)
    dARRAY * temp_ones = ones(act_dims);
    dARRAY * temp_sub = subtract(temp_ones,m->output);
    
    dARRAY * log_one_y_hat = NULL;
    log_one_y_hat = (dARRAY *)malloc(sizeof(dARRAY));
    log_one_y_hat->matrix = (float*)calloc(act_dims[0]*act_dims[1],sizeof(float));
    //calculate log(1-y_pred)
    #pragma omp parallel for num_threads(8)
    for(int i=0;i<act_dims[0]*act_dims[1];i++){
      log_one_y_hat->matrix[i] = log(temp_sub->matrix[i]);
    }
    log_one_y_hat->shape[0] = act_dims[0];
    log_one_y_hat->shape[1] = act_dims[1];
    free2d(temp_sub);
    temp_sub = NULL;
    //calculate y*log(y^)
    //Y could be Y_train, Y_test, Y_cv
    dARRAY * loss_term_1 = multiply(Y,log_y_hat);
    
    free2d(log_y_hat);
    log_y_hat = NULL;

    //calculate (1-Y)
    temp_sub = subtract(temp_ones,Y);
    
    free2d(temp_ones);
    temp_ones = NULL;

    //calculate (1-Y)*log(1-Y^)
    dARRAY * loss_term_2 = multiply(temp_sub,log_one_y_hat);
    
    free2d(temp_sub);
    temp_sub = NULL;

    free2d(log_one_y_hat);
    log_one_y_hat=NULL;

    loss = add(loss_term_1,loss_term_2);
    
    free2d(loss_term_1);
    free2d(loss_term_2);
    loss_term_1 = loss_term_2 = NULL;
  }

  // printf("calculating sum of losses\n");
  dARRAY * sum_of_losses = sum(loss,1);

  free2d(loss);
  loss = NULL;

  dARRAY * cost = NULL;
  dARRAY * cross_entropy_cost = divScalar(sum_of_losses,(float)(-1.0 * number_of_examples));
  // printf("calculated cost\n");
  
  free2d(sum_of_losses);
  sum_of_losses = NULL;

  float reg_cost=0.0;
  Computation_Graph * temp = NULL;
  
  if(m->regularization!=NULL){
    temp = m->graph->next_layer;
    if(!strcasecmp(m->regularization,"L2")){
      float layer_frobenius = 0.0;
      while(temp->next_layer->type!=LOSS){
        layer_frobenius += frobenius_norm(temp->DENSE->weights);
        temp = temp->next_layer;
      }
      reg_cost = m->lambda*layer_frobenius/(2.0*number_of_examples);
    }
    else if(!strcasecmp(m->regularization,"L1")){
      float layer_manhattan = 0.0;
      while(temp->next_layer->type!=LOSS){
        layer_manhattan += Manhattan_distance(temp->DENSE->weights);
        temp = temp->next_layer;
      }
      reg_cost = m->lambda * layer_manhattan/(2.0*number_of_examples);
    }
  }
  if(m->regularization!=NULL){
    cost = addScalar(cross_entropy_cost,reg_cost);
    temp = NULL;

    free2d(cross_entropy_cost);
    cross_entropy_cost = NULL;

    float total_cost = cost->matrix[0];
    // printf("calculated total cost\n");
    free2d(cost);
    cost = NULL;
    
    m->iter_cost = total_cost;
  }
  else{
    cost = cross_entropy_cost;
    float total_cost = cost->matrix[0];
    // printf("calculated total cost\n");
    free2d(cost);
    cost = NULL;
    m->iter_cost = total_cost;
  }
}

void backward_pass_L2_LOSS(){
  int act_dims[] = {m->output->shape[0],m->output->shape[1]};
  if(!strcasecmp(m->current_layer->prev_layer->DENSE->activation,"softmax")){
    dARRAY * temp = divison(loss_layer->gnd_truth,m->output);
    dARRAY * temp2 = transpose(temp);
    
    dARRAY * class_sum = (dARRAY*)malloc(sizeof(dARRAY));
    class_sum->matrix = (float*)calloc(temp->shape[1],sizeof(float));
    
    omp_set_num_threads(8);
    #pragma omp parallel for num_threads(8) collapse(1) shared(temp,class_sum) schedule(static)
    for(int i=0;i<temp2->shape[0];i++){
      float sum_of_exps=0.0;
      for(int j=0;j<temp2->shape[1];j++){
        sum_of_exps+= temp2->matrix[i*temp2->shape[1]+j];
      }
      class_sum->matrix[i] = sum_of_exps;
    }
    class_sum->shape[0] = 1;
    class_sum->shape[1] = temp->shape[1];
    
    free2d(temp);
    temp = NULL;

    free2d(temp2);
    temp2 = NULL;
    
    loss_layer->grad_out = mulScalar(class_sum,-1.0);
    
    free2d(class_sum);
    class_sum = NULL;
  }
  else{
    dARRAY * one = ones(act_dims);
    dARRAY * temp1 = subtract(m->Y_train,m->output);
    dARRAY * temp2 = subtract(one,m->output);
    dARRAY * temp3 = multiply(m->output,temp2);
    free2d(one);
    one = NULL;

    dARRAY * lgrad2 = divison(temp1,temp3);

    free2d(temp1);
    free2d(temp2);
    free2d(temp3);
    temp1 = temp2 = temp3 = NULL;

    loss_layer->grad_out = mulScalar(lgrad2,-1.0);
    free2d(lgrad2);
    lgrad2 = NULL;
  }
}

void (cross_entropy_loss)(cross_entropy_loss_args args){
  loss_layer = (cross_entropy_loss_layer*)malloc(sizeof(cross_entropy_loss_layer));
  loss_layer->cost = 0.0;
  loss_layer->grad_out = NULL;
  loss_layer->forward = forward_pass_L2_LOSS;
  loss_layer->backward = backward_pass_L2_LOSS;
  loss_layer->gnd_truth = m->Y_train;
  append_graph(loss_layer,"loss");
}