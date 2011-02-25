<?PHP

	/**
	 * Returns an administrative overview.
	 */
	function yelp_admin_overview() {
		$rows = array();
		//Get listing of all custom Yelp blocks
		$sql = "SELECT yid, nid, title FROM {yelp_blocks}";
		if($yelps = db_query($sql)){
			while($yelp = db_fetch_array($yelps)){
				$rows[] = array(
					l($yelp['title'],'node/'.$yelp['nid']),
					l('node/'.$yelp['nid'],'node/'.$yelp['nid']),
					l(t('edit'),'admin/content/yelp/edit/'.$yelp['yid']).' '.
					l(t('delete'),'admin/content/yelp/delete/'.$yelp['yid'])
				);
			}
		}else{
			$rows[] = array('','');
		}
		$headers = array(t('Yelp Block'),t('Node'), t('Operations'));
		return theme('table', $headers, $rows);
	}

	/**
	 * Builds and returns the settings form.
	 *
	 * @see yelp_admin_settings_validate()
	 *
	 * @ingroup forms
	 */	
	function yelp_admin_settings() {
		$form['yelp_api_key'] = array(
			'#type' => 'textfield',
			'#title' => t('Yelp API key'),
			'#size'=>50,
			'#maxlength'=>50,
			'#default_value' => variable_get('yelp_api_key',''),
			'#description' => t('Provide your API key (YWSID) that is registered to your URL (get an API key free '.l('here','http://www.yelp.com/developers/getting_started/api_access').')'),
			'#required' => TRUE,
		);
		$form['#validate'][] = 'yelp_admin_settings_validate';
		return system_settings_form($form);
	}
	
	/**
	 * Validate the yelp settings form.
	 *
	 * @see yelp_admin_settings()
	 **/
	function yelp_admin_settings_validate($form, &$form_state) {
		$key = $form_state['values']['yelp_api_key'];
		//Attempt a test API call
		$sample = json_decode(file_get_contents("http://api.yelp.com/business_review_search?location=02120&num_biz_requested=1&ywsid=".$key));
		//if Yelp returns the text 'OK', then the query worked
		if($sample->message->text!='OK'){
			//Else there was an error, display it to user
			form_set_Error('yelp_api_key',t($sample->message->text));
		}
	}
	
	/**
	 * Form to delete a Yelp block
	 **/
	function yelp_form_edit_block($yelp){
		//Make sure user has confirmed deleting block.
		if ((isset($_POST['op']) && $_POST['op'] == t('Delete')) || isset($_POST['confirm'])) {
			return drupal_get_form('yelp_confirm_delete_block', $yelp->yid);
		}
		return drupal_get_form('yelp_form_block', (array)$yelp);
	}
	
	/**
	 * Display form for editing/creating a Yelp Block
	 **/
	function yelp_form_block(&$form_state,$edit=array()){
		//Default values
		$edit += array(
			'title' => '',
			'nid' => '',
			'location' => '',
			'radius' => 10,
			'max' => 4,
			'categories' => '',
			'yid' =>NULL,
		);
		
		$form['#yelp'] = $edit;
		 
		 //Check for confirmation forms.
		if (isset($form_state['confirm_delete'])) {
			return array_merge($form, yelp_confirm_delete_block($form_state, $edit['yid']));
		}
		
		$form['yelp_block'] = array(
			'#type' => 'fieldset',
			'#title' => t('Yelp Block'),
			'#collapsible' => TRUE,
		);

		$form['yelp_block']['yid'] = array(
			'#type' => 'value',
			'#title' => t('yid'),
			'#default_value' => $edit['yid'] ? $edit['yid']:0,
		);
		
		$form['yelp_block']['title'] = array(
			'#type' => 'textfield',
			'#title' => t('Title'),
			'#default_value' => $edit['title'],
			'#maxlength' => 300,
			'#description' => t('Title for this block'),
			'#required' => TRUE);
			
		$form['yelp_block']['nid'] = array(
			'#type' => 'textfield',
			'#title' => t('Node'),
			'#default_value' => $edit['nid'],
			'#size' => 20,
			'#description' => t('NID of Node to display this block on.'),
			'#required' => TRUE);
 
		$form['yelp_block']['location'] = array(
			'#type' => 'textfield',
			'#title' => t('Location'),
			'#default_value' => $edit['location'],
			'#description' => t('Location to search from. Must be a valid address, or city, or zipcode, or state. (Example: 41 Winter St, Boston MA 02115)'),
			'#required' => TRUE);
			
		$form['yelp_block']['radius'] = array(
			'#type' => 'textfield',
			'#title' => t('Radius (miles)'),
			'#default_value' => $edit['radius'] ? $edit['radius']:10,
			'#size' => 20,
			'#description' => t('The radius in miles to search (max 50) (default 10)'),
			'#required' => TRUE);
		
		$form['yelp_block']['max'] = array(
			'#type' => 'textfield',
			'#title' => t('# of results'),
			'#default_value' => $edit['max'] ? $edit['max']:4,
			'#size' => 20,
			'#description' => t('Max number of results to return (default 4)'),
			'#required' => TRUE);

		$form['yelp_block']['category'] = array(
			'#type' =>'fieldset',
			'#title'=>t('Yelp Category'),
			'#description'=>t('Check the Yelp categories you would like to display results from'),
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#access' => true,
			'#weight' => 30,
		);
		
		//Next up is displaying category arrays, in proper hierarchy (with a recursive function)
		//Get the whole array of categories.
		$categories = yelp_get_categories();
		//if the yelp block has categories assigned to it already.
		if(strlen($edit['categories'])>1){
			//if multiple, split into an array
			if(strpos($edit['categories'],'+')>1) $selected_categories = explode('+',$edit['categories']);
			//if a single caytegory make into an array
			else $selected_categories = array($edit['categories']);
		//else create an empty array
		}else $selected_categories = array();
		
		
		//Build out categories checkboxes with a recursive function 
		yelp_build_form_categories($form['yelp_block']['category'],$categories,$selected_categories);
		
		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Save')
		);

		if($edit['yid']) {
			$form['delete'] = array(
				'#type' => 'submit',
				'#value' => t('Delete')
			);
			$form['yid'] = array(
				'#type' => 'value',
				'#value' => $edit['yid']
			);
		}else {
			$form['destination'] = array('#type' => 'hidden', '#value' => $_GET['q']);
		}
		
		$form['#validate'][] = 'yelp_form_block_validate';
		return $form;
	}
	
	/**
	 * Recursive form to build checkboxes for categories
	 **/
	function yelp_build_form_categories(&$form,$categories,$selected){
		$uncollapse = false;
		//for each category
		foreach($categories as $title=>$val){
			//If values is an array (sub-categories)
			if(is_array($val)){
				$sub_categories = $val[1];
				$val = $val[0];
			}else $sub_categories=  false;
			
			//check to see if category has been checked
			$checked = in_array($val,$selected) ? 1:0;
			
			//build checkbox.
			$form[$val] = array(
				'#type' 	=> 'checkbox',
				'#title'	=> t($title),
				'#return_value'	=> 1,
				'#default_value'=> $checked,
			);
			
			//if has a sub category, build it out
			if($sub_categories){
				$form[$title] = array(
					'#type' 	=> 'fieldset',
					'#title'	=> t($title.' sub categories'),
					'#collapsible'  => TRUE,
					'#collapsed'	=> TRUE,
					'#access'		=> TRUE,
				);
				
				//build out subcategories
				$sub_checked = yelp_build_form_categories($form[$title],$sub_categories,$selected);
				
				//see if a sub category was checked, if so uncollapse
				if($sub_checked) $form[$title]['#collapsed'] = FALSE;
			}else $sub_checked = false;
					
			//if category, or sub category was checked set uncollapse to true
			if($checked || $sub_checked) $uncollapse = true;	
		}

		//return value to tell parent to stay collapsed or not
		return $uncollapse;
	}
	
	/**
	* Validate the yelp block form.
	*
	* @see yelp_form_block()
	*/
	function yelp_form_block_validate($form, &$form_state) {
		$max = $form_state['values']['max'];
		if(!$max) $max = 4;
		if(!is_numeric($max)) form_set_Error('max',t('Number of results must be a number'));
		
		$radius = $form_state['values']['radius'];
		if(!$radius) $radius = 10;
		if(!is_numeric($radius)||$radius>50) form_set_Error('radius',t('Radius must be a value between 1 - 50 miles'));
		
		$nid = $form_state['values']['nid'];
		if(!node_load($nid)) form_set_Error('nid',t('Invalid Node Id (NID)'));
		
		if(!$form_state['values']['yid']){
			$sql = "SELECT yid FROM {yelp_blocks} WHERE `nid` = %d";
			$result = db_fetch_array(db_query($sql,$nid));
			if($result['yid']) form_set_Error('nid',t('This node already has a Yelp block'));
		}
	}
	
	/**
	 * Accept the form submission for a yelp block
	 **/
	function yelp_form_block_submit($form, &$form_state){
		switch (yelp_save_block($form_state['values'])) {
			case SAVED_NEW:
				drupal_set_message(t('Created New Yelp Block For Node %nid.', array('%nid' => $form_state['values']['nid'])));
				watchdog('yelp', 'Created New Yelp Block For Node %nid.',  array('%nid' => $form_state['values']['nid']), WATCHDOG_NOTICE, l(t('edit'), 'admin/content/yelp/edit/'. $form_state['values']['yid']));
				break;
			case SAVED_UPDATED:
				drupal_set_message(t('Updated Yelp Block For Node %nid.', array('%nid' => $form_state['values']['nid'])));
				watchdog('school', 'Updated Yelp Block For Node %nid.', array('%nid' => $form_state['values']['nid']), WATCHDOG_NOTICE, l(t('edit'), 'admin/content/yelp/edit/'. $form_state['values']['yid']));
			break;
		}

		$form_state['yid'] = $form_state['values']['yid'];
		$form_state['redirect'] = 'admin/content/yelp/list';
		return;
	}
	
	
	/**
	 * Sub step to delete a yelp (not from a form)
	 **/
	function yelp_pre_delete_block($yelp){
		return drupal_get_form('yelp_confirm_delete_block',$yelp->yid);
	}
	
	/**
	 * Form builder for the school delete confirmation form.
	 *
	 * @ingroup forms
	 * @see yelp_confirm_delete_block_submit()
	 **/
	function yelp_confirm_delete_block(&$form_state, $yid) {
		$yelp = yelp_block_load($yid);

		$form['type'] = array('#type' => 'value', '#value' => 'yelp');
		$form['yid'] = array('#type' => 'value', '#value' => $yid);
		$form['title'] = array('#type' => 'value', '#value' => $yelp->title);
		return confirm_form($form,
            t('Are you sure you want to delete the yelp block %title?',
            array('%title' => $yelp->title)),
            'admin/content/yelp',
            t('This action cannot be undone.'),
            t('Delete'),
            t('Cancel'));
	}
	
	/**
	 * Submit handler to delete a school after confirmation.
	 *	
	 * @see yelp_confirm_delete_block()
	 **/
	function yelp_confirm_delete_block_submit($form, &$form_state) {
		yelp_delete_block($form_state['values']['yid']);
		drupal_set_message(t('Deleted Yelp Block  %title.', array('%title' => $form_state['values']['title'])));
		watchdog('yelp', 'Deleted Yelp Block  %title.', array('%title' => $form_state['values']['title']), WATCHDOG_NOTICE);
		$form_state['redirect'] = 'admin/content/yelp/list';
		return;
	}