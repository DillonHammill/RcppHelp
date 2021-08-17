#include <Rcpp.h>
#include <RcppParallel.h>
using namespace Rcpp;
using namespace RcppParallel;

//[[Rcpp::export]]
NumericMatrix rcpp_dist(NumericMatrix x, 
                        int dist_method = 1) {
  
  // code to compute distance matrix depends on dist_method
  NumericMatrix x_dist(x.nrow(), x.nrow());
  return x_dist;
  
}

//[[Rcpp::export]]
IntegerVector rcpp_algo(NumericMatrix x, 
                        NumericMatrix dist) {
  
  // code to compute a label for each row in x using distance matrix
  IntegerVector labels(x.nrow());
  return labels;
  
}

#include <RcppParallel.h>
using namespace RcppParallel;

struct RcppParAlgo : public Worker {
  
  // source thread safe
  RMatrix<double> x;
  RVector<int> dist_method;
    
    // output thread safe
    RMatrix <int> labels;
  
  // initialise
  RcppParAlgo(NumericMatrix x, int dist_method)
    : x(x), dist_method(dist_method), labels(labels) {}
  
  // compute distance matrix based on method and run algo to get labels
  void operator() (std::size_t begin, std::size_t end) {
    
    // call rcpp_dist on x with dist_method
    // I know this code is wrong but hopefully shows what I am trying to do
    for(std::size_t i = begin; i < end; i++) {
      RVector<int> dist_type = dist_method[i];
      RMatrix<double> x_dist = rcpp_dist(x, dist_type);
      RVector<int> x_labels = rcpp_algo(x, x_dist);
      // push lables into output matrix
      for(int j = 0; j <= x_labels.size(); j++) {
        labels(j, i) = x_labels[j];
      }
    }
    
  }
  
};

//[[Rcpp::export]]
IntegerMatrix rcppParAlgo(NumericMatrix x) {
  
  // distance methods
  IntegerVector dist_methods = {1,2,3,4,5};
  
  // allocate output matrix to store labels
  IntegerMatrix labels(x.nrow(), dist_methods.size());
  
  // functor
  RcppParAlgo RcppParAlgo(x, dist_methods);
  
  // run in parallel
  parallelFor(0, dist_methods.size(), RcppParAlgo);
    
  return labels;  
  
}

/*** R
rcppParAlgo(
  matrix(
    1:100 + 0.1,
    ncol = 2,
    dimnames = list(NULL, c("x", "y"))
  )
)
*/
