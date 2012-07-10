<?php
  /**
   * @file yelp_api.categories.inc
   *  Contains functions related to fetching, displaying, and selecting yelp categories
   */
   
  /**
   * Fetches a nested php associative array of Yelp categories.
   * Since Yelp does not provide an API call to be able to retrieve the list of thier categories
   * The HTML page has to be scraped. Invoked by yelp_api_get_categories()
   *
   * @return array 
   *  A nested array of yelp categories, or returns boolean FALSE if failed.
   */
  function _yelp_api_fetch_categories() {
    // get the band-aids ready, we're about to get some scrapes.
    // scrape the yelp.com categories, to get an updated list.
    $yelp_url = 'http://www.yelp.com/developers/documentation/category_list';
    module_load_include('inc', 'yelp_api', 'yelp_api.pages');
    if ($html=_yelp_api_doCall($yelp_url)) {
      //scrape the HTML to get the categories list
      $categories_html   = strip_tags(preg_replace('/[\s\t\n\r]+/', ' ', $html), '<ul><li>');
      $categories_html   = array_shift(explode('<ul> <li class="sidebar', '<ul>' . array_pop(explode('<ul class="attr-list">', $categories_html))));
      //change HTML list into json string
      $categories_json  = str_replace(array('<ul>', '</ul>', '<li>', '</li>'), array('[', ']', '"', '"'), $categories_html);
      //handle nested arrays
      $categories_json  = trim(str_replace(array('] [', '" "', '" [', '] "'), array('], [', '", "', '", [', '], "'), $categories_json));
      //convert json into php array
      $categories        = json_decode($categories_json);
      //format the arrays correctly (key, values and nesting)
      $categories        = _yelp_api_parse_categories($categories);
    }
    else{
      $categories        = FALSE;
    }
    
    return $categories;
  }
  
  /**
   * A recursive function that parses the raw categories php array from _yelp_api_fetch_categories 
   * and forms a nested associate array.
   *
   * @param array $raw
   *    raw php array of categories
   * @return $categories
   *    properly formatted nested associative array
   *
   * @see _yelp_api_fetch_categories()
   */
  function _yelp_api_parse_categories($raw) {
    $categories = array();
      foreach ($raw as $x => $category) {
        if (is_array($category)) {
          list($pkey, $pval) = _yelp_api_category_key_values($raw[$x-1]);
          $categories[$pkey] = array($pval, _yelp_api_parse_categories($category));
        }
        else{
          list($key, $value) = _yelp_api_category_key_values($category);
          $categories[$key] = $value;
        }
      }
      return $categories;
  }
  
  /**
   * Splits raw category into display/key values. Invoked by _yelp_api_parse_categories()
   *
   * @param string $category
   *   raw category string
   *
   * @return array
   *  array containing the $key, $value pair.
   *
   * @see _yelp_api_parse_categories()
   */
  function _yelp_api_category_key_values($category) {
    list ($value, $key) = explode('(', $category);
    $value  = trim($value);
    $key    = trim(str_replace(')', '', $key));
    return array($key, $value);
  }
  
  /**
   * Flattens yelp categories nested array into a single level, recursively.
   * Invoked by yelp_api_get_categories() if the $flatten flag is passed.
   *
   * @param array $categories
   *  nested array
   *
   * @return array
   *    a flatened array of categories
   *
   * @see yelp_api_get_categories() 
   */
  function _yelp_api_flatten_categories($categories) {
    $flattened = array();
    foreach ($categories as $key  => $value) {
      if (is_array($value)) {
        $flattened[$key] = $value[0];
        $sub_flat = _yelp_api_flatten_categories($value[1]);
        foreach ($sub_flat as $sub_key => $sub_value) {
          $flattened[$sub_key] = $sub_value;
        }
      }
      else{
        $flattened[$key] = $value;
      }
    }
    return $flattened;
  }
  
  /**
   * Returns a associative array of categories.
   * If the $flatten flag is not passed, the array is nested.
   *
   * @param boolena $flatten
   *  Boolean value to determine if the array returned should be nested or a single level.
   *
   * @return array
   *  Returns an associative array of yelp categories.
   */
  function yelp_api_get_categories($flatten=FALSE) {
    //see if categories are cached
    if ($categories = cache_get('yelp_api_categories')) {
      $categories = $categories->data;
    //no cache, fetch from yelp
    }
    elseif ($categories = _yelp_api_fetch_categories()) {
      //save cache for a month (yelp does not update their categories that often.)
      cache_set('yelp_api_categories', $categories, 'cache', mktime(0, 0, 0, (date('n')+1)));
    }
    else{
      $categories = FALSE;
    }
    //do we want to flatten and successfuly retireved a list? or just return categories.
    return ($flatten && $categories) ? _yelp_api_flatten_categories($categories):$categories;
  }
  
  /**
   * Recursive function to build checkbox form fields for categories. Invoked by yelp_api_build_form_categories()
   *
   * @param array &$form
   *  form object to add fields to, passed by reference
   * @param array $categories
   *  associative array of yelp categories.
   *
   * @return nothing
   *  this function returns nothing.
   *
   * @see yelp_api_build_form_categories()
   */
  function _yelp_api_build_categories_form_fields(&$form, $categories, $selected=array()) {
    foreach ($categories as $key  => $value) {
      $form['yelp-category-' . $key] = array(
        '#type'           => 'checkbox', 
        '#return_value'   => $key, 
      );
      //checkif category is selected.
      if (is_array($selected) && in_array($key, $selected)) {
        if (isset($form['#type']) && ($form['#type'] == 'fieldset')) {
          $form['#collapsed'] = FALSE;
        }
        $form['yelp-category-' . $key]['#attributes'] = array('checked' => 'checked');
      }
      
      if (is_array($value)) {
        $form['yelp-category-' . $key]['#title'] = $value[0];
        $form['yelp-sub_categories-' . $key] = array(
          '#type'           => 'fieldset', 
          '#title'         => $value[0] . ' sub-categories', 
          '#collapsible'   => TRUE, 
          '#collapsed'     => TRUE, 
        );
        _yelp_api_build_categories_form_fields($form['yelp-sub_categories-' . $key], $value[1], $selected);
      }
      else{
        $form['yelp-category-' . $key]['#title']  = $value;
      }
    }
  }
  
  /**
   * Create form elements to select yelp categories
   *
   * @param array &$form
   *  form object to add categories to, passed by reference.
   * @param array $selected
   *  array of selected categories
   *
   * @return nothing
   * this function returns nothing.
   */
  function yelp_api_build_form_categories(&$form, $selected=array()) {
    //get the list of categories
    if ($categories = yelp_api_get_categories()) {
      //passed a fieldset to add to?
      if (isset($form['#type']) && ($form['#type'] == 'fieldset')) {
        _yelp_api_build_categories_form_fields($form, $categories, $selected);
      //add a fieldset to the form  
      }
      else{
        $form['yelp-categories'] = array(
          '#type'         => 'fieldset', 
          '#title'       => t('Categories'), 
          '#collapsible' => TRUE, 
          '#collapsed'    => FALSE, 
          '#description' => t('Select Yelp categories.'), 
        );
        _yelp_api_build_categories_form_fields($form['yelp-categories'], $categories, $selected);
      }
    }
  }
  
  /**
   * Gets a list of the yelp categories that were selected in a form
   *
   * @param array $form_state
   *  Passed form state array
   * 
   * @return array
   *  array of selected yelp categories
   */
  function yelp_api_selected_form_categories($form_state) {
    $selected = array();
    if ($categories = yelp_api_get_categories(TRUE)) {
      foreach ($categories as $key  => $value) {
        if (isset($form_state['values']['yelp-category-' . $key]) && ($form_state['values']['yelp-category-' . $key] === $key)) {
          $selected[] = $key;
        };
      }
    }
    return $selected;
  }  