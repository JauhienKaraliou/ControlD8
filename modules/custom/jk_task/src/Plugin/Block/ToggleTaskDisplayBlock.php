<?php

namespace Drupal\jk_assign_task\ToggleTaskDisplayBlock;

use Drupal\Core\Block\BlockBase;

/**
 * Provides an 'employee list' block.
 *
 * @Block(
 *   id = "toggle_task_display_block",
 *   admin_label = @Translation("Toggle Task Display"),
 *   category = @Translation("Toggle Task Display Block")
 * )
 */
class ToggleTaskDisplayBlock {

  /**
   * {@inheritdoc}
   */
  public function build() {
      return array(
        '#markup' => '<div class="modes"><span class="table_mode">Table Mode</span> | <span class="list_mode">List mode</span> </div>'
      );
  }
}