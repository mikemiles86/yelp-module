<?php
// $Id$

/**
 * @file yelp-business.tpl.php
 * Default theme implementation for displaying a yelp-business
 *
 * Available variables:
 *	- $business: the review object
 *		- $business->id: yelp.com id
 *  	- $business->mobile_url: url for mobile yelp.com of business 
 *  	- $business->avg_rating: numeric rating of business
 *		- $business->rating_img_url: url of ratings image
 *  	- $business->rating_img_url_small: smaller version of rating image 
 *  	- $business->name: name of business
 *  	- $business->url: url of business on yelp.com
 *  	- $business->photo_url:	url of business photo
 *  	- $business->photo_url_small:	url of smaller business photo
 *		- $business->is_closed: boolean value is business is closed or open
 *  	- $business->phone: business phone number
 *  	- $business->address1: address line 1
 *  	- $business->address2: address line 2
 *  	- $business->address3: address line 3
 *  	- $business->city: city business is located in
 *  	- $business->state: state of business
 *  	- $business->state_code: state code
 *  	- $business->zip: zip code of business
 *		- $business->country: country business is located in
 *		- $business->country_code: country business is located in
 *  	- $business->latitude: latitude of business location 
 *  	- $business->longitude: business longitude location
 *  	- $business->distance: distance in miles from given location
 *  	- $business->review_count: number of reviews for businesses
 *		- $business->categories: array of objects for categories business is in
 *  		- $business->categories->category_filters: category query name
 *  		- $business->categories->search_url: url to search yelp by the category
 *  		- $business->categories->name:	name of category
 *  	- $business->neighborhoods: array of neighborhoods business is in
 *  	- $business->reviews: array of review objects for this business
 *  		- $business->reviews->user_url: yelp url of reviewer account info
 *  		- $business->reviews->url: yelp url of review
 *  		- $business->reviews->mobile_urli: mobile yelp url of review
 *  		- $business->reviews->rating: numeric version of rating given by user
 *  		- $business->reviews->rating_img_url: url of image of rating giver by reviewer
 *  		- $business->reviews->rating_img_url_small: url of smaller image of rating giver by reviewer
 *  		- $business->reviews->user_photo: url of image of reviewer
 *  		- $business->reviews->text_excerpt: excerpt of review
 *  		- $business->reviews->date: date review was written
 *  		- $business->reviews->user_name: name of reviewer
 *  		- $business->reviews->id: id of review
 *  	- $business->nearby_url: url of businesses nearby this business
 *	- $delta: review delta number
 *
 */
?>
<div id="yelp_business-<?php echo $delta; ?>">
	<table border="0">
		<tr>
			<td valign="top" style="width:100px">
				<a href="<?php echo $business->url; ?>" target="_blank">
					<img border=0 src="<?php echo $business->photo_url; ?>" />
				</a>
			</td>
			<td valign="top">
				<b><?php echo l($business->name,$business->url); ?></b><br />
				<img src="<?php echo $business->rating_img_url; ?>" /><br />
				<span>
					<?php echo $business->address1,'<br />',$business->city,', ',$business->state,' ',$business->zip; ?><br />
					<?php echo $business->phone; ?>
				</span>
			</td>
		</tr>
	</table>
</div>