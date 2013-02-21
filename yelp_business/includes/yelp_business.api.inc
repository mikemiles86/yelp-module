<?php

/**
 * @file yelp_business.api.inc
 */

/**
 * Function calls, or retrieves from cache, yelp data for a specified business id
 *
 * @param string $business_id
 * @return mixed Returns an object (JSON decoded response from Yelp) if results were retrieved, else FALSE.
 */
function yelp_business_lookup($business_id) {
  module_load_include('inc', 'yelp_api', 'yelp_api.pages');
  if (!$business_id) {
    return FALSE;
  }

  $url = 'http://api.yelp.com/v2/business/' . $business_id;

  //see if results are cached
  if ($result = cache_get('yelp-api-business-' . $business_id , 'cache')) {
    $result = $result->data;
  }
  elseif ($result = _yelp_api_apiCall($url)) {
    //save results to cache for x hour(s) ,  default is 1
    $result_cache_hours  = variable_get('yelp_api_cache_results' , 1);
    cache_set('yelp-api-busiess-' . $business_id , $result , 'cache' , mktime(date('H')+$result_cache_hours));
  }

  if ($result) {
    $result = json_decode($result);
    $result = yelp_business_hook_alter_business_result($result , $business_id);
  }
  else{
    $result = FALSE;
  }

  return $result;
}


/**
 * Calls the hook: HOOK_yelp_api_alter_business_result().
 *
 * Allows other modules to alter a business result after being feteched from Yelp API (or cahced values)
 * Implements the HOOK_yelp_api_alter_business_result() as invoked by other modules.
 *
 * @param object $result JSON decoded response from Yelp API
 * @param string $business_id
 * @return object
 */
function yelp_business_hook_alter_business_result($result, $business_id) {
  foreach (module_implements('yelp_business_alter_business_result') as $module) {
    $result = module_invoke($module , 'yelp_business_alter_business_result' , $result , $business_id);
  }
  return $result;
}
