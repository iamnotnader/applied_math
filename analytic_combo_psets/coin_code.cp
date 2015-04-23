#include <iostream>
#include <pthread.h>
   using namespace std;

   const int num_coins = 4;
   const int total_cents = 100;

   struct indices {
      int start;
      int end;
   };

   struct ret {
      int num_ways;
      int best_coin_values[4];
   };

   int num_ways_to_make_change(int total_cents, int* coin_values, bool* has_value, int* cached_values)
   {
   	// cout << "entering: " << total_cents << endl;
   
      if (total_cents == 0) {
         cached_values[0] = 1;
         has_value[0] = true;
         return 1;
      }	
   
         int ret_sum = 0;
         for (int m = total_cents; m >= 1; m--) {
            for (int k = 0; k < num_coins; k++) {
               if ((m % coin_values[k]) == 0) {
                  if (has_value[total_cents-m]) {
							// cout << "HIT" << endl;
                     ret_sum += coin_values[k] * cached_values[total_cents-m];
                  } 
                  else {
                     // cout << "MISS" << endl;
                     ret_sum += coin_values[k] * num_ways_to_make_change(total_cents-m, coin_values, has_value, cached_values);
                  }
               }
            }
         }
         cached_values[total_cents] = ret_sum / total_cents;
         has_value[total_cents] = true;
   
   	
   	// cout << "returning: " << total_cents << endl;
   
      return cached_values[total_cents];
   }

   void* run_subset(void* arg) {
      struct indices* boundaries = (struct indices*)arg;
   	
      int num_ways_best = 0;
      int best_coin_values[num_coins];
   
      int coin_values[num_coins] = {0};
      bool has_value[total_cents + 1] = {0};
      int cached_values[total_cents + 1] = {0};
   
      for (int i = boundaries->start; i <= boundaries->end; i++) {
			// cout << i << endl;
         for (int j = i+1; j <= total_cents; j++) {
            for (int k = j+1; k <= total_cents; k++) {
               for(int l = k+1; l <= total_cents; l++) {
                  coin_values[0] = i;
                  coin_values[1] = j;
                  coin_values[2] = k;
                  coin_values[3] = l;
               	
                  for (int m = 0; m <= total_cents; m++) {
                     has_value[m] = false;
                     cached_values[m] = 0;
                  }
                  int temp =
                     num_ways_to_make_change(total_cents, coin_values, has_value, cached_values);
               		
                  if (temp > num_ways_best) {
                     // cout << "new best found " << temp << endl;
                     num_ways_best = temp;
                     for (int m = 0; m < num_coins; m++) {
                        best_coin_values[m] = coin_values[m];
                     }
                  }
               }
            }
         }
      }
   
      struct ret* result = new struct ret;
      result->num_ways = num_ways_best;
      for (int m = 0; m < num_coins; m++) {
         result->best_coin_values[m] = best_coin_values[m];
      }
   
      return result;
   }

   int main() {
		/*
      int coin_values[num_coins] = {1, 2, 3, 4};
      bool has_value[total_cents + 1] = {0};
      int cached_values[total_cents + 1] = {0};
   	
      for (int m = 0; m <= total_cents; m++) {
         has_value[m] = false;
         cached_values[m] = 0;
      }
   	
      cout << num_ways_to_make_change(total_cents, coin_values, has_value, cached_values) << endl;
      
      return 0;
		*/
		
      const int num_threads = total_cents;
      pthread_t threads[num_threads];
      struct indices inds[num_threads];
      for (int i = 0; i < num_threads; i++) {
         inds[i].start = 1 + i;
         inds[i].end = 1 + i;
         // cout << inds[i].start << " " << inds[i].end << endl;
         pthread_create(&threads[i], NULL, run_subset, &inds[i]);
      }
   
      int best_num_ways = -1;
      int best_coin_values[num_coins];
      for (int i = 0; i < num_threads; i++) {
         void* result;
         pthread_join(threads[i],&result);
       
         int this_num_ways = ((struct ret*)(result))->num_ways;
         int* this_best_coins = ((struct ret*)(result))->best_coin_values;
       
         if (this_num_ways > best_num_ways) {
            best_num_ways = this_num_ways;
            for (int m = 0; m < num_coins; m++) {
               best_coin_values[m] = this_best_coins[m];
            }
         }
      }
    
      cout << "Most number of ways to make change: " << best_num_ways << endl;
      cout << "Best coin values: ";
      for (int i = 0; i < num_coins; i++) {
         cout << best_coin_values[i] << " ";
      }
      cout << endl;
    
      return 0;
   }