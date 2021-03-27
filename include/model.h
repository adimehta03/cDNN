#ifndef MODEL_H
#define MODEL_H

#include "neural_net.h"
#include "utils.h"

typedef struct model_args{
  dARRAY * x_train;
  dARRAY * Y_train;
  dARRAY * x_cv;
  dARRAY * Y_cv;
  dARRAY * x_test;
  dARRAY * Y_test;
  float learning_rate;
  int mini_batch_size;
  int num_iter;
  char * optimizer;
  char * loss;
  char * regularization;
  float lambda;
  int print_cost;
  float beta;
  float beta1;
  float beta2;
  int checkpoint_every;
}Model_args;

#ifdef __cplusplus
extern "C"{
#endif
  typedef void (*__model_init__)();
  typedef void (*__model_fit__)();
  typedef void (*__model_predict__)(dARRAY * input_feature);
  typedef void (*__model_save__)(char * file_name);
  typedef void (*__model_load__)();
  typedef void (*__model_summary__)();
  typedef void (*__model_forward__)();
  typedef void (*__model_backward__)();
  typedef void (*__model_test__)();
  void __initialize_params__();
  void (create_model)();
  void (Model)(Model_args model_args);
  void __init__();
  void __forward__();
  void __backward__();
  void __fit__();
  void __load_model__();
  void __save_model__();
  void __summary__();
  void __predict__();
  void __test__();
  float calculate_accuracy(dARRAY * predicted, dARRAY * gnd_truth);
  float calculate_test_val_acc(dARRAY * input_features,dARRAY * gnd_truth);
  dARRAY * relu_val(dARRAY * linear_matrix);
  dARRAY * sigmoid_val(dARRAY * linear_matrix);
  dARRAY * tanh_val(dARRAY * linear_matrix);
  dARRAY * softmax_val(dARRAY * linear_matrix);
  dARRAY * load_x_train(char * filename, int * dims);
  dARRAY * load_y_train(char * filename, int * dims);
  dARRAY * load_x_cv(char * filename, int * dims);
  dARRAY * load_y_cv(char * filename, int * dims);
  dARRAY * load_x_test(char * filename, int * dims);
  dARRAY * load_y_test(char * filename, int * dims);
  void create_mini_batches();
  void dump_to_file(float * arr ,char * filename,char * mode);
  void dump_image(dARRAY * images);
  dARRAY * load_test_image(char * filename);
  void (destroy_model)();
#ifdef __cplusplus
}
#endif

typedef struct model{
  Computation_Graph * graph;
  Computation_Graph * current_layer;
  int number_of_layers;

  float learning_rate;
  int mini_batch_size;
  int num_mini_batches;
  int current_mini_batch;
  dARRAY * x_train;
  dARRAY * Y_train;
  dARRAY * x_test;
  dARRAY * Y_test;
  dARRAY * x_cv;
  dARRAY * Y_cv;
  dARRAY ** x_train_mini_batch;
  dARRAY ** y_train_mini_batch;
  dARRAY * output;

  int num_of_training_examples;
  int print_cost;
  int num_iter;
  char * optimizer;
  char * regularization;
  float lambda;
  char * loss;
  float beta;
  float beta1;
  float beta2;
  int time_step;
  dARRAY * m_t_dW[1024];
  dARRAY * v_t_dW[1024];
  dARRAY * m_t_db[1024];
  dARRAY * v_t_db[1024];
  float epsilon;
  dARRAY * cache_dW[1024];
  dARRAY * cache_db[1024];

  int input_size;
  int output_size;
  long int total_parameters;
  float train_cost;
  float iter_cost;
  float train_accuracy;
  float test_accuracy;
  float cross_val_accuracy;
  int predicting;
  int testing;
  int ckpt_every;
  int current_iter;

  __model_init__ init;
  __model_fit__ fit;
  __model_predict__ predict;
  __model_test__ test;
  __model_save__ save_model;
  __model_load__ load_model;
  __model_summary__ summary;
  __model_forward__ forward;
  __model_backward__ backward;
}__Model__;

#define create_model(...) create_model();

#define Model(...) Model((Model_args){\
.x_train=NULL,.Y_train=NULL,\
.x_cv=NULL,.Y_cv=NULL,\
.x_test=NULL,.Y_test=NULL,\
.num_iter=10,\
.mini_batch_size=64,\
.optimizer="Adam",\
.regularization=NULL,\
.lambda=0.0,\
.learning_rate=3e-4,\
.print_cost=1,\
.beta=0.9,\
.beta1=0.9,\
.beta2=0.999,\
.loss="cross_entropy_loss",\
.checkpoint_every=2500,__VA_ARGS__});

#define destroy_model(...) destroy_model();

#endif