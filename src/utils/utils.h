#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <math.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

  typedef struct array{
    double * matrix;
    int shape[2];
  }dARRAY;

  dARRAY * zeros(int * dims);
  dARRAY * ones(int * dims);
  dARRAY * eye(int * dims);
  dARRAY * randn(int * dims);
  dARRAY * add(dARRAY * MatrixA, dARRAY * MatrixB);
  dARRAY * addScalar(dARRAY * matrix, double scalar);
  dARRAY * subtract(dARRAY * MatrixA, dARRAY * MatrixB);
  dARRAY * subScalar(dARRAY * matrix, double scalar);
  dARRAY * sum(dARRAY * matrix, int axis);
  dARRAY * transpose(dARRAY *  Matrix);
  dARRAY * dot(dARRAY *  MatrixA, dARRAY *  MatrixB);
  dARRAY * multiply(dARRAY * MatrixA, dARRAY * MatrixB);
  dARRAY * mulScalar(dARRAY * matrix, double scalar);
  dARRAY * divison(dARRAY * MatrixA, dARRAY * MatrixB);
  dARRAY * divScalar(dARRAY * matrix, double scalar);
  dARRAY * power(dARRAY * matrix, int power);
  dARRAY * b_cast(dARRAY * MatrixA, dARRAY * MatrixB);
  dARRAY * reshape(dARRAY * matrix, int * dims);

  double mean(dARRAY * matrix);
  double var(dARRAY * matrix, char * type);
  double std(dARRAY * matrix, char * type);
  double gaussGenerator(double * cache, int * return_cache);
  double gaussRandom();
  double rand_norm(double mu, double std);

  int size(dARRAY * A);
  void shape(dARRAY * A);
  void free2d(dARRAY * matrix);
  void sleep(int milliseconds);
#ifdef __cplusplus
}
#endif
#endif //UTILS_H