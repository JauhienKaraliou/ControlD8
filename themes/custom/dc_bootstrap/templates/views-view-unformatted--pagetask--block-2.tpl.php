<?php

/**
 * @file
 * Default simple view template to display a list of rows.
 *
 * @ingroup views_templates
 */
?>
<?php if (!empty($title)): ?>
 <h3 class="row" style="clear: both;"><?php print $title; ?></h3>
<?php endif; ?>
<?php foreach ($rows as $id => $row): ?>
 <div<?php if ($classes_array[$id]) { print ' class="' . $classes_array[$id] .'"';  } ?> style="width: 23.4%; margin-right: 15px; height: 80px">
  <?php print $row; ?>
 </div>
<?php endforeach; ?>
