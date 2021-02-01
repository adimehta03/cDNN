#include "../model/model.h"
#include "./cross_entropy_loss.h"

extern __Model__ * m;
cross_entropy_loss_layer * loss_layer = NULL;

void forward_pass_L2_LOSS(){
  // printf("output : \n");
  //   for(int i=0;i<m->output->shape[0];i++){
  //     for(int j=0;j<m->output->shape[1];j++){
  //       printf("%lf ",m->output->matrix[i*m->output->shape[1]+j]);
  //     }
  //     printf("\n");
  //   }
  // shape(m->output);
  //Store the number of training examples in a variable
  // printf("loss forward\n");
  int number_of_examples = m->Y_train->shape[1];
  dARRAY * Y = loss_layer->gnd_truth;
  //we will also store prev layer activation dims
  int act_dims[] = {m->output->shape[0],m->output->shape[1]};

  dARRAY * log_y_hat= NULL;
  log_y_hat = (dARRAY *)malloc(sizeof(dARRAY));
  log_y_hat->matrix = (double*)calloc(act_dims[0]*act_dims[1],sizeof(double));
  //calculate log(y_pred)
  // omp_set_num_threads(4);
  // #pragma omp parallel for
  for(int i=0;i<act_dims[0]*act_dims[1];i++){
    log_y_hat->matrix[i] = log(m->output->matrix[i]);
  }
  log_y_hat->shape[0] = act_dims[0];
  log_y_hat->shape[1] = act_dims[1];
  // printf("log_y_hat : \n");
  // for(int i=0;i<log_y_hat->shape[0];i++){
  //   for(int j=0;j<log_y_hat->shape[1];j++){
  //     printf("%lf ",log_y_hat->matrix[i*log_y_hat->shape[1]+j]);
  //   }
  //   printf("\n");
  // }
  // shape(log_y_hat);
  //calculating (1-y_pred)
  dARRAY * temp_ones = ones(act_dims);
  dARRAY * temp_sub = subtract(temp_ones,m->output);
  
  dARRAY * log_one_y_hat = NULL;
  log_one_y_hat = (dARRAY *)malloc(sizeof(dARRAY));
  log_one_y_hat->matrix = (double*)calloc(act_dims[0]*act_dims[1],sizeof(double));
  //calculate log(1-y_pred)
  // omp_set_num_threads(4);
  // #pragma omp parallel for
  for(int i=0;i<act_dims[0]*act_dims[1];i++){
    log_one_y_hat->matrix[i] = log(temp_sub->matrix[i]);
  }
  log_one_y_hat->shape[0] = act_dims[0];
  log_one_y_hat->shape[1] = act_dims[1];
  // printf("log_one_y_hat : \n");
  // for(int i=0;i<log_one_y_hat->shape[0];i++){
  //   for(int j=0;j<log_one_y_hat->shape[1];j++){
  //     printf("%lf ",log_one_y_hat->matrix[i*log_one_y_hat->shape[1]+j]);
  //   }
  //   printf("\n");
  // }
  // shape(log_one_y_hat);
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

  dARRAY * loss = add(loss_term_1,loss_term_2);
  
  free2d(loss_term_1);
  free2d(loss_term_2);
  loss_term_1 = loss_term_2 = NULL;
  // printf("loss : \n");
  // for(int i=0;i<loss->shape[0];i++){
  //   for(int j=0;j<loss->shape[1];j++){
  //     printf("%lf ",loss->matrix[i*loss->shape[1]+j]);
  //   }
  //   printf("\n");
  // }
  dARRAY * sum_of_losses = sum(loss,1);

  free2d(loss);
  loss = NULL;

  dARRAY * cost = NULL;
  dARRAY * cross_entropy_cost = divScalar(sum_of_losses,(double)(-1 * number_of_examples));

  free2d(sum_of_losses);
  sum_of_losses = NULL;

  double reg_cost=0.0;
  Computation_Graph * temp = NULL;
  
  if(m->regularization!=NULL){
    temp = m->graph->next_layer;
    if(!strcasecmp(m->regularization,"L2")){
      double layer_frobenius = 0.0;
      while(temp->next_layer->type!=LOSS){
        layer_frobenius += frobenius_norm(temp->DENSE->weights);
        temp = temp->next_layer;
      }
      reg_cost = m->lambda*layer_frobenius/(2.0*number_of_examples);
    }
    else if(!strcasecmp(m->regularization,"L1")){
      double layer_manhattan = 0.0;
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

    double total_cost = cost->matrix[0];
    
    free2d(cost);
    cost = NULL;
    
    m->iter_cost = total_cost;
  }
  else{
    cost = cross_entropy_cost;
    double total_cost = cost->matrix[0];

    free2d(cost);
    cost = NULL;
    m->iter_cost = total_cost;
  }
  // double loss1 = (m->Y_train->matrix[0] * log(m->output->matrix[0])) + ((1-m->Y_train->matrix[0])*log((1-m->output->matrix[0])));
  // printf("loss 1 : %lf\n",loss1);
  // double loss2 = (m->Y_train->matrix[1] * log(m->output->matrix[1])) + ((1-m->Y_train->matrix[1])*log((1-m->output->matrix[1])));
  // printf("loss 2 : %lf\n",loss2);
  // double cost_manual = -1*(loss1+loss2)/(double)m->num_of_training_examples;
  // printf("cost using manual method : %lf\n",cost_manual);
  // printf("loss forward done\n");
}

void backward_pass_L2_LOSS(){
  // dARRAY * lgrad1 = divison(m->Y_train,m->output);

  int act_dims[] = {m->output->shape[0],m->output->shape[1]};
  dARRAY * one = ones(act_dims);
  dARRAY * temp1 = subtract(m->Y_train,m->output);
  // printf("Y-A : \n");
  // for(int i=0;i<temp1->shape[0];i++){
  //   for(int j=0;j<temp1->shape[1];j++){
  //     printf("%lf ",temp1->matrix[i*temp1->shape[1]+j]);
  //   }
  //   printf("\n");
  // }
  dARRAY * temp2 = subtract(one,m->output);
  dARRAY * temp3 = multiply(m->output,temp2);
  // printf("A(1-A) : \n");
  // for(int i=0;i<temp3->shape[0];i++){
  //   for(int j=0;j<temp3->shape[1];j++){
  //     printf("%lf ",temp3->matrix[i*temp3->shape[1]+j]);
  //   }
  //   printf("\n");
  // }
  free2d(one);
  one = NULL;

  dARRAY * lgrad2 = divison(temp1,temp3);

  free2d(temp1);
  free2d(temp2);
  free2d(temp3);
  temp1 = temp2 = temp3 = NULL;

  // dARRAY * temp_local_grad = subtract(lgrad1,lgrad2);
  loss_layer->grad_out = mulScalar(lgrad2,-1.0);
  // printf("Grad Out : \n");
  // for(int i=0;i<loss_layer->grad_out->shape[0];i++){
  //   for(int j=0;j<loss_layer->grad_out->shape[1];j++){
  //     printf("%lf ",loss_layer->grad_out->matrix[i*loss_layer->grad_out->shape[1]+j]);
  //   }
  //   printf("\n");
  // }

  // free2d(lgrad1);
  free2d(lgrad2);
  // free2d(temp_local_grad);
  // lgrad1 = lgrad2 = temp_local_grad = NULL;
  lgrad2 = NULL;
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