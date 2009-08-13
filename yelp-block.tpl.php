<?php
// $Id$

/**
 * @file yelp-block.tpl.php
 * Default theme implementation for displaying a yelp reviews block
 *
 * Available variables:
 *	- $block: the yelp block settings
 *	- $block->title ; title of block
 *	- $block->location	; the address given
 *  - $block->radius;	radius for block
 *	- $block->max;		Max number of results to return
 *	- $block->found;	the number of results returned
 *	- $block->businesses;	the businesses that were returned
 *	= $block->categories;	string value of categories for block
 *	- $delta: yelp block delta number
 *
 */
?>
<div id="yelp_block-<?php echo $delta; ?>">
	<b><?php echo $block->title; ?></b><br />
	<span><i>(found <?php echo $block->found; ?> businesses)</i></span>
	<?php if($block->businesses){ ?>
		<div id="yelp_block-businesses">
			<?php echo $block->businesses; ?>
		</div>
	<?php } ?>
</div>