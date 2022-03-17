#include "rlibm-prog.h"


polynomial* rlibm_solve_with_soplex(sample_data* sintervals,
				    size_t ssize, int* power, int termsize){
  
  SoPlex mysoplex;
  mysoplex.setBoolParam(SoPlex::RATFACJUMP,true);
  mysoplex.setIntParam(SoPlex::SOLVEMODE,2);
  mysoplex.setIntParam(SoPlex::CHECKMODE,2);
  mysoplex.setIntParam(SoPlex::SYNCMODE,1);
  mysoplex.setIntParam(SoPlex::READMODE,1);
  mysoplex.setRealParam(SoPlex::FEASTOL,0.0);
  mysoplex.setRealParam(SoPlex::OPTTOL,0.0);
  mysoplex.setRealParam(SoPlex::EPSILON_ZERO,0.0);
  mysoplex.setRealParam(SoPlex::EPSILON_FACTORIZATION,0.0);
  mysoplex.setRealParam(SoPlex::EPSILON_UPDATE,0.0);
  mysoplex.setRealParam(SoPlex::EPSILON_PIVOT,0.0);
  mysoplex.setIntParam(SoPlex::VERBOSITY,0);
  mysoplex.setRealParam(SoPlex::TIMELIMIT,5*60);
  
  /* we first add objective variables */
  DSVectorRational dummycol(0);
  for(int i=0;i<termsize;i++){
    auto column=LPColRational(1.0,dummycol,infinity,-infinity);
    mysoplex.addColRational(column);
  }
  
  /* then add constraints one by one */
  for(int i = 0; i < ssize; i++){
    Rational xValR(sintervals[i].x);
    DSVectorRational row1(termsize);
    
    for(int j=0; j<termsize;j++){
      Rational toAdd(1.0);
      for(int k=0;k<power[j];k++) toAdd*=xValR;

      row1.add(j,toAdd);
    }
        
    // LPRow: low bound, row, upper bound
    double lbnd= sintervals[i].lb;
    double ubnd= sintervals[i].ub;
    mysoplex.addRowRational(LPRowRational(lbnd,row1,ubnd));
  }

  /* solve LP */
  SPxSolver::Status stat;
  stat=mysoplex.optimize();
  
  /* get solution */
  if(stat==SPxSolver::OPTIMAL){
    DVectorRational prim(termsize);
    mysoplex.getPrimalRational(prim);

    /* generate the polynomial as a plain structure */
    polynomial* p = (polynomial *) calloc(1, sizeof(polynomial));
    p->termsize = termsize;
    p->power = power;
    p->coeffs = (double *) calloc(termsize, sizeof(double));
    
    for(int i=0;i<termsize;i++)
      p->coeffs[i] = mpq_get_d(*(prim[i].getMpqPtr_w()));

    return p;
  }
  else if(stat==SPxSolver::UNBOUNDED){

    polynomial* p = (polynomial *) calloc(1, sizeof(polynomial));
    p->termsize = termsize;
    p->power = power;
    p->coeffs = (double *) calloc(termsize, sizeof(double));
    
    for(int i=0;i<termsize;i++)
      p->coeffs[i] = 0.0;
    
    return p;
  }
  
  return nullptr;
}

void check_sorted(sample_info* sampled_indices, size_t ssize){
  double min= sampled_indices[0].key;

  for(size_t i = 0; i< ssize; i++){
    assert ( min <= sampled_indices[i].key);
    min = sampled_indices[i].key;
  }
  
}


double rlibm_poly_evaluation(double x, polynomial* poly){

  double ret_val = 0.0;

  // simulated Horner's method
  for(int i = poly->termsize-1; i> 0; i--){
    ret_val = ret_val + poly->coeffs[i];
    double xmul = 1.0;
    for(int j = 0; j < (poly->power[i] - poly->power[i-1]); j++){
      xmul = xmul * x;
    }
    ret_val = ret_val * xmul;	  
  }
  ret_val = ret_val + poly->coeffs[0];
  
  for(int j = 0; j < poly->power[0]; j++){
    ret_val = ret_val * x;
  }  
  return ret_val;
}

bool rlibm_validate_and_fix_intervals(sample_data* sintervals,
				      size_t ssize, polynomial* poly){

  bool return_val = true;
  for(size_t i = 0; i < ssize; i++){
    double y = rlibm_poly_evaluation(sintervals[i].x, poly);

    if(y < sintervals[i].orig_lb){
      return_val = false;
      double_x lbx;
      lbx.d = sintervals[i].lb;
      if(lbx.d >= 0) {
	lbx.x = lbx.x + 1;
      }
      else{
	lbx.x = lbx.x - 1 ;
      }
      sintervals[i].lb = lbx.d;
    }
    else if(y > sintervals[i].orig_ub){
      return_val = false;
      double_x ubx;
      ubx.d = sintervals[i].ub;
      if(ubx.d >= 0){
	ubx.x = ubx.x - 1;
      }
      else {
	ubx.x = ubx.x + 1;
      }
      sintervals[i].ub = ubx.d;
    }    
  }
  return return_val;
}

polynomial*
rlibm_generate_polynomial(sample_data* sintervals, size_t ssize,
			  int* power, int power_size, int max_tries, int *prev_successful_degree){

  for(int i = *prev_successful_degree; i < power_size; i++){
    printf("Trying to generate a polynomial  with %d terms \n", i+1);

    int count = 0;
    while(count < max_tries){
      polynomial* p = rlibm_solve_with_soplex(sintervals, ssize, power, i+1);
      if(p && rlibm_validate_and_fix_intervals(sintervals, ssize, p)){
	*prev_successful_degree = i;
	return p;
      }
      if(p != nullptr){
	free(p);
      }
      count++;
    }    
  }
  return nullptr;

}

int sample_compare(const void* s1, const void* s2){

  sample_info* e1 = (sample_info*) s1;
  sample_info* e2 = (sample_info*) s2;
  return e1->key > e2->key;
}

void rlibm_print_sample(sample_info* sampled_indices, size_t size){

  double prev = 0.0;
  for(size_t i = 0; i < size; i++){
    assert(sampled_indices[i].key >= prev);
    prev = sampled_indices[i].key;
    printf("counter=%lu, key=%e, sample_index=%lu\n", i, sampled_indices[i].key,
	   sampled_indices[i].index);
  }
}

void rlibm_weighted_random_sample(sample_info* sampled_indices, size_t ssize,
				  interval_data* intervals, size_t nentries){

  for(size_t i = 0; i < ssize; i++){
    sampled_indices[i].key = pow(intervals[i].u, 1./(intervals[i].w));
    sampled_indices[i].index = i;
  }
  
  qsort(sampled_indices, ssize, sizeof(sample_info), sample_compare);
  //  check_sorted (sampled_indices, ssize);

  /* select the top ssize indices from the entire interval data */
  
  for(size_t i = ssize; i < nentries; i++){

    /* if the ith element is smaller than the least element in the
       sample, then nothing to do */
    size_t j= 0;

    double interval_key = pow(intervals[i].u, 1./(intervals[i].w));
    
    if(interval_key > sampled_indices[0].key){
      /* do insertion sort of the data */
      while(interval_key > sampled_indices[j].key && j < ssize) j++;

      for(size_t t=1; t < j; t++) {
	sampled_indices[t-1] = sampled_indices[t];
      }
      sampled_indices[j-1].key = interval_key;
      sampled_indices[j-1].index = i;
    }
  }
  //  check_sorted(sampled_indices, ssize);
}


size_t rlibm_compute_violated_indices(size_t* violated_indices, interval_data* intervals, size_t nentries, polynomial* poly){

  size_t num_violated_indices = 0;
  for(size_t i = 0; i < nentries; i++){
    double y = rlibm_poly_evaluation(intervals[i].x, poly);
    if( y < intervals[i].lb || y > intervals[i].ub){
      violated_indices[num_violated_indices] = i;
      num_violated_indices++;
    }
  }
  return num_violated_indices;
}

void rlibm_evaluate_and_update_weights(size_t* violated_indices, size_t num_violated_indices,
				       interval_data* intervals, size_t nentries, size_t d){
  double w_v = 0.0;
  double w_s = 0.0;

  // this can be optimized to only change the updated weights. For now
  // using an inefficient one
  for (size_t i = 0; i < nentries; i++){
    w_s = w_s + intervals[i].w;
  }

  for(size_t i = 0; i < num_violated_indices; i++){
    w_v = w_v + intervals[violated_indices[i]].w;
  }

  //doubling the weights for a lucky iteration
  if(w_v <= 2 * w_s / (9*d -1)){
    for(size_t i = 0; i < num_violated_indices; i++){
      size_t vindex = violated_indices[i];
      intervals[vindex].w  = intervals[vindex].w * 2;
    }
  }  
}

void
rlibm_regenerate_random_values_and_reset_weights(interval_data* intervals,
						 size_t nentries){

  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_real_distribution<double> distribution(0.0, 1.0);

  for(size_t i = 0; i < nentries; i++){
    intervals[i].u = distribution(generator);
    intervals[i].w = 1.0;
  }
}

bool check_sampled_indices(sample_info* sample, sample_info* prev_sample, size_t size){

  for(size_t i =0; i < size; i++){
    if (sample[i].index != prev_sample[i].index){
      return false;
    }
  }
  return true;
}

void rlibm_print_polyinfo(polynomial* p){

  if(p->termsize == 0){
    printf("Polynomial has no terms!\n");
    exit(0);
  }

  printf("Polynomial: y=%.70e x^(%d)",p->coeffs[0],p->power[0]);
  for(int j=1;j<p->termsize;j++){
    printf(" + %.70e x^(%d)",p->coeffs[j],p->power[j]);
  }
  printf("\n");

}

interval_data* rlibm_read_interval_file(char* filename, size_t* nentries){
  
  FILE* fp = fopen(filename, "r");
  assert(fp != nullptr);

  /* count the number of entries */

  fseek(fp, 0, SEEK_END);
  size_t nentries_total = ftell(fp);
  nentries_total = nentries_total/(3*sizeof(double));
  printf("number of intervals = %lu\n", nentries_total);
  fseek(fp, 0, SEEK_SET);

  interval_data* intervals = (interval_data*) calloc(nentries_total, sizeof(interval_data));

  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_real_distribution<double> distribution(0.0, 1.0);

  for (unsigned long i = 0; i < nentries_total; i++){
    double data_entry[3];
    size_t bytes = fread(data_entry, sizeof(double), 3, fp);
    intervals[i].w = 1.0;
    intervals[i].u = distribution(generator);
    intervals[i].x = data_entry[0];
    intervals[i].lb = data_entry[1];
    intervals[i].ub = data_entry[2];
  }

  fclose(fp);
  *nentries = nentries_total;
  return intervals;
}


int main(int argc, char** argv){

  if(argc != 2){
    printf("Usage: %s <interval file> \n", argv[0]);
    exit(0);
  }

  printf("EXIT_ON_THRESHOLD is %d\n", RLIBM_EXIT_ON_THRESHOLD);

  size_t nentries_total = 0;
  interval_data* intervals_full = rlibm_read_interval_file(argv[1], &nentries_total);
  
  int powers[]={0, 1,2,3,4,5};
  int powers_size = sizeof(powers)/sizeof(powers[0]);
  
  //log2, log10
  //  int powers[]={0, 1,2,3,4,5,6, 7, 8, 9}; //coshx, exp2, exp10, exp
  // int powers[] = {1,3,5,7}; //sinhx
  // int powers[] = {0,2 ,4,6}; // coshx
  //int powers[] = {1,3 ,5,7}; // sinpi
  // int powers[] = {0,2,4,6}; // cospi
  size_t nentries_pieces = nentries_total/N_RLIBM_PIECES;
  
  int prev_successful_degree = 0;
  
  for(int i = 0; i < N_RLIBM_PIECES; i++){

    prev_successful_degree = 0;
    size_t start_index = i * nentries_pieces;

    size_t even_split = (i+1) * nentries_pieces;
    size_t end_index =  even_split > nentries_total? nentries_total: even_split;

    size_t nentries = end_index - start_index;
    interval_data* intervals = &intervals_full[start_index];

    printf("piece = %d\n", i);
    printf("start_index = %lu\n", start_index);
    printf("end_index = %lu\n", end_index);
    printf("nentries=%lu\n", nentries);

    /* sample size */
    
    size_t cd = 9 * powers_size * powers_size;
    size_t samplesize = cd;
    
    size_t n_violated_indices = 0;
    size_t *violated_indices = (size_t *) calloc(nentries, sizeof(size_t));
    
    sample_info* sampled_indices = (sample_info*) calloc(cd, sizeof(sample_info));
    
    size_t prev_violated_indices = 0;
    size_t matched_violated_indices = 0;
    
    sample_data* sampled_intervals = (sample_data *) calloc(cd, sizeof(sample_data));
    
    polynomial* p = nullptr;
    size_t total_iterations = 0;
    
    do{
      if(p != nullptr) free(p);
      
      n_violated_indices = 0;
      
      rlibm_weighted_random_sample(sampled_indices, cd, intervals, nentries);    
      total_iterations++;
      
      for (size_t i = 0; i < cd; i++){
	size_t iindex = sampled_indices[i].index;
	
	sampled_intervals[i].x = intervals[iindex].x;
	sampled_intervals[i].lb = intervals[iindex].lb;
	sampled_intervals[i].ub = intervals[iindex].ub;
	sampled_intervals[i].orig_lb = sampled_intervals[i].lb;
	sampled_intervals[i].orig_ub = sampled_intervals[i].ub;
	sampled_intervals[i].w = intervals[iindex].w;
	sampled_intervals[i].u = intervals[iindex].u;
	sampled_intervals[i].k = sampled_indices[i].key;
      }
      
      /* need to implement these functions */
      p = rlibm_generate_polynomial(sampled_intervals, samplesize, powers, powers_size, MAX_TRIES, &prev_successful_degree);
      
      if(p){
	n_violated_indices = rlibm_compute_violated_indices(violated_indices, intervals, nentries, p);
	printf("number of violated intervals: %lu, total iterations=%lu \n", n_violated_indices, total_iterations);
	
	if(n_violated_indices <= VIOLATE_THRESHOLD){
	  printf("VIOLATING INPUTS BELOW THRESHOLD:\n");
	  printf("starting input is %.70e\n", intervals[0].x);
	  
	  for(size_t m = 0; m < n_violated_indices; m++){
	    printf("violated_input is %.70e, lb is %.70e, ub is %.70e\n", intervals[violated_indices[m]].x, intervals[violated_indices[m]].lb, intervals[violated_indices[m]].ub);
	  }
	  rlibm_print_polyinfo(p);
	  if(RLIBM_EXIT_ON_THRESHOLD){
	    break;
	  }
	}
	
	rlibm_evaluate_and_update_weights(violated_indices, n_violated_indices, intervals, nentries, powers_size);
	
      }
      else {
	if(total_iterations > MAX_ITERATIONS){
	  printf("total iterations exceeded %d, terminating the polynomial geenerator\n", MAX_ITERATIONS);
	  if(p!= nullptr){
	    free(p);
	    p = nullptr;	  
	  }
	  break;
	}
	printf("failed to generate polynomial, resetting weights, total_iterations=%lu\n", total_iterations);
	prev_successful_degree = 0;      
	rlibm_regenerate_random_values_and_reset_weights(intervals, nentries);
      }
      
      /* debugging feature to reset weights for the sample if not making progress*/
      if(n_violated_indices != 0 && (prev_violated_indices == n_violated_indices)){
	matched_violated_indices++;
	if(matched_violated_indices > SAMPLE_MATCH_THRESHOLD){
	  matched_violated_indices = 0;
	  n_violated_indices = 0;
	  
	  printf("not making progress, same number of violated indices, resetting weights, total_iterations=%lu\n", total_iterations);
	  prev_successful_degree = 0;
	  rlibm_regenerate_random_values_and_reset_weights(intervals, nentries);
	  if(p!= nullptr) {
	    free(p);
	    p = nullptr;
	  }
	  continue;
	}
      }
      else{
	matched_violated_indices = 0;
	prev_violated_indices = n_violated_indices;
      }    
    } while(n_violated_indices > 0 || !p);
    
    if(p){
      rlibm_print_polyinfo(p);
    }
    else {
      printf("Could not generate the polynomial that satisifies all intervals, check for partial results with a few violated intervals\n");
    }
    free(p);
    free(sampled_intervals);
    free(sampled_indices);
    free(violated_indices);    
  } // ends the outer piecewise loop

  free(intervals_full);
  return 0;
}
